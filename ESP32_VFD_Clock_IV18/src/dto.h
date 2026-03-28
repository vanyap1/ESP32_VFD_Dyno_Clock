#ifndef DTO_H
#define DTO_H

#include <stdint.h>
#include <stdbool.h>

extern const char* ntpServers[];

typedef struct{
  uint8_t FirstStart;
  char ssid[32];
  char pass[64];


  
  uint8_t ambLightBrightness;
  uint8_t ambLightEnable;
  uint8_t ambLightColr[3];
  uint8_t ntpEN;
  uint8_t ntpServerIndex;
  uint8_t ntpTimeZone;
  uint8_t screenType;
  uint8_t screenDemoMode;
  uint32_t customCharData[96];
  uint8_t segmentsBitMask[96];
  
  // Display formats (3 screens as arrays)
  char displayFormat[3][32];
  uint8_t displayFormatTime[3];
  bool displayFormatEnable[3];
  bool displayFormatBlink[3];
  
  // Blink settings
  uint32_t blinkMask;
  uint8_t blinkPosition;
  
  
  // Sensors
  bool sensorPressure;
  bool sensorTemperature;
  bool sensorAutoBrightness;
  bool sensorWeatherApi;
  bool sensorCurrency;
  char myCurrency[4];  // Currency code (e.g., "UAH", "USD", "EUR")
  
  // Display
  uint8_t displayBrightness;
  
  // LED
  uint8_t ledCount;
  uint8_t ledEffect;
  uint8_t screenDriver;  // Screen driver mode (0-15: PT6315/PT6311/PT6324 display configuration)
}SystemSetup;



extern SystemSetup sysSetupStruc;

#endif