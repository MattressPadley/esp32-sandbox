# Community Links

## EthernetESP32 Library Issues

### Network.h Dependency Issue
[PlatformIO Forum Thread](https://community.platformio.org/t/issue-with-the-networking-for-arduino-ethernetesp32-library/41967)
- Issue: EthernetESP32 library has undeclared dependency on Network.h
- ESP32-S3 projects affected
- Network.h not found in PlatformIO Registry
- Manual copying of Network library from Arduino core causes errors
- WiFi projects unaffected as they don't require Network library

### Arduino 3.x Dependency

- Library requires Arduino 3.x framework
- Arduino 3.x not officially available in PlatformIO yet
- Workaround: Use pioarduino's custom platform
- Platform URL: `https://github.com/pioarduino/platform-espressif32/releases/download/51.03.03/platform-espressif32.zip`
- Add to platformio.ini under `platform =`

