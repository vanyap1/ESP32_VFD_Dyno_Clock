#include "HDC2010.h"

HDC2010::HDC2010() {
    _available = false;
}

bool HDC2010::begin(TwoWire &wirePort) {
    _i2cPort = &wirePort;
    
    // Check if device is present
    _i2cPort->beginTransmission(HDC2010_ADDR);
    if (_i2cPort->endTransmission() == 0) {
        // Verify device ID
        uint16_t deviceID = readRegister16(HDC2010_DEV_ID_L);
        uint16_t mfgID = readRegister16(HDC2010_MID_L);
        
        Serial.print("HDC2010 Device ID: 0x");
        Serial.println(deviceID, HEX);
        Serial.print("HDC2010 Manufacturer ID: 0x");
        Serial.println(mfgID, HEX);
        
        // HDC2010 has device ID 0x07D0, HDC2080 has 0x07D1
        if ((deviceID & 0xFFFC) == 0x07D0) {  // HDC2010/HDC2080 device ID
            _available = true;
            
            // Configure sensor: 14-bit resolution for both temp and humidity
            writeRegister(HDC2010_CONFIG, 0x00);
            
            // Configure measurement: Temperature and Humidity
            writeRegister(HDC2010_MEASUREMENT, 0x01);
            
            delay(10);
            return true;
        } else {
            Serial.print("Unexpected device ID, expected 0x07D0, got 0x");
            Serial.println(deviceID, HEX);
        }
    }
    
    _available = false;
    return false;
}

bool HDC2010::isAvailable() {
    return _available;
}

uint8_t HDC2010::readRegister(uint8_t reg) {
    _i2cPort->beginTransmission(HDC2010_ADDR);
    _i2cPort->write(reg);
    _i2cPort->endTransmission();
    
    _i2cPort->requestFrom(HDC2010_ADDR, 1);
    if (_i2cPort->available()) {
        return _i2cPort->read();
    }
    return 0;
}

uint16_t HDC2010::readRegister16(uint8_t reg) {
    _i2cPort->beginTransmission(HDC2010_ADDR);
    _i2cPort->write(reg);
    _i2cPort->endTransmission();
    
    _i2cPort->requestFrom(HDC2010_ADDR, 2);
    if (_i2cPort->available() >= 2) {
        uint8_t low = _i2cPort->read();
        uint8_t high = _i2cPort->read();
        return (high << 8) | low;
    }
    return 0;
}

bool HDC2010::writeRegister(uint8_t reg, uint8_t value) {
    _i2cPort->beginTransmission(HDC2010_ADDR);
    _i2cPort->write(reg);
    _i2cPort->write(value);
    return (_i2cPort->endTransmission() == 0);
}

bool HDC2010::triggerMeasurement() {
    if (!_available) return false;
    
    // Trigger measurement
    return writeRegister(HDC2010_MEASUREMENT, 0x01);
}

float HDC2010::readTemperature() {
    if (!_available) return 0.0;
    
    uint16_t rawTemp = readRegister16(HDC2010_TEMP_LOW);
    
    // Convert to Celsius: Temperature = (rawTemp / 2^16) * 165 - 40
    return (rawTemp / 65536.0) * 165.0 - 40.0;
}

float HDC2010::readHumidity() {
    if (!_available) return 0.0;
    
    uint16_t rawHumidity = readRegister16(HDC2010_HUMID_LOW);
    
    // Convert to %RH: Humidity = (rawHumidity / 2^16) * 100
    return (rawHumidity / 65536.0) * 100.0;
}

bool HDC2010::read(float &temperature, float &humidity) {
    if (!_available) return false;
    
    // Trigger measurement
    triggerMeasurement();
    
    // Wait for conversion (typical 3.65ms for 14-bit)
    delay(5);
    
    // Read values
    temperature = readTemperature();
    humidity = readHumidity();
    
    return true;
}

bool HDC2010::reset() {
    if (!_available) return false;
    
    // Software reset
    writeRegister(HDC2010_CONFIG, 0x80);
    delay(10);
    
    // Reconfigure after reset
    writeRegister(HDC2010_CONFIG, 0x00);
    writeRegister(HDC2010_MEASUREMENT, 0x01);
    
    return true;
}
