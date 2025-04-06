#include <iostream>
#include <random>
#include <chrono>
#include <iomanip>
#include <cstdint>

// Simulated I2C interface (abstract base class)
class I2CInterface {
public:
    virtual bool write(uint8_t address, uint8_t reg, const uint8_t* data, size_t length) = 0;
    virtual bool read(uint8_t address, uint8_t reg, uint8_t* data, size_t length) = 0;
    virtual ~I2CInterface() = default;
};

// Simulated I2C bus for TMP102
class SimulatedI2C : public I2CInterface {
public:
    SimulatedI2C() {
        std::random_device rd;
        gen = std::mt19937(rd());
        dist = std::uniform_int_distribution<>(-640, 2000); // -40°C to 125°C (raw values)
    }

    bool write(uint8_t address, uint8_t reg, const uint8_t* data, size_t length) override {
        (void)address;
        (void)reg;
        (void)data;
        (void)length;
        return true;
    }

    bool read(uint8_t address, uint8_t reg, uint8_t* data, size_t length) override {
        (void)address;
        if (reg == 0x00 && length == 2) {
            int16_t rawTemp = dist(gen); // Simulate realistic raw value
            if (rawTemp < 0) {
                rawTemp &= 0xFFF; // Mask to 12 bits for negative values
            }
            data[0] = (rawTemp >> 4) & 0xFF; // High byte
            data[1] = (rawTemp << 4) & 0xF0; // Low byte (TMP102 format)
            return true;
        }
        return false;
    }

private:
    std::mt19937 gen;
    std::uniform_int_distribution<> dist; // Raw values: -640 (-40°C) to 2000 (125°C)
};

// TMP102 Driver
class TMP102Driver {
public:
    TMP102Driver(I2CInterface& i2c, uint8_t address = 0x48)
        : i2c_(i2c), address_(address) {
        initialize();
    }

    bool initialize() {
        uint8_t config[2] = {0x60, 0xA0}; // Default config: 12-bit, continuous
        return i2c_.write(address_, 0x01, config, 2);
    }

    bool readTemperature(float& temperature) {
        uint8_t data[2];
        if (!i2c_.read(address_, 0x00, data, 2)) {
            return false;
        }

        int16_t raw = (data[0] << 4) | (data[1] >> 4);
        if (raw & 0x800) { // Sign extend for negative temps
            raw |= 0xF000;
        }

        temperature = raw * 0.0625f;
        return true;
    }

private:
    I2CInterface& i2c_;
    uint8_t address_;
};

// Get timestamp
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::string ts = std::ctime(&time);
    ts.pop_back();
    return ts;
}

// Simple busy-wait delay
void delay(int milliseconds) {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now() - start).count() < milliseconds) {
        // Busy wait
    }
}

int main() {
    SimulatedI2C i2c;
    TMP102Driver sensor(i2c);

    std::cout << "Starting TMP102 Temperature Driver...\n";

    while (true) {
        float temp;
        if (sensor.readTemperature(temp)) {
            std::cout << "[" << getTimestamp() << "] Temperature: " 
                      << std::fixed << std::setprecision(2) << temp << " °C\n";
        } else {
            std::cout << "[" << getTimestamp() << "] Error: Failed to read sensor\n";
        }

        delay(1000); // 1 second
    }

    return 0;
}