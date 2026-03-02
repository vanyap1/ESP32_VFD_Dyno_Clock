#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <Arduino.h>
#include "FastLED.h"
#include "dto.h"

// System states for LED indication
enum SystemState {
  STATE_NORMAL,           // Normal operation - use user selected effect
  STATE_WIFI_CONNECTING,  // Connecting to WiFi - chase animation
  STATE_WIFI_FAILED,      // WiFi connection failed - red pulse
  STATE_AP_MODE,          // AP mode active - blue pulse
  STATE_SCANNING          // Scanning networks - orange blink
};

// Initialize LED effects system
void initLEDEffects(CRGB* ledsArray, uint8_t numLeds);

// Update LED effects (call regularly from loop)
void updateLEDEffect();

// Set system state for status indication
void setSystemState(SystemState state);

// Get current system state
SystemState getSystemState();

// LED state indication functions
void ledIndicateWiFiConnecting();
void ledIndicateWiFiFailed();
void ledIndicateAPMode();
void ledIndicateScanning();

#endif // LED_EFFECTS_H
