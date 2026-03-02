#include "RV8803.h"
#include <sys/time.h>
#include <time.h>

RV8803::RV8803() {
    _available = false;
}

bool RV8803::begin(TwoWire &wirePort) {
    _i2cPort = &wirePort;
    
    // Check if device is present
    _i2cPort->beginTransmission(RV8803_ADDR);
    if (_i2cPort->endTransmission() == 0) {
        _available = true;
        
        // Reset control register to defaults
        writeRegister(RV8803_CTRL, 0x00);
        
        return true;
    }
    
    _available = false;
    return false;
}

bool RV8803::isAvailable() {
    return _available;
}

uint8_t RV8803::readRegister(uint8_t reg) {
    _i2cPort->beginTransmission(RV8803_ADDR);
    _i2cPort->write(reg);
    _i2cPort->endTransmission();
    
    _i2cPort->requestFrom(RV8803_ADDR, 1);
    if (_i2cPort->available()) {
        return _i2cPort->read();
    }
    return 0;
}

bool RV8803::writeRegister(uint8_t reg, uint8_t value) {
    _i2cPort->beginTransmission(RV8803_ADDR);
    _i2cPort->write(reg);
    _i2cPort->write(value);
    return (_i2cPort->endTransmission() == 0);
}

uint8_t RV8803::bcdToDec(uint8_t val) {
    return ((val / 16 * 10) + (val % 16));
}

uint8_t RV8803::decToBcd(uint8_t val) {
    return ((val / 10 * 16) + (val % 10));
}

bool RV8803::setTime(uint8_t sec, uint8_t min, uint8_t hour, 
                     uint8_t date, uint8_t month, uint16_t year) {
    if (!_available) return false;
    
    // Convert to BCD and write to RTC
    writeRegister(RV8803_SEC, decToBcd(sec));
    writeRegister(RV8803_MIN, decToBcd(min));
    writeRegister(RV8803_HOUR, decToBcd(hour));
    writeRegister(RV8803_DATE, decToBcd(date));
    writeRegister(RV8803_MONTH, decToBcd(month));
    writeRegister(RV8803_YEAR, decToBcd(year - 2000));
    
    return true;
}

bool RV8803::getTime(uint8_t &sec, uint8_t &min, uint8_t &hour, 
                     uint8_t &date, uint8_t &month, uint16_t &year) {
    if (!_available) return false;
    
    // Read from RTC and convert from BCD
    sec = bcdToDec(readRegister(RV8803_SEC) & 0x7F);
    min = bcdToDec(readRegister(RV8803_MIN) & 0x7F);
    hour = bcdToDec(readRegister(RV8803_HOUR) & 0x3F);
    date = bcdToDec(readRegister(RV8803_DATE) & 0x3F);
    month = bcdToDec(readRegister(RV8803_MONTH) & 0x1F);
    year = bcdToDec(readRegister(RV8803_YEAR)) + 2000;
    
    return true;
}

bool RV8803::updateSystemTime() {
    if (!_available) return false;
    
    uint8_t sec, min, hour, date, month;
    uint16_t year;
    
    if (!getTime(sec, min, hour, date, month, year)) {
        return false;
    }
    
    // Set ESP32 system time
    struct tm timeinfo;
    timeinfo.tm_sec = sec;
    timeinfo.tm_min = min;
    timeinfo.tm_hour = hour;
    timeinfo.tm_mday = date;
    timeinfo.tm_mon = month - 1;  // tm_mon is 0-11
    timeinfo.tm_year = year - 1900;  // tm_year is years since 1900
    
    time_t t = mktime(&timeinfo);
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);
    
    return true;
}

bool RV8803::setFromSystemTime() {
    if (!_available) return false;
    
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    return setTime(timeinfo.tm_sec, timeinfo.tm_min, timeinfo.tm_hour,
                   timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
}
