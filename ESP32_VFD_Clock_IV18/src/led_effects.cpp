#include "led_effects.h"

// External references
extern SystemSetup sysSetupStruc;

// Internal state
static CRGB* leds = nullptr;
static uint8_t ledCount = 0;
static uint32_t ledUpdateTimer = 0;
static uint8_t ledAnimationPhase = 0;
static SystemState systemState = STATE_NORMAL;

// FreeRTOS task handle and mutex
static TaskHandle_t ledTaskHandle = nullptr;
static SemaphoreHandle_t stateMutex = nullptr;

void initLEDEffects(CRGB* ledsArray, uint8_t numLeds) {
  leds = ledsArray;
  ledCount = numLeds;
  ledUpdateTimer = 0;
  ledAnimationPhase = 0;
  systemState = STATE_NORMAL;
  
  // Create mutex for thread-safe state access
  if (stateMutex == nullptr) {
    stateMutex = xSemaphoreCreateMutex();
  }
}

void setSystemState(SystemState state) {
  if (stateMutex != nullptr) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    systemState = state;
    xSemaphoreGive(stateMutex);
  } else {
    systemState = state;
  }
}

SystemState getSystemState() {
  SystemState state;
  if (stateMutex != nullptr) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    state = systemState;
    xSemaphoreGive(stateMutex);
  } else {
    state = systemState;
  }
  return state;
}

// FreeRTOS task for LED animation
void ledAnimationTask(void* parameter) {
  const TickType_t xDelay = pdMS_TO_TICKS(20); // 50Hz update rate
  
  while (true) {
    updateLEDEffect();
    vTaskDelay(xDelay);
  }
}

// Start LED animation task
void startLEDAnimationTask() {
  if (ledTaskHandle == nullptr) {
    xTaskCreatePinnedToCore(
      ledAnimationTask,     // Task function
      "LEDAnimation",       // Task name
      2048,                 // Stack size (bytes)
      nullptr,              // Task parameter
      1,                    // Priority (1 = low)
      &ledTaskHandle,       // Task handle
      0                     // Core 0 (Core 1 is used by WiFi)
    );
    Serial.println("LED animation task started");
  }
}

// Stop LED animation task
void stopLEDAnimationTask() {
  if (ledTaskHandle != nullptr) {
    vTaskDelete(ledTaskHandle);
    ledTaskHandle = nullptr;
    Serial.println("LED animation task stopped");
  }
}

void ledIndicateWiFiConnecting() {
  // Chase effect with color transition for WiFi connecting
  int position = (ledAnimationPhase / 6) % 4;
  for (int i = 0; i < 4; i++) {
    if (i == position) {
      // Bright cyan leading LED
      leds[i] = CRGB(0, 150, 255);
    } else if (i == (position - 1 + 4) % 4) {
      // Blue trail
      leds[i] = CRGB(0, 50, 150);
    } else if (i == (position - 2 + 4) % 4) {
      // Dim purple trail
      leds[i] = CRGB(20, 0, 80);
    } else {
      leds[i] = CRGB(0, 0, 0);
    }
  }
}

void ledIndicateWiFiConnected() {
  // Quick green flash to indicate successful connection
  // Flash twice quickly then fade out
  int cycle = ledAnimationPhase % 60;
  uint8_t brightness = 0;
  
  if (cycle < 8) {
    // First flash
    brightness = 255;
  } else if (cycle < 16) {
    // First fade
    brightness = 255 - ((cycle - 8) * 32);
  } else if (cycle < 24) {
    // Second flash
    brightness = 255;
  } else if (cycle < 40) {
    // Second fade
    brightness = 255 - ((cycle - 24) * 16);
  }
  
  for (int i = 0; i < 4; i++) {
    leds[i] = CRGB(0, brightness, 0);
  }
}

void ledIndicateWiFiFailed() {
  // Red pulsing for WiFi error
  uint8_t brightness = beatsin8(30, 50, 255);
  for (int i = 0; i < 4; i++) {
    leds[i] = CRGB(brightness, 0, 0);
  }
  delay(500); // Short delay to allow pulse to be visible
}

void ledIndicateAPMode() {
  // Gentle breathing blue pulse for AP mode
  uint8_t brightness = beatsin8(25, 20, 180);
  // Add slight color shift
  uint8_t blueVariation = beatsin8(15, 180, 255);
  for (int i = 0; i < 4; i++) {
    // Offset each LED slightly for wave effect
    uint8_t offset = i * 15;
    uint8_t ledBrightness = beatsin8(25, 20, 180, 0, offset);
    leds[i] = CRGB(0, ledBrightness / 4, ledBrightness);
  }
}

void ledIndicateScanning() {
  // Radar scanning effect - LEDs light up sequentially then fade
  int position = (ledAnimationPhase / 4) % 8; // 0-7 for forward and back sweep
  
  for (int i = 0; i < 4; i++) {
    int ledPos = i;
    if (position >= 4) {
      // Reverse sweep
      ledPos = 3 - i;
      position = 7 - position;
    }
    
    if (ledPos == position) {
      // Active scanning LED - bright orange
      leds[i] = CRGB(255, 120, 0);
    } else if (ledPos == position - 1) {
      // Trail - dimmer orange
      leds[i] = CRGB(150, 60, 0);
    } else if (ledPos < position) {
      // Already scanned - very dim green
      leds[i] = CRGB(0, 40, 0);
    } else {
      // Not yet scanned - off
      leds[i] = CRGB(0, 0, 0);
    }
  }
}

