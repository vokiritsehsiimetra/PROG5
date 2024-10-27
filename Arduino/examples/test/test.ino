#include <SPI.h>
#include <BMP280.h> // Include the modified BMP280 library

// Define the chip select pin for the BMP280
#define BMP280_CS_PIN 10

// Create an instance of the BMP280 sensor using SPI
BMP280 bmp(BMP280_CS_PIN, &SPI);

void setup() {
  Serial.begin(9600); // Initialize serial communication

  // Start the BMP280 sensor
  if (!bmp.begin(BMP280_CS_PIN)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1); // Halt the program
  }

  Serial.println("BMP280 sensor initialized successfully!");
}

void loop() {
  // Read temperature and pressure
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();

  // Print temperature and pressure to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  delay(2000); // Wait for 2 seconds before the next reading
}
