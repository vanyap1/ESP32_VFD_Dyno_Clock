#ifndef RV8803_H
#define RV8803_H

#include <Arduino.h>
#include <Wire.h>

#define RV8803_ADDR 0x32

// RV-8803 Register addresses
#define RV8803_SEC      0x00
#define RV8803_MIN      0x01
#define RV8803_HOUR     0x02
#define RV8803_WEEKDAY  0x03
#define RV8803_DATE     0x04
#define RV8803_MONTH    0x05
#define RV8803_YEAR     0x06
#define RV8803_RAM      0x07
#define RV8803_CTRL     0x0F
#define RV8803_FLAG     0x10

class RV8803 {
public:
    RV8803();
    
    // Initialize the RTC
    bool begin(TwoWire &wirePort = Wire);
    
    // Check if RTC is available on I2C bus
    bool isAvailable();
    
    // Set time
    bool setTime(uint8_t sec, uint8_t min, uint8_t hour, 
                 uint8_t date, uint8_t month, uint16_t year);
    
    // Get time
    bool getTime(uint8_t &sec, uint8_t &min, uint8_t &hour, 
                 uint8_t &date, uint8_t &month, uint16_t &year);
    
    // Update system time from RTC
    bool updateSystemTime();
    
    // Set system time to RTC
    bool setFromSystemTime();
    
private:
    TwoWire *_i2cPort;
    bool _available;
    
    uint8_t readRegister(uint8_t reg);
    bool writeRegister(uint8_t reg, uint8_t value);
    
    uint8_t bcdToDec(uint8_t val);
    uint8_t decToBcd(uint8_t val);
};

#endif
