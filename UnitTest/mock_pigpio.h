#ifndef MOCK_PIGPIO_H
#define MOCK_PIGPIO_H

#include <gmock/gmock.h>

class MockPigpio {
public:
    MOCK_METHOD(int, i2cOpen, (int i2c_bus, int address, int flags), ());
    MOCK_METHOD(int, i2cClose, (int handle), ());
    MOCK_METHOD(int, i2cReadByteData, (int handle, uint8_t reg), ());
    MOCK_METHOD(int, i2cWriteByteData, (int handle, uint8_t reg, uint8_t value), ());
    MOCK_METHOD(int, gpioInitialise, (), ());
};

#endif // MOCK_PIGPIO_H
