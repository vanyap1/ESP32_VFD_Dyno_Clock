#ifndef HDC2010_H
#define HDC2010_H

#include <Arduino.h>
#include <Wire.h>

#define HDC2010_ADDR 0x40  // ADDR pin to GND

// HDC2010 Register addresses
#define HDC2010_TEMP_LOW       0x00
#define HDC2010_TEMP_HIGH      0x01
#define HDC2010_HUMID_LOW      0x02
#define HDC2010_HUMID_HIGH     0x03
#define HDC2010_INTERRUPT      0x04
#define HDC2010_TEMP_MAX       0x05
#define HDC2010_HUMID_MAX      0x06
#define HDC2010_INTERRUPT_EN   0x07
#define HDC2010_TEMP_OFFSET    0x08
#define HDC2010_HUMID_OFFSET   0x09
#define HDC2010_TEMP_THR_L     0x0A
#define HDC2010_TEMP_THR_H     0x0B
#define HDC2010_HUMID_THR_L    0x0C
#define HDC2010_HUMID_THR_H    0x0D
#define HDC2010_CONFIG         0x0E
#define HDC2010_MEASUREMENT    0x0F
#define HDC2010_MID_L          0xFC
#define HDC2010_MID_H          0xFD
#define HDC2010_DEV_ID_L       0xFE
#define HDC2010_DEV_ID_H       0xFF

class HDC2010 {
public:
    HDC2010();
    
    // Initialize the sensor
    bool begin(TwoWire &wirePort = Wire);
    
    // Check if sensor is available on I2C bus
    bool isAvailable();
    
    // Trigger measurement
    bool triggerMeasurement();
    
    // Read temperature in Celsius
    float readTemperature();
    
    // Read humidity in %RH
    float readHumidity();
    
    // Read both temperature and humidity
    bool read(float &temperature, float &humidity);
    
    // Reset sensor
    bool reset();
    
private:
    TwoWire *_i2cPort;
    bool _available;
    
    uint8_t readRegister(uint8_t reg);
    bool writeRegister(uint8_t reg, uint8_t value);
    uint16_t readRegister16(uint8_t reg);
};

#endif