void updateLEDEffect() {
  // Update LEDs at ~50Hz for smooth animations
  if (millis() - ledUpdateTimer < 20) {
    return;
  }
  ledUpdateTimer = millis();
  
  // Increment animation phase (0-255)
  ledAnimationPhase++;
  
  // Get current system state (thread-safe)
  SystemState currentState = getSystemState();
  
  // Check if we're in a special system state
  if (currentState != STATE_NORMAL) {
    FastLED.setBrightness(255); // Full brightness for status indication
    
    switch(currentState) {
      case STATE_WIFI_CONNECTING:
        ledIndicateWiFiConnecting();
        break;
      case STATE_WIFI_CONNECTED:
        ledIndicateWiFiConnected();
        break;
      case STATE_WIFI_FAILED:
        ledIndicateWiFiFailed();
        break;
      case STATE_AP_MODE:
        ledIndicateAPMode();
        break;
      case STATE_SCANNING:
        ledIndicateScanning();
        break;
      default:
        break;
    }
    
    FastLED.show();
    return;
  }
  
  FastLED.setBrightness(sysSetupStruc.ambLightBrightness);
  
  switch(sysSetupStruc.ledEffect) {
    case 0: // Solid Color
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        leds[i] = CRGB(sysSetupStruc.ambLightColr[0], 
                       sysSetupStruc.ambLightColr[1], 
                       sysSetupStruc.ambLightColr[2]);
      }
      break;
      
    case 1: // Rainbow
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        leds[i] = CHSV((ledAnimationPhase + i * (256 / sysSetupStruc.ledCount)) % 256, 255, 255);
      }
      break;
      
    case 2: // Breathing
      {
        uint8_t brightness = (exp(sin(ledAnimationPhase / 20.0 * PI)) - 0.36787944) * 108.0;
        for (int i = 0; i < sysSetupStruc.ledCount; i++) {
          leds[i] = CRGB(
            (sysSetupStruc.ambLightColr[0] * brightness) / 255,
            (sysSetupStruc.ambLightColr[1] * brightness) / 255,
            (sysSetupStruc.ambLightColr[2] * brightness) / 255
          );
        }
      }
      break;
      
    case 3: // Pulse
      {
        uint8_t pulse = beatsin8(60, 0, 255);
        for (int i = 0; i < sysSetupStruc.ledCount; i++) {
          leds[i] = CRGB(
            (sysSetupStruc.ambLightColr[0] * pulse) / 255,
            (sysSetupStruc.ambLightColr[1] * pulse) / 255,
            (sysSetupStruc.ambLightColr[2] * pulse) / 255
          );
        }
      }
      break;
      
    case 4: // Wave
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        uint8_t brightness = sin8((ledAnimationPhase * 2) + (i * 32));
        leds[i] = CRGB(
          (sysSetupStruc.ambLightColr[0] * brightness) / 255,
          (sysSetupStruc.ambLightColr[1] * brightness) / 255,
          (sysSetupStruc.ambLightColr[2] * brightness) / 255
        );
      }
      break;
      
    case 5: // Chase
      {
        int position = (ledAnimationPhase / 8) % sysSetupStruc.ledCount;
        for (int i = 0; i < sysSetupStruc.ledCount; i++) {
          if (i == position) {
            leds[i] = CRGB(sysSetupStruc.ambLightColr[0], 
                           sysSetupStruc.ambLightColr[1], 
                           sysSetupStruc.ambLightColr[2]);
          } else if (i == (position - 1 + sysSetupStruc.ledCount) % sysSetupStruc.ledCount) {
            leds[i] = CRGB(sysSetupStruc.ambLightColr[0] / 4, 
                           sysSetupStruc.ambLightColr[1] / 4, 
                           sysSetupStruc.ambLightColr[2] / 4);
          } else {
            leds[i] = CRGB(0, 0, 0);
          }
        }
      }
      break;
      
    case 6: // Twinkle
      {
        static uint32_t lastTwinkle = 0;
        if (millis() - lastTwinkle > 100) {
          lastTwinkle = millis();
          int led = random(sysSetupStruc.ledCount);
          if (random(100) < 30) { // 30% chance to twinkle
            leds[led] = CRGB(sysSetupStruc.ambLightColr[0], 
                            sysSetupStruc.ambLightColr[1], 
                            sysSetupStruc.ambLightColr[2]);
          } else {
            leds[led].fadeToBlackBy(64);
          }
          // Fade all LEDs slightly
          for (int i = 0; i < sysSetupStruc.ledCount; i++) {
            leds[i].fadeToBlackBy(16);
          }
        }
      }
      break;
      
    default:
      // Fallback to solid color
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        leds[i] = CRGB(sysSetupStruc.ambLightColr[0], 
                       sysSetupStruc.ambLightColr[1], 
                       sysSetupStruc.ambLightColr[2]);
      }
      break;
  }
  
  FastLED.show();
}
