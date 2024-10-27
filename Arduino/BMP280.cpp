#include "BMP280.h"

BMP280::BMP280(int8_t cspin, SPIClass *theSPI) {
    _csPin = cspin;
    _spi = theSPI;
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
}

BMP280::~BMP280() {}

bool BMP280::begin(uint8_t chipid) {
    _spi->begin();

    // Check chip ID
    _sensorID = read8(BMP280_REGISTER_CHIPID);
    if (_sensorID != chipid) return false;

    readCoefficients();
    return true;
}

void BMP280::readCoefficients() {
    _bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    _bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
    _bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

    _bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    _bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
    _bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
    _bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
    _bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
    _bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
    _bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
    _bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
    _bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
}

uint8_t BMP280::read8(byte reg) {
    digitalWrite(_csPin, LOW);
    _spi->transfer(reg);
    uint8_t value = _spi->transfer(0x00);
    digitalWrite(_csPin, HIGH);
    return value;
}

void BMP280::write8(byte reg, byte value) {
    digitalWrite(_csPin, LOW);
    _spi->transfer(reg);
    _spi->transfer(value);
    digitalWrite(_csPin, HIGH);
}

uint16_t BMP280::read16_LE(byte reg) {
    uint16_t value;
    digitalWrite(_csPin, LOW);
    _spi->transfer(reg);
    value = _spi->transfer(0x00);
    value |= (_spi->transfer(0x00) << 8);
    digitalWrite(_csPin, HIGH);
    return value;
}

int16_t BMP280::readS16_LE(byte reg) {
    return (int16_t)read16_LE(reg);
}

uint32_t BMP280::read24(byte reg) {
    uint32_t value;
    digitalWrite(_csPin, LOW);
    _spi->transfer(reg);
    value = _spi->transfer(0x00) << 16;
    value |= _spi->transfer(0x00) << 8;
    value |= _spi->transfer(0x00);
    digitalWrite(_csPin, HIGH);
    return value;
}

float BMP280::readTemperature() {
    int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
    adc_T >>= 4;

    int32_t var1, var2;
    var1 = (((((adc_T >> 3) - ((int32_t)_bmp280_calib.dig_T1 << 1))) * ((int32_t)_bmp280_calib.dig_T2)) >> 11);
    var2 = ((((((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) * ((int32_t)_bmp280_calib.dig_T3)) >> 14);
    t_fine = var1 + var2;

    
   return ((t_fine * 5 + 128) >> 8) / 100.0;
}

float BMP280::readPressure() {
    int64_t var1, var2, p;
    readTemperature(); // Update t_fine

    int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)_bmp280_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)_bmp280_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3) >> 8) + ((var1 * (int64_t)_bmp280_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_bmp280_calib.dig_P1) >> 33;

    if (var1 == 0) return 0; // Avoid division by zero

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)_bmp280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7) << 4);
    return (float)p / 256;
}

void BMP280::reset() {
    write8(BMP280_REGISTER_SOFTRESET, 0xB6);
}
