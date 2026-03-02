#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <Wire.h>

#ifdef ESP32
  #include <esp_wifi.h>
#endif

#define FIRST_SETUP_AP_NAME "VFD_Setup"

// WiFi scan result structure
struct WiFiScanResult {
  String ssid;
  int rssi;
  bool secure;
};

// I2C scanner for diagnostics
void scanI2C();

// Start Access Point mode
void startAPMode();

// Get AP mode active status
bool isAPModeActive();

// Get DNS server reference
DNSServer& getDNSServer();

// Scan WiFi networks and store results
void scanAndStoreNetworks();

// Get stored scan results count
int getStoredNetworksCount();

// Get stored scan result by index
WiFiScanResult getStoredNetwork(int index);

#endif // NETWORK_UTILS_H
