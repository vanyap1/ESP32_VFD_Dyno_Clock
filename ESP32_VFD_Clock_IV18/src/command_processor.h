#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <Arduino.h>
#include <WiFi.h>
#include "pt63xx.h"
#include "RV8803.h"
#include "HDC2010.h"

// HTTP Response Helpers
void sendHTTPHeader(WiFiClient& client, int statusCode, const char* contentType);
void sendOKResponse(WiFiClient& client, const char* message);
void sendJSONHeader(WiFiClient& client);
void sendHTMLHeader(WiFiClient& client);
void sendCSSHeader(WiFiClient& client);
void sendJSHeader(WiFiClient& client);
void sendBinaryHeader(WiFiClient& client, const char* contentType);
void sendErrorResponse(WiFiClient& client, int statusCode, const char* message);

// Process HTTP commands from client
// Returns true if connection should be closed
bool processHTTPCommand(WiFiClient& client, String& header, 
                       PT63XX& vfd, RV8803& rtc, HDC2010& tempHumiditySensor,
                       bool rtcAvailable, bool& screeenUpdateRestricted);

#endif // COMMAND_PROCESSOR_H
