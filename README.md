# server-communication-esp32-lilyGo-T-Call

## Arduino Project: HTTP GET Requests with a GSM Module

This Arduino project configures a GSM module to perform HTTP GET requests to a server.

## Author

M'Crai Laydam

## Date

08/04/2024

## Hardware Used

- ESP32 TTGO T-Call
- GSM Module SIM800
- Additional sensors and modules (if needed)

## Description

This code configures a GSM module to connect to a GPRS network and send an HTTP GET request to a specified server. The server's response information is displayed on the serial monitor.

## Connection Diagram

Here are the essential connections for the project:

- **GSM Module**:
  - `MODEM_RST`: GPIO 5
  - `MODEM_PWKEY`: GPIO 4
  - `MODEM_POWER_ON`: GPIO 23
  - `MODEM_TX`: GPIO 27
  - `MODEM_RX`: GPIO 26

- **I2C Pins** (to keep the SIM800 module running):
  - `I2C_SDA`: GPIO 21
  - `I2C_SCL`: GPIO 22

- **Outputs**:
  - `OUTPUT_1`: GPIO 13
  - `OUTPUT_2`: GPIO 15

## Installation

1. **Hardware Setup**:
   - Connect the GSM module to the ESP32 TTGO T-Call following the connections described above.

2. **Software Setup**:
   - Download and install the Arduino IDE if not already installed.
   - Add the `TinyGsmClient` library to your Arduino IDE. This can be done via the library manager in the IDE.
   - Open the `.ino` file of this project in the Arduino IDE.

3. **APN Settings**:
   - Make sure to update the APN settings in the code to match your service provider.
   ```cpp
   const char apn[] = "internet"; // APN (ORANGE: orangenet, TELMA: internet, AIRTEL: internet)
