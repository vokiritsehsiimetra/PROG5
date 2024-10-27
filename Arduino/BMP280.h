#ifndef BMP280_H
#define BMP280_H

#include <SPI.h>

#define BMP280_REGISTER_CHIPID 0xD0
#define BMP280_REGISTER_CONTROL 0xF4
#define BMP280_REGISTER_CONFIG 0xF5
#define BMP280_REGISTER_PRESSUREDATA 0xF7
#define BMP280_REGISTER_TEMPDATA 0xFA
#define BMP280_REGISTER_SOFTRESET 0xE0
#define BMP280_REGISTER_STATUS 0xF3
#define BMP280_REGISTER_DIG_T1 0x88
#define BMP280_REGISTER_DIG_T2 0x8A
#define BMP280_REGISTER_DIG_T3 0x8C
#define BMP280_REGISTER_DIG_P1 0x8E
#define BMP280_REGISTER_DIG_P2 0x90
#define BMP280_REGISTER_DIG_P3 0x92
#define BMP280_REGISTER_DIG_P4 0x94
#define BMP280_REGISTER_DIG_P5 0x96
#define BMP280_REGISTER_DIG_P6 0x98
#define BMP280_REGISTER_DIG_P7 0x9A
#define BMP280_REGISTER_DIG_P8 0x9C
#define BMP280_REGISTER_DIG_P9 0x9E

typedef struct {
    int32_t version;
    int32_t sensor_id;
    int32_t type;
    int32_t reserved0;
    int32_t timestamp;
    union {
        float data[4];
        struct {
            float x;
            float y;
            float z;
        } acceleration;
        struct {
            float x;
            float y;
            float z;
        } magnetic;
        struct {
            float roll;
            float pitch;
            float heading;
        } orientation;
        struct {
            float x;
            float y;
            float z;
        } gyro;
        float temperature;
        float distance;
        float light;
        float pressure;
        float relative_humidity;
        float current;
        float voltage;
        float tvoc;
        float voc_index;
        float nox_index;
        float CO2;
        float eCO2;
        float pm10_std;
        float pm25_std;
        float pm100_std;
        float pm10_env;
        float pm25_env;
        float pm100_env;
        float gas_resistance;
        float unitless_percent;
        struct {
            float r;
            float g;
            float b;
        } color;
        float altitude;
    };
} sensors_event_t;

typedef struct {
    char name[12];
    int32_t version;
    int32_t sensor_id;
    int32_t type;
    float max_value;
    float min_value;
    float resolution;
    int32_t min_delay;
} sensor_t;

class BMP280 {
public:
    BMP280(int8_t cspin, SPIClass *theSPI = &SPI);
    ~BMP280();

    bool begin(uint8_t chipid = 0x58);
    float readTemperature();
    float readPressure();
    void reset();

private:
    SPIClass *_spi;
    int8_t _csPin;
    uint8_t _sensorID;
    uint32_t read24(byte reg);
    uint16_t read16_LE(byte reg);
    int16_t readS16_LE(byte reg);
    uint8_t read8(byte reg);
    void write8(byte reg, byte value);
    
    struct {
        uint16_t dig_T1;
        int16_t dig_T2;
        int16_t dig_T3;
        uint16_t dig_P1;
        int16_t dig_P2;
        int16_t dig_P3;
        int16_t dig_P4;
        int16_t dig_P5;
        int16_t dig_P6;
        int16_t dig_P7;
        int16_t dig_P8;
        int16_t dig_P9;
    } _bmp280_calib;

    int t_fine;
    void readCoefficients();
};

#endif
