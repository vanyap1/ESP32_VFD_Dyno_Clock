#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <Wire.h>

#define FIRST_SETUP_AP_NAME "VFD_Setup"
// I2C scanner for diagnostics
void scanI2C();

// Start Access Point mode
void startAPMode();

// Get AP mode active status
bool isAPModeActive();

// Get DNS server reference
DNSServer& getDNSServer();

#endif // NETWORK_UTILS_H
