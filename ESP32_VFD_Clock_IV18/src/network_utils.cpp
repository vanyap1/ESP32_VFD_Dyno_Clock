#include "network_utils.h"
#include "led_effects.h"

// Internal state
static bool apModeActive = false;
static DNSServer dnsServerAP;

// WiFi scan results storage
#define MAX_WIFI_NETWORKS 20
static WiFiScanResult storedNetworks[MAX_WIFI_NETWORKS];
static int storedNetworksCount = 0;
static bool scanInProgress = false;

// I2C scanner function for diagnostics
void scanI2C() {
  Serial.println("\n=== I2C Scanner ===");
  Serial.println("Scanning I2C bus...");
  
  byte count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print(" (");
      Serial.print(address);
      Serial.println(")");
      count++;
    }
  }
  
  if (count == 0) {
    Serial.println("No I2C devices found!");
  } else {
    Serial.print("Found ");
    Serial.print(count);
    Serial.println(" device(s)");
  }
  Serial.println("==================\n");
}

// Non-blocking AP mode startup
void startAPMode() {
  Serial.println("Starting non-blocking AP mode...");
  
  // Setup AP mode (keeps existing WiFi STA mode if connected)
  if (WiFi.status() == WL_CONNECTED) {
    // Already connected, add AP mode
    WiFi.mode(WIFI_AP_STA);
  } else {
    // Not connected, AP only
    WiFi.mode(WIFI_AP);
  }
  
  // Configure AP with better settings for stability
  WiFi.softAPConfig(
    IPAddress(192, 168, 4, 1),    // AP IP
    IPAddress(192, 168, 4, 1),    // Gateway
    IPAddress(255, 255, 255, 0)   // Subnet mask
  );
  
  #ifdef ESP32
    // Disable power saving for more stable connection
    WiFi.setSleep(false);
    
    // Set moderate WiFi power to avoid connection issues with mobile devices
    WiFi.setTxPower(WIFI_POWER_11dBm);  // Lower power for better stability
    
    // Configure WiFi to be less aggressive with connections
    esp_wifi_set_ps(WIFI_PS_NONE);  // Disable power saving completely
  #endif
  
  // Start AP with open security, channel 6 (most compatible), visible SSID, max 4 connections
  // Using channel 6 as it's less congested than 1 and 11
  WiFi.softAP(FIRST_SETUP_AP_NAME, NULL, 6, 0, 4);
  
  // Small delay to let AP stabilize
  delay(100);
  
  // Start DNS server for captive portal (redirects all domains to AP IP)
  dnsServerAP.start(53, "*", WiFi.softAPIP());
  
  apModeActive = true;
  
  Serial.print("AP SSID: ");
  Serial.println(FIRST_SETUP_AP_NAME);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("Web interface available at: http://" + WiFi.softAPIP().toString());
  
  // Scan and store available networks for quick access
  Serial.println("Pre-scanning WiFi networks for web interface...");
  delay(500);  // Wait for AP to fully start
  ledIndicateScanning();  // Show scanning animation
  scanAndStoreNetworks();
  
  Serial.print("AP mode started. Pre-scan found ");
  Serial.print(storedNetworksCount);
  Serial.println(" networks.");
}

bool isAPModeActive() {
  return apModeActive;
}

DNSServer& getDNSServer() {
  return dnsServerAP;
}

// Scan WiFi networks and store results
void scanAndStoreNetworks() {
  if (scanInProgress) {
    Serial.println("Scan already in progress, skipping...");
    return;
  }
  
  scanInProgress = true;
  Serial.println("Scanning WiFi networks...");
  
  WiFiMode_t currentMode = WiFi.getMode();
  if (currentMode == WIFI_AP) {
    WiFi.mode(WIFI_AP_STA);
    delay(100);  // Wait for mode switch
  }
  
  // Synchronous scan - LED animation runs in separate task so won't block
  int n = WiFi.scanNetworks(false, false);
  
  if (n < 0) {
    Serial.println("WiFi scan failed!");
    scanInProgress = false;
    return;  // Keep previous scan results if available
  }
  
  storedNetworksCount = 0;
  
  Serial.print("Networks found: ");
  Serial.println(n);
  
  // Store up to MAX_WIFI_NETWORKS
  for (int i = 0; i < n && i < MAX_WIFI_NETWORKS; i++) {
    storedNetworks[i].ssid = WiFi.SSID(i);
    storedNetworks[i].rssi = WiFi.RSSI(i);
    #ifdef ESP32
      storedNetworks[i].secure = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
    #else
      storedNetworks[i].secure = (WiFi.encryptionType(i) != AUTH_OPEN);
    #endif
    storedNetworksCount++;
    
    Serial.print("  ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(storedNetworks[i].ssid);
    Serial.print(" (");
    Serial.print(storedNetworks[i].rssi);
    Serial.println(" dBm)");
  }
  
  if (currentMode == WIFI_AP) {
    WiFi.mode(WIFI_AP);
  }
  
  scanInProgress = false;
  Serial.print("WiFi scan complete, stored ");
  Serial.print(storedNetworksCount);
  Serial.println(" networks");
}

// Get stored scan results count
int getStoredNetworksCount() {
  return storedNetworksCount;
}

// Get stored scan result by index
WiFiScanResult getStoredNetwork(int index) {
  if (index >= 0 && index < storedNetworksCount) {
    return storedNetworks[index];
  }
  WiFiScanResult empty = {"", 0, false};
  return empty;
}
