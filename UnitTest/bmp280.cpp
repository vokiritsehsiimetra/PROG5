#include "bmp280.h"
#include <pigpio.h>
#include <iostream>
#include <unistd.h>

// BMP280 Register definitions
#define BMP280_REG_TEMP_XLSB 0xFC
#define BMP280_REG_TEMP_LSB  0xFB
#define BMP280_REG_TEMP_MSB  0xFA
#define BMP280_REG_PRESS_XLSB 0xF9
#define BMP280_REG_PRESS_LSB  0xF8
#define BMP280_REG_PRESS_MSB  0xF7
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG    0xF5

// Constructor: initializes the BMP280 object with specified I2C bus and address
BMP280::BMP280(int i2c_bus, int address) : i2c_bus(i2c_bus), address(address), i2c_handle(-1) {}

// Destructor: closes the I2C handle if it was successfully opened
BMP280::~BMP280() {
    if (i2c_handle >= 0) {
        i2cClose(i2c_handle);
    }
}

// Initializes the BMP280 sensor, configures it, and reads calibration data
bool BMP280::begin() {
    if (gpioInitialise() < 0) { // Initialize the pigpio library
        std::cerr << "pigpio initialization failed" << std::endl;
        return false;
    }

    i2c_handle = i2cOpen(i2c_bus, address, 0); // Open I2C communication
    if (i2c_handle < 0) {
        std::cerr << "Failed to open I2C" << std::endl;
        return false;
    }

    // Read the sensor's calibration data from memory registers
    readCalibrationData();

    // Configure measurement control and configuration registers for normal operation
    writeRegister(BMP280_REG_CTRL_MEAS, 0x27); // Set normal mode, oversampling rate for temp and pressure to 1
    writeRegister(BMP280_REG_CONFIG, 0xA0);    // Set standby time to 1000 ms

    sleep(1); // Wait for sensor to stabilize

    return true;
}

// Reads a 24-bit value from a given register (for temperature and pressure data)
uint32_t BMP280::read24(uint8_t reg) {
    uint32_t value = i2cReadByteData(i2c_handle, reg) << 16;
    value |= i2cReadByteData(i2c_handle, reg + 1) << 8;
    value |= i2cReadByteData(i2c_handle, reg + 2);
    return value;
}

// Reads and compensates temperature data from the sensor
float BMP280::readTemperature() {
    int32_t adc_T = read24(BMP280_REG_TEMP_MSB); // Read the raw temperature value
    adc_T >>= 4; // Shift to correct the format
    int32_t temp = compensateTemperature(adc_T); // Apply compensation formula
    return temp / 100.0; // Convert to degrees Celsius
}

// Reads and compensates pressure data from the sensor
float BMP280::readPressure() {
    int32_t adc_P = read24(BMP280_REG_PRESS_MSB); // Read the raw pressure value
    adc_P >>= 4; // Shift to correct the format
    uint32_t pressure = compensatePressure(adc_P); // Apply compensation formula
    return pressure / 25600.0; // Convert to hPa
}

// Reads and stores the calibration data needed for temperature and pressure compensation
void BMP280::readCalibrationData() {
    dig_T1 = i2cReadWordData(i2c_handle, 0x88);
    dig_T2 = i2cReadWordData(i2c_handle, 0x8A);
    dig_T3 = i2cReadWordData(i2c_handle, 0x8C);
    dig_P1 = i2cReadWordData(i2c_handle, 0x8E);
    dig_P2 = i2cReadWordData(i2c_handle, 0x90);
    dig_P3 = i2cReadWordData(i2c_handle, 0x92);
    dig_P4 = i2cReadWordData(i2c_handle, 0x94);
    dig_P5 = i2cReadWordData(i2c_handle, 0x96);
    dig_P6 = i2cReadWordData(i2c_handle, 0x98);
    dig_P7 = i2cReadWordData(i2c_handle, 0x9A);
    dig_P8 = i2cReadWordData(i2c_handle, 0x9C);
    dig_P9 = i2cReadWordData(i2c_handle, 0x9E);
}

// Temperature compensation algorithm (provided by BMP280 datasheet) for more accurate readings
int32_t BMP280::compensateTemperature(int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2; // Update t_fine for pressure compensation
    T = (t_fine * 5 + 128) >> 8; // Final temperature in hundredths of °C
    return T;
}

// Pressure compensation algorithm (provided by BMP280 datasheet) for more accurate readings
uint32_t BMP280::compensatePressure(int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    if (var1 == 0) {
        return 0; // Avoid division by zero error
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4); // Final pressure in Pa
    return (uint32_t)p;
}

// Writes a single byte to a specified register on the BMP280
void BMP280::writeRegister(uint8_t reg, uint8_t value) {
    i2cWriteByteData(i2c_handle, reg, value);
}
