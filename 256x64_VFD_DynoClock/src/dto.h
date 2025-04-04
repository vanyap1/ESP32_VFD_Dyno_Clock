#ifndef DTO_H
#define DTO_H

extern const char* ntpServers[];

typedef struct{
  uint8_t FirstStart;
  bool NodeWasUpdated; 
  char ssid[20];
  char pass[20];
  uint8_t ambLightBrightness;
  uint8_t ambLightEnable;
  uint8_t ambLightColr[3];
  uint8_t ntpEN;
  uint8_t ntpServerIndex;
  uint8_t ntpTimeZone;
  uint8_t screenType;
  uint8_t screenDemoMode;
}SystemSetup;



extern SystemSetup sysSetupStruc;

#endif