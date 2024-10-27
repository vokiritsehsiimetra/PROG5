#include <iostream>
#include <bmp280.h>
#include <mqtt/async_client.h>  // Paho MQTT header

#define I2C_BUS     1
#define I2C_ID     0x76
#define TPC       "sensor/bmp280"
#define QOS         1
#define ADDR     "tcp://localhost:1883"
#define CID    "RaspberryPiZero"
#define TIMEOUT     10000L

// Initialize BMP280 object with I2C bus 1 and I2C ADDR 0x76
BMP280 bmp280(I2C_BUS, I2C_ID);

// MQTT configuration
mqtt::async_client client(ADDR, CID);
mqtt::connect_options connOpts;

void publish_data(float tmprtr, float pressure) {
    std::string payload = "{\"tmprtr\": " + std::to_string(tmprtr) + ", \"pressure\": " + std::to_string(pressure) + "}";
    mqtt::message_ptr pubmsg = mqtt::make_message(TPC, payload);
    pubmsg->set_qos(QOS);
    client.publish(pubmsg);
}

int main() {
    // Initialize BMP280 sensor
    if (!bmp280.begin()) {
        std::cerr << "Failed to initialize BMP280" << std::endl;
        return 1;
    }

    // Connect to MQTT broker
    try {
        client.connect(connOpts)->wait();
        std::cout << "Connected to MQTT broker" << std::endl;
    } catch (const mqtt::exception& exc) {
        std::cerr << "MQTT connection failed: " << exc.what() << std::endl;
        return 1;
    }

    // Main loop to read sensor data and publish via MQTT
    while (true) {
        float tmprtr = bmp280.readtmprtr();
        float pressure = bmp280.readPressure();

        std::cout << "tmprtr: " << tmprtr << " °C, Pressure: " << pressure << " hPa" << std::endl;

        // Publish data to MQTT
        publish_data(tmprtr, pressure);

        // Sleep for 5 seconds before reading again
        sleep(5);
    }

    // Disconnect from MQTT broker
    client.disconnect()->wait();
    return 0;
}
