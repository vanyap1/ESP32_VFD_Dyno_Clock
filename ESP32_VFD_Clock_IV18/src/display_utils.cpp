#include "display_utils.h"

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
  
  sprintf(buffer, "%02d", (int)sensorTemp);
  result.replace("*TEMP*", buffer);
  
  sprintf(buffer, "%03d", sensorPress);
  result.replace("*PRESS*", buffer);
  
  sprintf(buffer, "%02d", sensorHum);
  result.replace("*HUM*", buffer);
  
  sprintf(buffer, "%02d", (int)weatherTemp);
  result.replace("*WTEMP*", buffer);
  
  result.replace("*WCOND*", weatherCond);
  
  sprintf(buffer, "%.1f", currencyEUR);
  result.replace("*EUR*", buffer);
  
  sprintf(buffer, "%.1f", currencyUSD);
  result.replace("*USD*", buffer);
  
  sprintf(buffer, "%.1f", currencyBTC);
  result.replace("*BTC*", buffer);
  
  return result;
}
