#include "led_effects.h"

// External references
extern SystemSetup sysSetupStruc;

// Internal state
static CRGB* leds = nullptr;
static uint8_t ledCount = 0;
static uint32_t ledUpdateTimer = 0;
static uint8_t ledAnimationPhase = 0;
static SystemState systemState = STATE_NORMAL;

void initLEDEffects(CRGB* ledsArray, uint8_t numLeds) {
  leds = ledsArray;
  ledCount = numLeds;
  ledUpdateTimer = 0;
  ledAnimationPhase = 0;
  systemState = STATE_NORMAL;
}

void setSystemState(SystemState state) {
  systemState = state;
}

SystemState getSystemState() {
  return systemState;
}

void ledIndicateWiFiConnecting() {
  // Chase effect for WiFi connecting
  int position = (ledAnimationPhase / 8) % 4;
  for (int i = 0; i < 4; i++) {
    if (i == position) {
      leds[i] = CRGB(0, 100, 255); // Blue leading LED
    } else if (i == (position - 1 + 4) % 4) {
      leds[i] = CRGB(0, 30, 80); // Dimmer trail
    } else {
      leds[i] = CRGB(0, 0, 0);
    }
  }
}

void ledIndicateWiFiFailed() {
  // Red pulsing for WiFi error
  uint8_t brightness = beatsin8(30, 50, 255);
  for (int i = 0; i < 4; i++) {
    leds[i] = CRGB(brightness, 0, 0);
  }
}

void ledIndicateAPMode() {
  // Slow blue pulse for AP mode
  uint8_t brightness = beatsin8(20, 30, 200);
  for (int i = 0; i < 4; i++) {
    leds[i] = CRGB(0, brightness / 3, brightness);
  }
}

void ledIndicateScanning() {
  // Fast orange blink for scanning
  uint8_t blink = (ledAnimationPhase / 10) % 2;
  for (int i = 0; i < 4; i++) {
    if (blink) {
      leds[i] = CRGB(255, 100, 0);
    } else {
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
  
  // Check if we're in a special system state
  if (systemState != STATE_NORMAL) {
    FastLED.setBrightness(255); // Full brightness for status indication
    
    switch(systemState) {
      case STATE_WIFI_CONNECTING:
        ledIndicateWiFiConnecting();
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
