#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <Arduino.h>
#include <time.h>

// URL decode function
String urlDecode(String str);

// Parse display format and replace variables
String parseDisplayFormat(String format, struct tm &timeinfo, 
                         float sensorTemp, int sensorPress, int sensorHum,
                         float weatherTemp, const char* weatherCond,
                         float currencyEUR, float currencyUSD, float currencyBTC);

#endif // DISPLAY_UTILS_H
