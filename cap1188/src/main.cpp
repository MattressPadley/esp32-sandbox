#include <Arduino.h>
#include <Wire.h>

// =========================
// User-Editable Variables
// =========================

// I2C address for your CAP1188
static const uint8_t CAP1188_ADDRESS = 0x29;

// SDA/SCL pins on your ESP32
static const int I2C_SDA = 4;
static const int I2C_SCL = 5;

// Increase threshold to reduce false touches
static uint8_t thresholdValue = 0x70;
// Lower sensitivity for fewer false touches (default is around 0x2F)
static uint8_t sensitivityValue = 0x4F;

// A comfortable "Pulse 1" total period, in 32ms increments
// e.g., 0x20 => 32 decimal * 32ms = ~1 second total for each pulse cycle
static uint8_t pulse1Period = 0x20;

// Number of pulses to do before returning to idle
// 0x88 register has [PULSE2_CNT(3 bits)][PULSE1_CNT(3 bits)]
// For instance, if we want Pulse1 count = 5 pulses => bits = 100 => 0x04 in that field
// And let's keep Pulse2 count = default (like 1) in the top bits, so 0x28 or 0x44, etc.
// We'll just write everything so that PULSE1_CNT is 5, PULSE2_CNT is 1 for example.
static uint8_t ledConfigValue = 0x24; // Pulse2_Cnt=001 => 1 pulse, Pulse1_Cnt=100 => 5 pulses

// =========================
// CAP1188 Register Addresses
// =========================
#define REG_MAIN_CONTROL 0x00
#define REG_SENSOR_INPUT_EN 0x21
#define REG_SENSOR_INPUT_STATUS 0x03
#define REG_CALIBRATION_ACTIVATE 0x26
#define REG_SENSITIVITY_CONTROL 0x1F
#define REG_SENSOR_INPUT_LED_LINK 0x72
#define REG_LED_OUTPUT_TYPE 0x71
#define REG_LED_POLARITY 0x73
#define REG_LED_BEHAVIOR_1 0x81 // LEDs 1..4
#define REG_LED_BEHAVIOR_2 0x82 // LEDs 5..8
#define REG_LED_PULSE1_PERIOD 0x84
#define REG_LED_CONFIG 0x88
#define REG_LED_OUTPUT_CONTROL 0x74
// Thresholds for channels 1..8 => 0x30..0x37

// ---------------------------------
// Write a single byte over I2C
// ---------------------------------
void writeRegister8(uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(CAP1188_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// ---------------------------------
// Read a single byte over I2C
// ---------------------------------
uint8_t readRegister8(uint8_t reg)
{
  Wire.beginTransmission(CAP1188_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((int)CAP1188_ADDRESS, 1);
  if (Wire.available())
  {
    return Wire.read();
  }
  return 0;
}

// ---------------------------------
// Simple debug: read Sensor Input Status
// ---------------------------------
void checkTouches()
{
  uint8_t status = readRegister8(REG_SENSOR_INPUT_STATUS);
  if (status)
  {
    Serial.print("Touch status: 0x");
    Serial.print(status, HEX);
    Serial.print(" => ");
    for (int i = 0; i < 8; i++)
    {
      if (status & (1 << i))
      {
        Serial.print("CS");
        Serial.print(i + 1);
        Serial.print(" ");
      }
    }
    Serial.println();
  }
  else
  {
    Serial.println("No touches.");
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.println("CAP1188 All-LED-Pulse1 Setup...");

  // 1) Force calibration so we start from a clean baseline
  writeRegister8(REG_CALIBRATION_ACTIVATE, 0xFF);
  delay(500);

  // 2) Enable all 8 sensor inputs
  writeRegister8(REG_SENSOR_INPUT_EN, 0xFF);

  // 3) Set global sensitivity
  writeRegister8(REG_SENSITIVITY_CONTROL, sensitivityValue);

  // 4) Set thresholds for all 8 channels
  for (uint8_t regAddr = 0x30; regAddr <= 0x37; regAddr++)
  {
    writeRegister8(regAddr, thresholdValue);
  }

  // 5) Link each channel to its LED
  //    0xFF => CS1->LED1, CS2->LED2, ... CS8->LED8
  writeRegister8(REG_SENSOR_INPUT_LED_LINK, 0xFF);

  // 6) Set LED output type:
  //    0x00 = open-drain, 0xFF = push-pull for all
  //    Choose whichever your hardware requires.
  writeRegister8(REG_LED_OUTPUT_TYPE, 0x00); // open-drain

  // 7) LED polarity:
  //    Typically 0 => "inverted" for sink.
  //    If your LED is from VDD->LED->pin, you want '0' so a logic "1" pulls the pin low to turn on.
  writeRegister8(REG_LED_POLARITY, 0x00);

  // 8) Set LED Behavior to Pulse 1 (which is '01' in the 2-bit field).
  //    Each nibble is 2 bits per LED.
  //    For 4 LEDs in a register:
  //      LED4_CTL(7:6) LED3_CTL(5:4) LED2_CTL(3:2) LED1_CTL(1:0)
  //    '01' repeated => 0x55
  writeRegister8(REG_LED_BEHAVIOR_1, 0x55); // LED1..4 => Pulse1
  writeRegister8(REG_LED_BEHAVIOR_2, 0x55); // LED5..8 => Pulse1

  // 9) Set PULSE 1 Period register (0x84).
  //    B7 => ST_TRIG (0= start on touch, 1= start on release)
  //    Lower 7 bits => 32ms increments
  //    We'll keep ST_TRIG=0, so just store pulse1Period in bits [6:0].
  writeRegister8(REG_LED_PULSE1_PERIOD, pulse1Period);

  // 10) Set the LED Config register (0x88) to define how many times to pulse, etc.
  //     Lower 3 bits => PULSE1_CNT, upper 3 bits => PULSE2_CNT
  //     e.g., 0x24 => PULSE2_CNT=001 (1 pulse), PULSE1_CNT=100 (5 pulses).
  writeRegister8(REG_LED_CONFIG, ledConfigValue);

  // Optionally, you can set the Min/Max Duty Cycle for Pulse 1 in regs 0x90 (Pulse1 Duty),
  // which defaults to 0xF0 => min=0%, max=100%. That’s often fine.

  Serial.println("All channels set to Pulse1 behavior. Ready!");
  
}

void loop()
{
  // Just poll the status every second for demonstration
  static uint32_t lastCheck = 0;
  if (millis() - lastCheck > 1000)
  {
    lastCheck = millis();
    checkTouches();
  }
}