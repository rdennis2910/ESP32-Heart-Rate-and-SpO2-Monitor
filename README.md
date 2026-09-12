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
