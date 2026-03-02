#include "network_utils.h"
#include "setup.h"

// Internal state
static bool apModeActive = false;
static DNSServer dnsServerAP;

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
  
  WiFi.softAP(FIRST_SETUP_AP_NAME, NULL, 3);
  
  // Start DNS server for captive portal
  dnsServerAP.start(53, "*", WiFi.softAPIP());
  
  apModeActive = true;
  
  Serial.print("AP SSID: ");
  Serial.println(FIRST_SETUP_AP_NAME);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("Web interface available at: http://" + WiFi.softAPIP().toString());
}

bool isAPModeActive() {
  return apModeActive;
}

DNSServer& getDNSServer() {
  return dnsServerAP;
}
