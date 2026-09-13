# ESP32_Heart_Rate_and_SpO2_Monitor

Heart rate and SpO2 monitor with an OLED display and Wi-Fi web dashboard utilizing an ESP32
This project is a functional heart rate and SpO2 monitor, implemented using an ESP32 microcontroller. The microcontroller collects the readings and displays them on a 0.96" OLED display, while also sending the data to a local website that updates as new data comes in. Additionally, LEDs provide visual indicators for the validity of the readings. 

## Features

- Measures heart rate and SpO2 using a MAX30105 sensor
- Filters readings outside of defined valid ranges
- Displays the last valid readings on an OLED
- Uses LED indicators to visually show reading validity
- Sends readings to a local Wi-Fi dashboard
- Displays recent readings using live graphs

## Hardware Used
- ESP32 DevKitM-1
- 0.96" I2C OLED display
- MAX30102 pulse oximeter sensor
- LEDs
- Resistors
- Breadboard and jumper wires

## Libraries Used
- [ESP32 Arduino Core by Espressif](https://github.com/espressif/arduino-esp32)
- [SparkFun MAX3010x Sensor Library](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [ESP32Dashboard](https://github.com/ErfanGanji87/ESP32Dashboard)
- Install these via Arduino IDE

## Circuit Diagram
<img width="1470" height="930" alt="Image" src="https://github.com/user-attachments/assets/955f636a-0dcd-42c0-bd57-6deaff9d5924" />

## Project Preview
<img width="1918" height="911" alt="Image" src="https://github.com/user-attachments/assets/e5283225-70c1-4bd8-aeca-c770740c5443" />
<img width="1918" height="911" alt="Image" src="https://github.com/user-attachments/assets/993c27f0-dbec-4d80-95da-46e2e28c27aa" />

## Setup
- 1. Install the libraries above through the Arduino IDE.
- 2. Connect the components according to the circuit diagram.
- 3. Open the '.ino' file in Arduino IDE.
- 4. Enter your Wi-Fi name and password in the code.
- 5. Select the ESP32 board and upload the code.
- 6. Place a finger firmly on the MAX30105 sensor to begin taking readings. 

## Liscense
This project is licensed under the [MIT License](LICENSE).
