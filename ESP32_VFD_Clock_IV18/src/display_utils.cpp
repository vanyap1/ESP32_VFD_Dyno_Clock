#include "display_utils.h"
#include <math.h>

static String formatSignedTemperature(float value) {
  if (isnan(value)) {
    return "--";
  }

  char buffer[8];
  snprintf(buffer, sizeof(buffer), "%02d", (int)roundf(value));
  return String(buffer);
}

static String formatIntegerPlaceholder(int value, const char* placeholder, const char* pattern) {
  if (value < 0) {
    return String(placeholder);
  }

  char buffer[8];
  snprintf(buffer, sizeof(buffer), pattern, value);
  return String(buffer);
}

static String formatCompactCurrency(float value) {
  if (isnan(value) || value <= 0.0f) {
    return "--.-";
  }

  char buffer[16];
  float absoluteValue = fabsf(value);

  if (absoluteValue >= 1000000.0f) {
    snprintf(buffer, sizeof(buffer), "%.1fM", value / 1000000.0f);
  } else if (absoluteValue >= 1000.0f) {
    snprintf(buffer, sizeof(buffer), "%.1fK", value / 1000.0f);
  } else if (absoluteValue >= 100.0f) {
    snprintf(buffer, sizeof(buffer), "%.0f", value);
  } else if (absoluteValue >= 10.0f) {
    snprintf(buffer, sizeof(buffer), "%.1f", value);
  } else {
    snprintf(buffer, sizeof(buffer), "%.2f", value);
  }

  return String(buffer);
}

static String formatWholeNumber(float value) {
  if (isnan(value) || value <= 0.0f) {
    return "--";
  }

  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%.0f", value);
  return String(buffer);
}

static String formatBtcShort(float value) {
  if (isnan(value) || value <= 0.0f) {
    return "--";
  }

  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%.3f", value / 1000000.0f);
  return String(buffer);
}

// URL decode function
String urlDecode(String str) {
  String decoded = "";
  char temp[] = "0x00";
  unsigned int len = str.length();
  unsigned int i = 0;
  while (i < len) {
    char decodedChar;
    char encodedChar = str.charAt(i++);
    if ((encodedChar == '%') && (i + 1 < len)) {
      temp[2] = str.charAt(i++);
      temp[3] = str.charAt(i++);
      decodedChar = strtol(temp, NULL, 16);
    } else if (encodedChar == '+') {
      decodedChar = ' ';
    } else {
      decodedChar = encodedChar;
    }
    decoded += decodedChar;
  }
  return decoded;
}

// Parse display format and replace variables
String parseDisplayFormat(String format, struct tm &timeinfo, 
                         float sensorTemp, int sensorPress, int sensorHum,
                         float weatherTemp, const char* weatherCond,
                         float currencyEUR, float currencyUSD, float currencyBTC) {
  String result = format;
  char buffer[16];
  
  // Time
  sprintf(buffer, "%02d", timeinfo.tm_hour);
  result.replace("*HH*", buffer);
  
  sprintf(buffer, "%02d", timeinfo.tm_min);
  result.replace("*MM*", buffer);
  
  sprintf(buffer, "%02d", timeinfo.tm_sec);
  result.replace("*SS*", buffer);
  
  sprintf(buffer, "%02d", timeinfo.tm_mday);
  result.replace("*DD*", buffer);
  
  sprintf(buffer, "%02d", timeinfo.tm_mon + 1);
  result.replace("*MO*", buffer);
  
  sprintf(buffer, "%02d", (timeinfo.tm_year + 1900) % 100);
  result.replace("*YY*", buffer);
  
  sprintf(buffer, "%04d", timeinfo.tm_year + 1900);
  result.replace("*YYYY*", buffer);
  
  result.replace("*TEMP*", formatSignedTemperature(sensorTemp));
  result.replace("*PRESS*", formatIntegerPlaceholder(sensorPress, "---", "%03d"));
  result.replace("*HUM*", formatIntegerPlaceholder(sensorHum, "--", "%02d"));
  result.replace("*WTEMP*", formatSignedTemperature(weatherTemp));
  result.replace("*WCOND*", (weatherCond != nullptr && weatherCond[0] != '\0') ? weatherCond : "--");
  result.replace("*EUR*", formatCompactCurrency(currencyEUR));
  result.replace("*USD*", formatCompactCurrency(currencyUSD));
  result.replace("*BTCf*", formatWholeNumber(currencyBTC));
  result.replace("*BTCs*", formatBtcShort(currencyBTC));
  result.replace("*BTC*", formatWholeNumber(currencyBTC));
  
  return result;
}
