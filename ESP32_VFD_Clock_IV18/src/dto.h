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
}SystemSetup;



extern SystemSetup sysSetupStruc;

#endif