#include <Arduino.h>
#include <CD74HC4067.h>

// Initialize the CD74HC4067 with control pins (S0, S1, S2, S3)
CD74HC4067 mux(4, 5, 6, 7);  // Control pins: S0=4, S1=5, S2=6, S3=7

// H-bridge control pins through multiplexer
const int MOTOR_FWD = 0;  // Forward control on mux channel 0
const int MOTOR_REV = 1;  // Reverse control on mux channel 1
const int SIG_PIN = 15;   // Common (SIG) pin
const int MAX_MOTOR_SPEED = 255;  // Maximum PWM value
const int CHANNEL_DELAY = 0;  // Delay in ms between channel switches

void setup() {
  Serial.begin(115200);
  Serial.println("Starting motor control system...");
  
  // Configure SIG pin as output
  pinMode(SIG_PIN, OUTPUT);
  Serial.println("SIG pin configured as output");
}

void loop() {
  // Example motor control sequence
  
  // Move UP
  Serial.println("Moving UP...");
  // First set both channels to 0
  mux.channel(MOTOR_FWD);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, 0);
  mux.channel(MOTOR_REV);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, 0);
  delay(CHANNEL_DELAY);
  
  // Then set the direction
  mux.channel(MOTOR_REV);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, MAX_MOTOR_SPEED);
  delay(2000);
  
  // Stop - set both to 0
  Serial.println("Stopping...");
  mux.channel(MOTOR_FWD);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, 0);
  mux.channel(MOTOR_REV);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, 0);
  delay(1000);
  
  // Move DOWN
  Serial.println("Moving DOWN...");
  // First set both channels to 0
  mux.channel(MOTOR_FWD);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, 0);
  mux.channel(MOTOR_REV);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, 0);
  delay(CHANNEL_DELAY);
  
  // Then set the direction
  mux.channel(MOTOR_FWD);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, MAX_MOTOR_SPEED);
  delay(2000);
  
  // Stop again - set both to 0
  Serial.println("Stopping...");
  mux.channel(MOTOR_FWD);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, 0);
  mux.channel(MOTOR_REV);
  delay(CHANNEL_DELAY);
  analogWrite(SIG_PIN, 0);
  delay(1000);

  Serial.println("-------------------");
}