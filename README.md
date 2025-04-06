# TMP102 Temperature Sensor Driver

## Overview
This project implements a C++ driver for the TMP102, a digital temperature sensor that communicates over the I2C protocol. The driver is designed for firmware development and includes a simulation mode to test functionality without hardware. It’s a beginner-friendly example of an embedded systems driver, focusing on modularity and real-world applicability.

The simulation generates temperature readings within the TMP102’s realistic range (-40°C to 125°C), mimicking how the sensor would behave in an actual embedded environment.

---

## Features
- **I2C Interface**: Abstract base class for I2C communication, allowing easy swaps between simulated and real hardware implementations.
- **Temperature Reading**: Converts 12-bit raw data to Celsius, supporting the TMP102’s full range (-40°C to 125°C).
- **Simulation Mode**: Runs on a desktop without hardware, generating realistic temperature values.
- **Error Handling**: Basic checks for I2C read failures.
- **Timestamped Output**: Displays readings with current time for monitoring.

---

## Prerequisites
- **Compiler**: A C++11-compatible compiler (e.g., MinGW `g++`).
- **OS**: Tested on Windows 10; should work on Linux/macOS with minor adjustments.
- **Hardware (Optional)**: For real deployment, an I2C-capable microcontroller (e.g., Arduino, STM32) and a TMP102 sensor.

---

## Installation
1. **Clone or Download**  
   - Clone this repository or download the source files:
     ```bash
     git clone https://github.com/yourusername/tmp102-driver.git
     ```
   - Alternatively, copy `code.cpp` from this documentation.

2. **Directory Structure**  
   - Place `code.cpp` in your project folder (e.g., `C:\path\to\drivercode\`).
   - Create an `output` subfolder for the executable (e.g., `C:\path\to\drivercode\output\`).

3. **Compile**  
   - Open a command prompt and navigate to your project folder:
     ```bash
     cd C:\path\to\drivercode
     ```
   - Compile with `g++`:
     ```bash
     C:\MinGW\bin\g++.exe -Wall -Wextra -g3 code.cpp -o output\code.exe
     ```

---

## Usage
1. **Run the Simulation**  
   - Navigate to the output folder:
     ```bash
     cd C:\path\to\drivercode\output
     ```
   - Execute the program:
     ```bash
     .\code.exe
     ```
   - Optional: Set UTF-8 encoding for proper °C display:
     ```bash
     chcp 65001
     .\code.exe
     ```

2. **Expected Output**  
   - The program continuously prints temperature readings every second:
     ```
     Starting TMP102 Temperature Driver...
     [Sun Apr 06 16:45:00 2025] Temperature: 23.75 °C
     [Sun Apr 06 16:45:01 2025] Temperature: -5.25 °C
     [Sun Apr 06 16:45:02 2025] Temperature: 87.12 °C
     ```

3. **Stop the Program**  
   - Press `Ctrl+C` in the command prompt to exit.

---

## Code Structure
- **`I2CInterface`**: Abstract base class defining `write` and `read` methods for I2C communication.
- **`SimulatedI2C`**: Concrete class simulating I2C reads/writes with random temperature data (-40°C to 125°C).
- **`TMP102Driver`**: Main driver class handling sensor initialization and temperature conversion.
- **`main()`**: Runs an infinite loop to test the driver.

---

## Porting to Hardware
To use this with a real TMP102 sensor:
1. **Replace `SimulatedI2C`**  
   - Implement `I2CInterface` for your microcontroller’s I2C library (e.g., Arduino’s `Wire`).
   - Example for Arduino:
     ```cpp
     #include <Wire.h>
     class ArduinoI2C : public I2CInterface {
     public:
         ArduinoI2C() { Wire.begin(); }
         bool write(uint8_t address, uint8_t reg, const uint8_t* data, size_t length) override {
             Wire.beginTransmission(address);
             Wire.write(reg);
             Wire.write(data, length);
             return Wire.endTransmission() == 0;
         }
         bool read(uint8_t address, uint8_t reg, uint8_t* data, size_t length) override {
             Wire.beginTransmission(address);
             Wire.write(reg);
             Wire.endTransmission(false);
             Wire.requestFrom(address, length);
             for (size_t i = 0; i < length; i++) data[i] = Wire.read();
             return true;
         }
     };
     ```

2. **Adjust `main()`**  
   - Initialize the hardware I2C and pass it to `TMP102Driver`.
   - Output via serial (e.g., `Serial.println()`).

3. **Compile and Upload**  
   - Use your microcontroller’s IDE (e.g., Arduino IDE, STM32CubeIDE).

---

## License
This project is open-source under the [MIT License](LICENSE). Use it freely, modify it, and share it!

---
