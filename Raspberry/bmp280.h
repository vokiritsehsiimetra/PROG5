#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>

// BMP280 class for interfacing with the BMP280 sensor
class BMP280 {
public:
    // Constructor: Initializes the BMP280 object with the specified I2C bus and address
    BMP280(int i2c_bus, int address);
    
    // Destructor: Cleans up resources when the BMP280 object is destroyed
    ~BMP280();

    // Initializes the BMP280 sensor and configures it for operation
    bool begin();
    
    // Reads and returns the temperature in degrees Celsius
    float readTemperature();
    
    // Reads and returns the pressure in hectopascals (hPa)
    float readPressure();

private:
    int i2c_handle;  // Handle for the I2C communication
    int i2c_bus;     // I2C bus number
    int address;     // I2C address of the BMP280 sensor
    int32_t t_fine;  // Fine temperature value for compensation calculations

    // Calibration parameters for temperature compensation
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;

    // Calibration parameters for pressure compensation
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

    // Writes a single byte to the specified register on the BMP280
    void writeRegister(uint8_t reg, uint8_t value);
    
    // Reads a 24-bit value from a given register on the BMP280
    uint32_t read24(uint8_t reg);
    
    // Reads and stores calibration data from the BMP280
    void readCalibrationData();
    
    // Compensates the raw temperature reading to get a corrected temperature value
    int32_t compensateTemperature(int32_t adc_T);
    
    // Compensates the raw pressure reading to get a corrected pressure value
    uint32_t compensatePressure(int32_t adc_P);
};

#endif // BMP280_H
