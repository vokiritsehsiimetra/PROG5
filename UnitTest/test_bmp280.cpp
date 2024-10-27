#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "bmp280.h"
#include "mock_pigpio.h"

using ::testing::Return;
using ::testing::AtLeast;

class BMP280Test : public ::testing::Test {
protected:
    MockPigpio mockI2C;
    BMP280* bmp280;

    void SetUp() override {
        bmp280 = new BMP280(&mockI2C, 1, 0x76);
    }

    void TearDown() override {
        delete bmp280;
    }
};

TEST_F(BMP280Test, InitializeSuccess) {
    EXPECT_CALL(mockI2C, gpioInitialise())
        .WillOnce(Return(0)); // Successful initialization
    EXPECT_CALL(mockI2C, i2cOpen(1, 0x76, 0))
        .WillOnce(Return(1)); // Simulate successful I2C open

    EXPECT_TRUE(bmp280->begin());
}

TEST_F(BMP280Test, InitializeFailure) {
    EXPECT_CALL(mockI2C, gpioInitialise())
        .WillOnce(Return(-1)); // Failure in GPIO initialization

    EXPECT_FALSE(bmp280->begin());
}

TEST_F(BMP280Test, I2COpenFailure) {
    EXPECT_CALL(mockI2C, gpioInitialise())
        .WillOnce(Return(0)); // Successful initialization
    EXPECT_CALL(mockI2C, i2cOpen(1, 0x76, 0))
        .WillOnce(Return(-1)); // Simulate I2C open failure

    EXPECT_FALSE(bmp280->begin());
}

// Add more tests for reading temperature and pressure...

