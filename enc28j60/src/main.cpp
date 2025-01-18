#include <EthernetESP32.h>

ENC28J60Driver driver(10,9);

void setup()
{

  // SPI.setFrequency(80000);
  Serial.begin(115200);

  Serial.println("Initializing Ethernet...");
  Ethernet.init(driver);

  Ethernet.begin(); // begin with default MAC address and DHCP IP
  Serial.println("Ethernet initialized");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  // Your loop code here
}