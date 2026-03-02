#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <Arduino.h>
#include "FastLED.h"
#include "dto.h"

// System states for LED indication
enum SystemState {
  STATE_NORMAL,           // Normal operation - use user selected effect
  STATE_WIFI_CONNECTING,  // Connecting to WiFi - chase animation
  STATE_WIFI_CONNECTED,   // WiFi connected - green flash
  STATE_WIFI_FAILED,      // WiFi connection failed - red pulse
  STATE_AP_MODE,          // AP mode active - blue pulse
  STATE_SCANNING          // Scanning networks - radar sweep
};

// Initialize LED effects system
void initLEDEffects(CRGB* ledsArray, uint8_t numLeds);

// Start LED animation in separate FreeRTOS task
void startLEDAnimationTask();

// Stop LED animation task
void stopLEDAnimationTask();

// Update LED effects (call regularly from loop or use task)
void updateLEDEffect();

// Set system state for status indication
void setSystemState(SystemState state);

// Get current system state
SystemState getSystemState();

// LED state indication functions
void ledIndicateWiFiConnecting();
void ledIndicateWiFiConnected();
void ledIndicateWiFiFailed();
void ledIndicateAPMode();
void ledIndicateScanning();

#endif // LED_EFFECTS_H
