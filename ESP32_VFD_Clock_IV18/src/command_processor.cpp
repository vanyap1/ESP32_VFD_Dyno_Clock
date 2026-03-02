#include "command_processor.h"
#include "display_utils.h"
#include "led_effects.h"
#include "network_utils.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <esp_task_wdt.h>
#include <NTPClient.h>
#include <sys/time.h>
#include <Wire.h>
#include "FastLED.h"
#include "WebRoot/html_page.h"
#include "WebRoot/scripts.h"
#include "WebRoot/styles.h"
#include "WebRoot/images.h"

// External references from main.cpp
extern SystemSetup sysSetupStruc;
extern char weatherCond[16];
extern bool offlineMode;
extern const char *ntpServers[];
extern NTPClient *timeClient;
extern WiFiUDP *ntpUDP;
extern uint32_t customCharData[96];
extern CRGB leds[];
extern float sensorTemp;
extern int sensorPress;
extern int sensorHum;
extern float weatherTemp;
extern float currencyEUR;
extern float currencyUSD;
extern float currencyBTC;

// HTTP Response Helper Functions
void sendHTTPHeader(WiFiClient& client, int statusCode, const char* contentType) {
  switch (statusCode) {
    case 200:
      client.println("HTTP/1.1 200 OK");
      break;
    case 400:
      client.println("HTTP/1.1 400 Bad Request");
      break;
    case 404:
      client.println("HTTP/1.1 404 Not Found");
      break;
    case 500:
      client.println("HTTP/1.1 500 Internal Server Error");
      break;
    default:
      client.println("HTTP/1.1 200 OK");
      break;
  }
  client.print("Content-type:");
  client.println(contentType);
  client.println("Connection: close");
  client.println();
}

void sendHTTPHeaderWithLength(WiFiClient& client, int statusCode, const char* contentType, size_t contentLength) {
  switch (statusCode) {
    case 200:
      client.println("HTTP/1.1 200 OK");
      break;
    case 400:
      client.println("HTTP/1.1 400 Bad Request");
      break;
    case 404:
      client.println("HTTP/1.1 404 Not Found");
      break;
    case 500:
      client.println("HTTP/1.1 500 Internal Server Error");
      break;
    default:
      client.println("HTTP/1.1 200 OK");
      break;
  }
  client.print("Content-type:");
  client.println(contentType);
  client.print("Content-Length: ");
  client.println(contentLength);
  client.println("Connection: close");
  client.println();
}

void sendOKResponse(WiFiClient& client, const char* message) {
  sendHTTPHeader(client, 200, "text/plain");
  client.println(message);
}

void sendJSONHeader(WiFiClient& client) {
  sendHTTPHeader(client, 200, "application/json");
}

void sendHTMLHeader(WiFiClient& client) {
  sendHTTPHeader(client, 200, "text/html");
}

void sendCSSHeader(WiFiClient& client) {
  sendHTTPHeader(client, 200, "text/css");
}

void sendJSHeader(WiFiClient& client) {
  sendHTTPHeader(client, 200, "text/javascript");
}

void sendBinaryHeader(WiFiClient& client, const char* contentType) {
  sendHTTPHeader(client, 200, contentType);
}

void sendErrorResponse(WiFiClient& client, int statusCode, const char* message) {
  sendHTTPHeader(client, statusCode, "text/plain");
  client.println(message);
}

bool processHTTPCommand(WiFiClient& client, String& header, 
                       PT63XX& vfd, RV8803& rtc, HDC2010& tempHumiditySensor,
                       bool rtcAvailable, bool& screeenUpdateRestricted) {
  
  String currentLine = "";
  
  // Handle captive portal detection requests from various platforms
  if (offlineMode) {
    // Check for captive portal detection URLs
    if (header.indexOf("GET /generate_204") >= 0 ||      // Android
        header.indexOf("GET /gen_204") >= 0 ||            // Android alternative
        header.indexOf("GET /hotspot-detect.html") >= 0 || // iOS/macOS
        header.indexOf("GET /library/test/success.html") >= 0 || // iOS alternative
        header.indexOf("GET /connecttest.txt") >= 0 ||     // Windows
        header.indexOf("GET /ncsi.txt") >= 0 ||            // Windows
        header.indexOf("GET /redirect") >= 0 ||            // Generic
        header.indexOf("GET /success.txt") >= 0 ||         // Various devices
        header.indexOf("GET /canonical.html") >= 0) {      // Ubuntu
      
      Serial.print("Captive portal request: ");
      String path = header.substring(header.indexOf("GET ") + 4, header.indexOf(" HTTP"));
      Serial.println(path);
      
      // For connecttest.txt, return text response
      if (header.indexOf("/connecttest.txt") >= 0 || header.indexOf("/ncsi.txt") >= 0) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Content-Length: 23");
        client.println("Connection: close");
        client.println();
        client.print("Microsoft Connect Test");
        client.flush();
        return true;
      }
      
      // For success.txt, return OK
      if (header.indexOf("/success.txt") >= 0) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Content-Length: 7");
        client.println("Connection: close");
        client.println();
        client.print("success");
        client.flush();
        return true;
      }
      
      // For Android generate_204, return proper response
      if (header.indexOf("/generate_204") >= 0 || header.indexOf("/gen_204") >= 0) {
        client.println("HTTP/1.1 302 Found");
        client.println("Location: http://192.168.4.1/");
        client.println("Cache-Control: no-cache, no-store, must-revalidate");
        client.println("Connection: close");
        client.println();
        client.flush();
        return true;
      }
      
      // For all others, redirect to main page
      client.println("HTTP/1.1 302 Found");
      client.println("Location: http://192.168.4.1/");
      client.println("Cache-Control: no-cache");
      client.println("Connection: close");
      client.println();
      client.flush();
      return true;
    }
  }
  
  if (header.indexOf("GET /cmd=GET:SETTINGS?") >= 0) {
    sendJSONHeader(client);

    StaticJsonDocument<1024> jsonDoc;
    
    JsonObject wifi = jsonDoc.createNestedObject("wifi");
    wifi["ssid"] = sysSetupStruc.ssid;
    wifi["security"] = 0; // WPA2
    
    JsonObject ntp = jsonDoc.createNestedObject("ntp");
    ntp["server"] = sysSetupStruc.ntpServerIndex;
    ntp["enabled"] = sysSetupStruc.ntpEN ? true : false;
    
    // Send timezone as hour offset (-12..+12)
    jsonDoc["timezone"] = sysSetupStruc.ntpTimeZone;
    JsonArray formats = jsonDoc.createNestedArray("formats");
    for (int i = 0; i < 3; i++) {
      JsonObject format = formats.createNestedObject();
      format["text"] = sysSetupStruc.displayFormat[i];
      format["time"] = sysSetupStruc.displayFormatTime[i];
      format["enabled"] = sysSetupStruc.displayFormatEnable[i];
      format["blink"] = sysSetupStruc.displayFormatBlink[i];
    }
    
    JsonObject blink = jsonDoc.createNestedObject("blink");
    char maskStr[16];
    sprintf(maskStr, "0x%X", sysSetupStruc.blinkMask);
    blink["mask"] = maskStr;
    blink["position"] = sysSetupStruc.blinkPosition;

    JsonObject sensors = jsonDoc.createNestedObject("sensors");
    sensors["pressure"] = sysSetupStruc.sensorPressure;
    sensors["temperature"] = sysSetupStruc.sensorTemperature;
    sensors["autobrightness"] = sysSetupStruc.sensorAutoBrightness;
    sensors["weatherapi"] = sysSetupStruc.sensorWeatherApi;
    sensors["currency"] = sysSetupStruc.sensorCurrency;
    
    JsonObject display = jsonDoc.createNestedObject("display");
    display["brightness"] = sysSetupStruc.displayBrightness;

    JsonObject led = jsonDoc.createNestedObject("led");
    
    JsonObject ledColor = led.createNestedObject("color");
    ledColor["r"] = sysSetupStruc.ambLightColr[0];
    ledColor["g"] = sysSetupStruc.ambLightColr[1];
    ledColor["b"] = sysSetupStruc.ambLightColr[2];
    
    led["brightness"] = sysSetupStruc.ambLightBrightness;
    led["count"] = sysSetupStruc.ledCount;
    led["effect"] = sysSetupStruc.ledEffect;
    
    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);
    client.println(jsonResponse);
    return true;
  }
  
  else if (header.indexOf("POST /cmd=FIRMWARE") >= 0) {
    Serial.println("Starting OTA firmware update...");
    
    int contentLength = 0;
    int headerStart = header.indexOf("Content-Length: ");
    if (headerStart != -1) {
      headerStart += 16; 
      int headerEnd = header.indexOf('\r', headerStart);
      if (headerEnd == -1) headerEnd = header.indexOf('\n', headerStart);
      String lengthStr = header.substring(headerStart, headerEnd);
      contentLength = lengthStr.toInt();
    }

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r" || line.length() == 0) break;
    }
    
    Serial.print("Expected firmware size: ");
    Serial.print(contentLength);
    Serial.println(" bytes");
    
    if (contentLength > 0) {
      if (!Update.begin(contentLength)) {
        Serial.println("Not enough space for OTA update");
        sendErrorResponse(client, 500, "Not enough space");
      } else {
        size_t written = 0;
        uint8_t buffer[4096]; // Increased buffer size for faster OTA
        
        while (written < contentLength && client.connected()) {
          esp_task_wdt_reset(); // Reset watchdog during OTA
          
          size_t available = client.available();
          if (available) {
            size_t toRead = min(available, sizeof(buffer));
            size_t bytesRead = client.read(buffer, toRead);
            
            if (Update.write(buffer, bytesRead) != bytesRead) {
              Serial.println("Write error during OTA update");
              break;
            }
            
            written += bytesRead;
            
            if (written % 10240 == 0 || written == contentLength) {
              Serial.print("Progress: ");
              Serial.print((written * 100) / contentLength);
              Serial.println("%");
            }
          } else {
            delay(10); // Increased delay for stability
          }
        }
        
        if (written == contentLength) {
          if (Update.end(true)) {
            Serial.println("OTA update completed successfully");
            
            sendOKResponse(client, "Firmware updated successfully. Restarting...");
            
            delay(1000);
            ESP.restart();
          } else {
            Serial.print("Update end error: ");
            Serial.println(Update.errorString());
            
            sendHTTPHeader(client, 500, "text/plain");
            client.print("Update error: ");
            client.println(Update.errorString());
          }
        } else {
          Serial.println("Incomplete firmware upload");
          Update.abort();
          
          sendErrorResponse(client, 400, "Incomplete firmware data");
        }
      }
    } else {
      sendErrorResponse(client, 400, "Invalid content length");
    }
    return true;
  }
  
  else if (header.indexOf("POST /cmd=LOAD") >= 0) {
    Serial.println("Receiving character table data...");
    
    while (client.available() && currentLine.length() > 0) {
      char c = client.read();
      if (c == '\n') {
        if (currentLine.length() == 0) break;
        currentLine = "";
      } else if (c != '\r') {
        currentLine += c;
      }
    }
    
    uint8_t* dataPtr = (uint8_t*)sysSetupStruc.customCharData;
    int bytesRead = 0;
    int expectedBytes = sizeof(sysSetupStruc.customCharData);
    
    unsigned long startTime = millis();
    while (bytesRead < expectedBytes && (millis() - startTime < 5000)) {
      if (client.available()) {
        dataPtr[bytesRead++] = client.read();
      }
    }
    
    if (bytesRead == expectedBytes) {
      memcpy(customCharData, sysSetupStruc.customCharData, sizeof(customCharData));
      EEPROM.put(0, sysSetupStruc);
      EEPROM.commit();
      
      Serial.print("Character table loaded successfully: ");
      Serial.print(bytesRead);
      Serial.println(" bytes");
      
      sendOKResponse(client, "Character table loaded successfully");
    } else {
      Serial.print("Error: Expected ");
      Serial.print(expectedBytes);
      Serial.print(" bytes, got ");
      Serial.println(bytesRead);
      
      sendErrorResponse(client, 400, "Error: Incomplete data received");
    }
    return true;
  }
  
  else if (header.indexOf("POST /submit") >= 0) {
    String requestBody = "";
    while (client.available()) {
      char c = client.read();
      requestBody += c;
    }
    Serial.println(requestBody);
    StaticJsonDocument<500> jsonDoc;
    deserializeJson(jsonDoc, requestBody);
    if (jsonDoc.containsKey("demoConf")) {
      sysSetupStruc.screenDemoMode = jsonDoc["demoConf"];
    }
    if (jsonDoc.containsKey("ntpServer")) {
      sysSetupStruc.ntpServerIndex = jsonDoc["ntpServer"];
      if (!offlineMode && sysSetupStruc.ntpEN && timeClient != nullptr) {
        timeClient->setPoolServerName(ntpServers[sysSetupStruc.ntpServerIndex]);
        timeClient->setTimeOffset(sysSetupStruc.ntpTimeZone * 3600);
        configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);
      }
    }
    if (jsonDoc.containsKey("timezone")) {
      sysSetupStruc.ntpTimeZone = jsonDoc["timezone"];
      if (!offlineMode && sysSetupStruc.ntpEN && timeClient != nullptr) {
        timeClient->setTimeOffset(sysSetupStruc.ntpTimeZone * 3600);
        configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);
        
        // Re-sync time with new timezone
        Serial.println("Timezone changed, re-syncing time from NTP...");
        if (timeClient->update()) {
          unsigned long epochTime = timeClient->getEpochTime();
          struct timeval tv = { .tv_sec = (time_t)epochTime, .tv_usec = 0 };
          settimeofday(&tv, NULL);
          
          delay(50);
          
          // Update I2C RTC with new timezone-adjusted time
          if (rtcAvailable) {
            rtc.setFromSystemTime();
            Serial.println("I2C RTC updated with new timezone");
          } else {
            Serial.println("ESP32 RTC updated with new timezone");
          }
        } else {
          Serial.println("NTP re-sync failed after timezone change");
        }
      }
    }
    if (jsonDoc.containsKey("manualTime")) {
      sysSetupStruc.ntpEN = (jsonDoc["manualTime"] == "on") ? 1 : 0;
    }
    if (jsonDoc.containsKey("ambiLightColor")) {
      String color = jsonDoc["ambiLightColor"];
      sysSetupStruc.ambLightColr[0] = strtol(color.substring(1, 3).c_str(), NULL, 16);
      sysSetupStruc.ambLightColr[1] = strtol(color.substring(3, 5).c_str(), NULL, 16);
      sysSetupStruc.ambLightColr[2] = strtol(color.substring(5, 7).c_str(), NULL, 16);
    }
    if (jsonDoc.containsKey("brightness")) {
      sysSetupStruc.ambLightBrightness = jsonDoc["brightness"];
    }
    if (jsonDoc.containsKey("ambiLight")) {
      sysSetupStruc.ambLightEnable = (jsonDoc["ambiLight"] == "on") ? 1 : 0;
    }
    EEPROM.put(0, sysSetupStruc);
    EEPROM.commit();
    sendOKResponse(client, "Settings updated");
    return true;
  }
  else if (header.indexOf("GET / ") >= 0) {
    screeenUpdateRestricted = false;
    
    // Small delay to ensure client is ready (helps with captive portal detection)
    delay(10);
    
    // Check if client is still connected before sending
    if (!client.connected()) {
      Serial.println("Client disconnected before sending response");
      return false;
    }
    
    // Calculate content length once
    size_t contentLength = strlen(index_html);
    sendHTTPHeaderWithLength(client, 200, "text/html", contentLength);
    
    // Use smaller chunks in offline/AP mode for better mobile device compatibility
    const char* ptr = index_html;
    size_t remaining = contentLength;
    const size_t chunkSize = offlineMode ? 512 : 2048;  // Smaller chunks for AP mode
    
    while (remaining > 0 && client.connected()) {
      size_t toSend = (remaining > chunkSize) ? chunkSize : remaining;
      size_t sent = client.write((const uint8_t*)ptr, toSend);
      
      if (sent == 0) {
        // Write failed, client disconnected
        Serial.println("Client disconnected during transfer");
        break;
      }
      
      ptr += sent;
      remaining -= sent;
      
      // Yield to allow WiFi stack to process, more often in AP mode
      if (remaining > 0) {
        yield();
        if (offlineMode) {
          delay(5);  // Longer delay for AP mode stability
        }
      }
    }
    
    if (remaining == 0) {
      client.flush();
    }
    return true;
  }
  else if (header.indexOf("GET /chargen") >= 0) {
    delay(10);
    
    if (!client.connected()) {
      Serial.println("Client disconnected before sending chargen");
      return false;
    }
    
    size_t contentLength = strlen(charGen);
    sendHTTPHeaderWithLength(client, 200, "text/html", contentLength);
    
    const char* ptr = charGen;
    size_t remaining = contentLength;
    const size_t chunkSize = offlineMode ? 512 : 2048;
    
    while (remaining > 0 && client.connected()) {
      size_t toSend = (remaining > chunkSize) ? chunkSize : remaining;
      size_t sent = client.write((const uint8_t*)ptr, toSend);
      
      if (sent == 0) break;
      
      ptr += sent;
      remaining -= sent;
      
      if (remaining > 0) {
        yield();
        if (offlineMode) delay(1);
      }
    }
    client.flush();
    return true;
  }
  
  else if (header.indexOf("GET /styles.css") >= 0) {
    delay(10);
    
    if (!client.connected()) {
      return false;
    }
    
    size_t contentLength = strlen(commonStyles);
    sendHTTPHeaderWithLength(client, 200, "text/css", contentLength);
    
    const char* ptr = commonStyles;
    size_t remaining = contentLength;
    const size_t chunkSize = offlineMode ? 512 : 2048;
    
    while (remaining > 0 && client.connected()) {
      size_t toSend = (remaining > chunkSize) ? chunkSize : remaining;
      size_t sent = client.write((const uint8_t*)ptr, toSend);
      
      if (sent == 0) break;
      
      ptr += sent;
      remaining -= sent;
      
      if (remaining > 0) {
        yield();
        if (offlineMode) delay(5);
      }
    }
    
    if (remaining == 0) {
      client.flush();
    }
    return true;
  }
  
  else if (header.indexOf("GET /commonRest.js") >= 0) {
    delay(10);
    
    if (!client.connected()) {
      return false;
    }
    
    size_t contentLength = strlen(commonRest);
    sendHTTPHeaderWithLength(client, 200, "text/javascript", contentLength);
    
    const char* ptr = commonRest;
    size_t remaining = contentLength;
    const size_t chunkSize = offlineMode ? 512 : 2048;
    
    while (remaining > 0 && client.connected()) {
      size_t toSend = (remaining > chunkSize) ? chunkSize : remaining;
      size_t sent = client.write((const uint8_t*)ptr, toSend);
      
      if (sent == 0) break;
      
      ptr += sent;
      remaining -= sent;
      
      if (remaining > 0) {
        yield();
        if (offlineMode) delay(5);
      }
    }
    
    if (remaining == 0) {
      client.flush();
    }
    return true;
  }
  
  else if (header.indexOf("GET /favicon.ico") >= 0) {
    delay(10);
    
    if (!client.connected()) {
      return false;
    }
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:image/x-icon");
    client.print("Content-Length: ");
    client.println(sizeof(favicon_ico));
    client.println("Connection: close");
    client.println("Content-Transfer-Encoding: binary");
    client.println();
    
    // Send favicon in chunks for better stability
    const uint8_t* ptr = (const uint8_t*)favicon_ico;
    size_t remaining = sizeof(favicon_ico);
    const size_t chunkSize = 512;
    
    while (remaining > 0 && client.connected()) {
      size_t toSend = (remaining > chunkSize) ? chunkSize : remaining;
      size_t sent = client.write(ptr, toSend);
      
      if (sent == 0) break;
      
      ptr += sent;
      remaining -= sent;
      
      if (remaining > 0) {
        yield();
      }
    }
    
    if (remaining == 0) {
      client.flush();
    }
    return true;
  }
  
  else if (header.indexOf("GET /cmd=GET_DEVICE_INFO") >= 0) {
    sendHTTPHeader(client, 200, "text/plain");
    String deviceInfo = String(WiFi.localIP().toString()) + "," +
                        String(80) + "," +
                        String(8080) + "," +
                        "ESP32 VFD Clock," +
                        String(0) + "," +
                        String(0) + "," +
                        String(sysSetupStruc.screenDemoMode) + "," +
                        String(sysSetupStruc.ntpServerIndex) + "," +
                        String(offlineMode ? "OFFLINE" : "READY");
    client.println(deviceInfo);
    return true;
  }
  
  else if (header.indexOf("GET /cmd=DUMP?") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/octet-stream");
    client.println("Content-Disposition: attachment; filename=\"chartab.bin\"");
    client.println("Connection: close");
    client.println();
    
    client.write((const uint8_t*)sysSetupStruc.customCharData, sizeof(sysSetupStruc.customCharData));
    
    Serial.println("Custom character table dumped (384 bytes)");
    return true;
  }
  
  else if (header.indexOf("GET /cmd=SEG?") >= 0) {
    sendHTTPHeader(client, 200, "text/plain");
    String dumpData = "";
    if (sysSetupStruc.segmentsBitMask[0] == 0xFF) {
      dumpData = "x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,dp,g,f,e,d,c,b,a";
    } else {
      for (int i = 0; i < 96 && sysSetupStruc.segmentsBitMask[i] != '\0'; i++) {
        char byte = (char)sysSetupStruc.segmentsBitMask[i];
        if ((uint8_t)byte == 0xFF) {
          dumpData += 'x';
        } else {
          dumpData += byte;
        }
      }
    }

    Serial.print("SEG response: ");
    Serial.println(dumpData);

    client.println(dumpData);
    return true;
  }
  
  else if (header.indexOf("GET /cmd=SEG=") >= 0) {
    int paramStart = header.indexOf("GET /cmd=SEG=") + strlen("GET /cmd=SEG=");
    String params = header.substring(paramStart, header.indexOf(" ", paramStart));
    params.trim();

    Serial.print("Received SEG command with params: ");
    Serial.println(params);
    
    int len = params.length();
    if (len > 95) len = 95; 
    for (int i = 0; i < len; i++) {
      sysSetupStruc.segmentsBitMask[i] = (uint8_t)params[i];
    }

    sysSetupStruc.segmentsBitMask[len] = '\0';

    EEPROM.put(0, sysSetupStruc);
    EEPROM.commit();

    Serial.print("Segment bitmask updated. Total bytes: ");
    Serial.println(len);

    sendOKResponse(client, "Segment bitmask updated");
    return true;
  }
  
  else if (header.indexOf("GET /cmd=CLOCK") >= 0) {
    screeenUpdateRestricted = false;
    sendOKResponse(client, "Switched to CLOCK mode");
    return true;
  }
  
  else if (header.indexOf("GET /cmd=RESTART") >= 0) {
    sendOKResponse(client, "Restarting device...");
    delay(1000);
    ESP.restart();
    return true;
  }
  
  else if (header.indexOf("GET /cmd=TEXT") >= 0) {
    screeenUpdateRestricted = true;
    int paramStart = header.indexOf("GET /cmd=TEXT") + strlen("GET /cmd=TEXT");
    String textToDisplay = header.substring(paramStart, header.indexOf(" ", paramStart));
    textToDisplay.trim();

    if (textToDisplay.startsWith("%20") || textToDisplay.startsWith(" ")) {
      textToDisplay = textToDisplay.substring(textToDisplay.startsWith("%20") ? 3 : 1);
    }
    
    textToDisplay = urlDecode(textToDisplay);
    
    Serial.print("Displaying text: ");
    Serial.println(textToDisplay);
    vfd.writeStringUniverslaChrTab(textToDisplay.c_str(), 0);
    sendOKResponse(client, "Text displayed");
    return true;
  }
  
  else if (header.indexOf("GET /cmd=WIFI:SCAN") >= 0) {
    Serial.println("WiFi networks scan requested");
    
    // Check if we need to rescan or use cached results
    bool forceRescan = (header.indexOf("force=1") >= 0);
    int cachedCount = getStoredNetworksCount();
    
    Serial.print("Cached networks count: ");
    Serial.println(cachedCount);
    Serial.print("Force rescan: ");
    Serial.println(forceRescan ? "YES" : "NO");
    
    if (forceRescan || cachedCount == 0) {
      Serial.println("Performing new WiFi scan...");
      SystemState previousState = getSystemState();
      setSystemState(STATE_SCANNING);
      
      scanAndStoreNetworks();
      
      setSystemState(previousState);
    } else {
      Serial.println("Using cached WiFi scan results");
    }
    
    int n = getStoredNetworksCount();
    Serial.print("Networks to send: ");
    Serial.println(n);
    
    sendJSONHeader(client);
    
    client.print("[");
    for (int i = 0; i < n; i++) {
      if (i > 0) client.print(",");
      WiFiScanResult network = getStoredNetwork(i);
      client.print("{\"ssid\":\"");
      client.print(network.ssid);
      client.print("\",\"rssi\":");
      client.print(network.rssi);
      client.print(",\"secure\":");
      client.print(network.secure ? "true" : "false");
      client.print("}");
    }
    client.print("]");
    client.flush();
    
    Serial.println("WiFi scan results sent");
    
    return true;
  }
  
  else if (header.indexOf("GET /cmd=CHARSET") >= 0) {
    screeenUpdateRestricted = true;
    int paramStart = header.indexOf("GET /cmd=CHARSET") + strlen("GET /cmd=CHARSET");
    String params = header.substring(paramStart, header.indexOf(" ", paramStart));
    params.trim();
    
    if (params.startsWith(",")) {
      params = params.substring(1);
    }
    
    int firstComma = params.indexOf(",");
    int secondComma = params.indexOf(",", firstComma + 1);
    if (firstComma != -1 && secondComma != -1) {
      String posStr = params.substring(0, firstComma);
      String asciiStr = params.substring(firstComma + 1, secondComma);
      String segStr = params.substring(secondComma + 1);
      int position = posStr.toInt();
      int asciiCode = strtol(asciiStr.c_str(), NULL, 16);
      long segments = strtol(segStr.c_str(), NULL, 16);
      Serial.print("Setting char in custom table at pos ");;
      Serial.print(position);
      Serial.print(" with ASCII code ");
      Serial.print(asciiCode);
      Serial.print(" and segments 0x");
      Serial.println(segStr);
      vfd.setCharToCustomTable(asciiCode, segments);
      
      memcpy(sysSetupStruc.customCharData, customCharData, sizeof(customCharData));
      EEPROM.put(0, sysSetupStruc);
      EEPROM.commit();

      sendOKResponse(client, "Character set in custom table");
    } else {
      sendErrorResponse(client, 400, "Invalid parameters");
    }
    return true;
  }
  
  else if (header.indexOf("GET /cmd=CHARTEST") >= 0) {
    screeenUpdateRestricted = true;
    int paramStart = header.indexOf("GET /cmd=CHARTEST") + strlen("GET /cmd=CHARTEST");
    String params = header.substring(paramStart, header.indexOf(" ", paramStart));
    params.trim();
    
    if (params.startsWith(",")) {
      params = params.substring(1);
    }
    
    int firstComma = params.indexOf(",");
    int secondComma = params.indexOf(",", firstComma + 1);
    if (firstComma != -1 && secondComma != -1) {
      String posStr = params.substring(0, firstComma);
      String asciiStr = params.substring(firstComma + 1, secondComma);
      String segStr = params.substring(secondComma + 1);
      int position = posStr.toInt();
      int asciiCode = strtol(asciiStr.c_str(), NULL, 16);
      long segments = strtol(segStr.c_str(), NULL, 16);
      Serial.print("Defining char at pos ");;
      Serial.print(position);
      Serial.print(" with ASCII code ");
      Serial.print(asciiCode);
      Serial.print(" and segments 0x");
      Serial.println(segStr);
      vfd.writeRawData(position, segments);

      sendOKResponse(client, "Character defined and displayed");
    } else {
      sendErrorResponse(client, 400, "Invalid parameters");
    }
    return true;
  }
  
  
  
  else if (header.indexOf("GET /cmd=WIFI:SAVE=") >= 0) {
    int paramStart = header.indexOf("GET /cmd=WIFI:SAVE=") + strlen("GET /cmd=WIFI:SAVE=");
    String params = header.substring(paramStart, header.indexOf(" ", paramStart));
    params = urlDecode(params);
    
    Serial.print("WIFI:SAVE received params: ");
    Serial.println(params);
    
    int firstComma = params.indexOf(",");
    int secondComma = params.indexOf(",", firstComma + 1);
    
    if (firstComma != -1 && secondComma != -1) {
      String ssid = params.substring(0, firstComma);
      String password = params.substring(firstComma + 1, secondComma);
      String security = params.substring(secondComma + 1);
      
      Serial.print("Saving SSID: ");
      Serial.println(ssid);
      Serial.print("Password length: ");
      Serial.println(password.length());
      
      ssid.toCharArray(sysSetupStruc.ssid, sizeof(sysSetupStruc.ssid));
      password.toCharArray(sysSetupStruc.pass, sizeof(sysSetupStruc.pass));
      sysSetupStruc.FirstStart = 55;  // Mark as configured
      
      EEPROM.put(0, sysSetupStruc);
      EEPROM.commit();
      
      Serial.println("WiFi settings saved to EEPROM");
      
      sendOKResponse(client, "WiFi settings saved. Device will restart...");
      client.flush();
      client.stop();
      
      Serial.println("Restarting in 2 seconds...");
      delay(2000);
      ESP.restart();
    } else {
      Serial.println("Invalid WIFI:SAVE parameters!");
      sendErrorResponse(client, 400, "Invalid parameters");
    }
    return true;
  }
  
  else if (header.indexOf("GET /cmd=DATETIME,") >= 0) {
    int paramStart = header.indexOf("GET /cmd=DATETIME,") + strlen("GET /cmd=DATETIME,");
    String params = header.substring(paramStart, header.indexOf(" ", paramStart));
    
    int year, month, day, hour, minute, second;
    if (sscanf(params.c_str(), "%d,%d,%d,%d,%d,%d", &year, &month, &day, &hour, &minute, &second) == 6) {
      struct tm manualTime;
      manualTime.tm_year = year - 1900;
      manualTime.tm_mon = month - 1;
      manualTime.tm_mday = day;
      manualTime.tm_hour = hour;
      manualTime.tm_min = minute;
      manualTime.tm_sec = second;
      
      time_t t = mktime(&manualTime);
      struct timeval now = { .tv_sec = t };
      settimeofday(&now, NULL);  // Sets both ESP32 RTC and system time
      
      // Sync I2C RTC if available
      // ESP32 built-in RTC is already set by settimeofday()
      if (rtcAvailable) {
        rtc.setFromSystemTime();
        Serial.println("I2C RTC updated with manual time");
      } else {
        Serial.println("ESP32 built-in RTC updated with manual time");
      }
      
      Serial.println("Manual time set");
      
      sendOKResponse(client, "Time set successfully");
    } else {
      sendErrorResponse(client, 400, "Invalid date format");
    }
    return true;
  }
  
  else if (header.indexOf("GET /cmd=NTP:SERVER=") >= 0) {
    int paramStart = header.indexOf("GET /cmd=NTP:SERVER=") + strlen("GET /cmd=NTP:SERVER=");
    String value = header.substring(paramStart, header.indexOf(" ", paramStart));
    
    sysSetupStruc.ntpServerIndex = value.toInt();
    configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);
    
    EEPROM.put(0, sysSetupStruc);
    EEPROM.commit();
    
    Serial.print("NTP server set to: ");
    Serial.println(sysSetupStruc.ntpServerIndex);
    
    sendOKResponse(client, "NTP server updated");
    return true;
  }
  
  else if (header.indexOf("GET /cmd=TIMEZONE=") >= 0) {
    int paramStart = header.indexOf("GET /cmd=TIMEZONE=") + strlen("GET /cmd=TIMEZONE=");
    String value = header.substring(paramStart, header.indexOf(" ", paramStart));
    
    // Store timezone as hour offset (-12..+12)
    sysSetupStruc.ntpTimeZone = value.toInt();
    
    if (!offlineMode && sysSetupStruc.ntpEN && timeClient != nullptr) {
      timeClient->setTimeOffset(sysSetupStruc.ntpTimeZone * 3600);
      configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);
      
      // Re-sync time with new timezone
      Serial.println("Timezone changed, re-syncing time from NTP...");
      if (timeClient->update()) {
        unsigned long epochTime = timeClient->getEpochTime();
        struct timeval tv = { .tv_sec = (time_t)epochTime, .tv_usec = 0 };
        settimeofday(&tv, NULL);
        
        delay(50);
        
        // Update I2C RTC with new timezone-adjusted time
        if (rtcAvailable) {
          rtc.setFromSystemTime();
          Serial.println("I2C RTC updated with new timezone");
        } else {
          Serial.println("ESP32 RTC updated with new timezone");
        }
      } else {
        Serial.println("NTP re-sync failed after timezone change");
      }
    }
    
    EEPROM.put(0, sysSetupStruc);
    EEPROM.commit();
    
    Serial.print("Timezone set to: ");
    Serial.println(sysSetupStruc.ntpTimeZone);
    
    sendOKResponse(client, "Timezone updated");
    return true;
  }
  
  else if (header.indexOf("GET /cmd=DISPLAY:SCREEN") >= 0) {
    int screenNum = -1;
    if (header.indexOf("GET /cmd=DISPLAY:SCREEN1=") >= 0) {
      screenNum = 0;
    } else if (header.indexOf("GET /cmd=DISPLAY:SCREEN2=") >= 0) {
      screenNum = 1;
    } else if (header.indexOf("GET /cmd=DISPLAY:SCREEN3=") >= 0) {
      screenNum = 2;
    }
    
    if (screenNum != -1) {
      String searchStr = "GET /cmd=DISPLAY:SCREEN" + String(screenNum + 1) + "=";
      int paramStart = header.indexOf(searchStr) + searchStr.length();
      String params = header.substring(paramStart, header.indexOf(" ", paramStart));
      params = urlDecode(params);
      
      int comma1 = params.indexOf(",");
      int comma2 = params.indexOf(",", comma1 + 1);
      int comma3 = params.indexOf(",", comma2 + 1);
      
      if (comma1 != -1 && comma2 != -1 && comma3 != -1) {
        String format = params.substring(0, comma1);
        format.toCharArray(sysSetupStruc.displayFormat[screenNum], sizeof(sysSetupStruc.displayFormat[screenNum]));
        sysSetupStruc.displayFormatTime[screenNum] = params.substring(comma1 + 1, comma2).toInt();
        sysSetupStruc.displayFormatEnable[screenNum] = params.substring(comma2 + 1, comma3).toInt();
        sysSetupStruc.displayFormatBlink[screenNum] = params.substring(comma3 + 1).toInt();
        
        EEPROM.put(0, sysSetupStruc);
        EEPROM.commit();
        
        Serial.print("Display Screen ");
        Serial.print(screenNum + 1);
        Serial.println(" settings saved");
        
        String response = "Screen " + String(screenNum + 1) + " updated";
        sendOKResponse(client, response.c_str());
      } else {
        sendErrorResponse(client, 400, "Invalid parameters");
      }
      return true;
    }
  }
  
  else if (header.indexOf("GET /cmd=BLINK:POINT=") >= 0) {
    int paramStart = header.indexOf("GET /cmd=BLINK:POINT=") + strlen("GET /cmd=BLINK:POINT=");
    String params = header.substring(paramStart, header.indexOf(" ", paramStart));
    
    int comma = params.indexOf(",");
    if (comma != -1) {
      String maskStr = params.substring(0, comma);
      String posStr = params.substring(comma + 1);
      
      sysSetupStruc.blinkMask = strtoul(maskStr.c_str(), NULL, 16);
      sysSetupStruc.blinkPosition = posStr.toInt();
      vfd.setBlinkCharData(sysSetupStruc.blinkMask, sysSetupStruc.blinkPosition);
      EEPROM.put(0, sysSetupStruc);
      EEPROM.commit();
      
      Serial.print("Blink mask: 0x");
      Serial.print(sysSetupStruc.blinkMask, HEX);
      Serial.print(", position: ");
      Serial.println(sysSetupStruc.blinkPosition);
      
      sendOKResponse(client, "Blink settings updated");
    } else {
      sendErrorResponse(client, 400, "Invalid parameters");
    }
    return true;
  }
  
  else if (header.indexOf("GET /cmd=SENSOR:") >= 0) {
    String sensorType = "";
    bool* sensorPtr = nullptr;
    bool hasSpecialLogic = false;
    
    if (header.indexOf("SENSOR:PRESSURE=") >= 0) {
      sensorType = "PRESSURE";
      sensorPtr = &sysSetupStruc.sensorPressure;
    } else if (header.indexOf("SENSOR:TEMPERATURE=") >= 0) {
      sensorType = "TEMPERATURE";
      sensorPtr = &sysSetupStruc.sensorTemperature;
    } else if (header.indexOf("SENSOR:AUTOBRIGHTNESS=") >= 0) {
      sensorType = "AUTOBRIGHTNESS";
      sensorPtr = &sysSetupStruc.sensorAutoBrightness;
      hasSpecialLogic = true;
    } else if (header.indexOf("SENSOR:WEATHERAPI=") >= 0) {
      sensorType = "WEATHERAPI";
      sensorPtr = &sysSetupStruc.sensorWeatherApi;
    } else if (header.indexOf("SENSOR:CURRENCY=") >= 0) {
      sensorType = "CURRENCY";
      sensorPtr = &sysSetupStruc.sensorCurrency;
    }
    
    if (sensorPtr != nullptr) {
      String searchStr = "GET /cmd=SENSOR:" + sensorType + "=";
      int paramStart = header.indexOf(searchStr) + searchStr.length();
      String value = header.substring(paramStart, header.indexOf(" ", paramStart));
      
      *sensorPtr = (value.toInt() == 1);
      
      if (hasSpecialLogic && sensorType == "AUTOBRIGHTNESS") {
        if (sysSetupStruc.sensorAutoBrightness) {
          time_t now;
          time(&now);
          struct tm timeinfo;
          localtime_r(&now, &timeinfo);
          int hour = timeinfo.tm_hour;
          vfd.setBrightness((hour >= 21 || hour < 7) ? 1 : 7);
        } else {
          vfd.setBrightness(sysSetupStruc.displayBrightness);
        }
      }
      
      EEPROM.put(0, sysSetupStruc);
      EEPROM.commit();
      
      Serial.print(sensorType);
      Serial.print(" sensor: ");
      Serial.println(*sensorPtr ? "ON" : "OFF");
      
      String response = sensorType + " sensor updated";
      sendOKResponse(client, response.c_str());
      return true;
    }
  }
  
  else if (header.indexOf("GET /cmd=LED:") >= 0) {
    if (header.indexOf("LED:COLOR=") >= 0) {
      int paramStart = header.indexOf("GET /cmd=LED:COLOR=") + strlen("GET /cmd=LED:COLOR=");
      String params = header.substring(paramStart, header.indexOf(" ", paramStart));
      
      int r, g, b;
      if (sscanf(params.c_str(), "%d,%d,%d", &r, &g, &b) == 3) {
        sysSetupStruc.ambLightColr[0] = r;
        sysSetupStruc.ambLightColr[1] = g;
        sysSetupStruc.ambLightColr[2] = b;
        
        EEPROM.put(0, sysSetupStruc);
        EEPROM.commit();
        
        Serial.printf("LED color: R=%d, G=%d, B=%d\n", r, g, b);
        
        sendOKResponse(client, "LED color updated");
      } else {
        sendErrorResponse(client, 400, "Invalid color format");
      }
      return true;
    }
    else if (header.indexOf("LED:BRIGHTNESS=") >= 0) {
      int paramStart = header.indexOf("GET /cmd=LED:BRIGHTNESS=") + strlen("GET /cmd=LED:BRIGHTNESS=");
      String value = header.substring(paramStart, header.indexOf(" ", paramStart));
      
      sysSetupStruc.ambLightBrightness = constrain(value.toInt(), 0, 255);
      
      EEPROM.put(0, sysSetupStruc);
      EEPROM.commit();
      
      Serial.print("LED brightness: ");
      Serial.println(sysSetupStruc.ambLightBrightness);
      
      sendOKResponse(client, "LED brightness updated");
      return true;
    }
    else if (header.indexOf("LED:COUNT=") >= 0) {
      int paramStart = header.indexOf("GET /cmd=LED:COUNT=") + strlen("GET /cmd=LED:COUNT=");
      String value = header.substring(paramStart, header.indexOf(" ", paramStart));
      
      sysSetupStruc.ledCount = value.toInt();
      
      EEPROM.put(0, sysSetupStruc);
      EEPROM.commit();
      
      Serial.print("LED count: ");
      Serial.println(sysSetupStruc.ledCount);
      
      sendOKResponse(client, "LED count updated");
      return true;
    }
    else if (header.indexOf("LED:EFFECT=") >= 0) {
      int paramStart = header.indexOf("GET /cmd=LED:EFFECT=") + strlen("GET /cmd=LED:EFFECT=");
      String value = header.substring(paramStart, header.indexOf(" ", paramStart));
      
      sysSetupStruc.ledEffect = value.toInt();
      
      EEPROM.put(0, sysSetupStruc);
      EEPROM.commit();
      
      Serial.print("LED effect: ");
      Serial.println(sysSetupStruc.ledEffect);
      
      sendOKResponse(client, "LED effect updated");
      return true;
    }
    else if (header.indexOf("LED:TEST") >= 0) {
      Serial.println("LED test sequence started");
      
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        leds[i] = CRGB::Red;
        FastLED.show();
        delay(200);
        leds[i] = CRGB::Green;
        FastLED.show();
        delay(200);
        leds[i] = CRGB::Blue;
        FastLED.show();
        delay(200);
        leds[i] = CRGB::Black;
        FastLED.show();
      }
      
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        leds[i] = CRGB(sysSetupStruc.ambLightColr[0], 
                       sysSetupStruc.ambLightColr[1], 
                       sysSetupStruc.ambLightColr[2]);
      }
      FastLED.show();
      
      Serial.println("LED test completed");
      sendOKResponse(client, "LED test completed");
      return true;
    }
  }
  
  else if (header.indexOf("GET /cmd=DISPLAY:BRIGHTNESS=") >= 0) {
    int paramStart = header.indexOf("GET /cmd=DISPLAY:BRIGHTNESS=") + strlen("GET /cmd=DISPLAY:BRIGHTNESS=");
    String value = header.substring(paramStart, header.indexOf(" ", paramStart));

    int brightness = constrain(value.toInt(), 0, 7);
    sysSetupStruc.displayBrightness = brightness;

    if (!sysSetupStruc.sensorAutoBrightness) {
      vfd.setBrightness(brightness);
    }
    
    EEPROM.put(0, sysSetupStruc);
    EEPROM.commit();
    
    Serial.print("Display brightness: ");
    Serial.println(sysSetupStruc.displayBrightness);
    
    sendOKResponse(client, "Display brightness updated");
    return true;
  }
  
  else if (header.indexOf("GET /cmd=I2C:SCAN") >= 0) {
    sendJSONHeader(client);
    
    byte count = 0;
    client.print("[");
    
    for (byte address = 1; address < 127; address++) {
      Wire.beginTransmission(address);
      byte error = Wire.endTransmission();
      
      if (error == 0) {
        if (count > 0) client.print(",");
        client.print("{\"address\":\"0x");
        if (address < 16) client.print("0");
        client.print(address, HEX);
        client.print("\",\"decimal\":");
        client.print(address);
        client.print("}");
        count++;
      }
    }
    
    client.print("]");
    
    Serial.print("I2C scan found ");
    Serial.print(count);
    Serial.println(" device(s)");
    
    return true;
  }
  
  else if (header.indexOf(":TEST") >= 0) {
    if (header.indexOf("HDC2010:TEST") >= 0) {
    sendHTTPHeader(client, 200, "text/plain");
    
    if (tempHumiditySensor.isAvailable()) {
      float temp = tempHumiditySensor.readTemperature();
      float hum = tempHumiditySensor.readHumidity();
      
      client.print("HDC2010 sensor OK\\n");
      client.print("Temperature: ");
      client.print(temp, 1);
      client.print(" C\\n");
      client.print("Humidity: ");
      client.print(hum, 1);
      client.print(" %");
      
      Serial.println("HDC2010 test: OK");
    } else {
      client.print("HDC2010 sensor not found");
      Serial.println("HDC2010 test: FAIL");
    }
    
      return true;
    }
    else if (header.indexOf("RTC:TEST") >= 0) {
      sendHTTPHeader(client, 200, "text/plain");
    
    if (rtc.isAvailable()) {
      uint8_t sec, min, hour, date, month;
      uint16_t year;
      
      // Get system time first
      time_t now;
      struct tm timeinfo;
      time(&now);
      localtime_r(&now, &timeinfo);
      
      client.print("System Time:\\n");
      client.print("Time: ");
      if (timeinfo.tm_hour < 10) client.print("0");
      client.print(timeinfo.tm_hour);
      client.print(":");
      if (timeinfo.tm_min < 10) client.print("0");
      client.print(timeinfo.tm_min);
      client.print(":");
      if (timeinfo.tm_sec < 10) client.print("0");
      client.print(timeinfo.tm_sec);
      client.print("\\n");
      client.print("Date: ");
      if (timeinfo.tm_mday < 10) client.print("0");
      client.print(timeinfo.tm_mday);
      client.print(".");
      if ((timeinfo.tm_mon + 1) < 10) client.print("0");
      client.print(timeinfo.tm_mon + 1);
      client.print(".");
      client.print(timeinfo.tm_year + 1900);
      client.print("\\n\\n");
      
      if (rtc.getTime(sec, min, hour, date, month, year)) {
        client.print("RV8803 RTC OK\\n");
        client.print("Time: ");
        if (hour < 10) client.print("0");
        client.print(hour);
        client.print(":");
        if (min < 10) client.print("0");
        client.print(min);
        client.print(":");
        if (sec < 10) client.print("0");
        client.print(sec);
        client.print("\\n");
        client.print("Date: ");
        if (date < 10) client.print("0");
        client.print(date);
        client.print(".");
        if (month < 10) client.print("0");
        client.print(month);
        client.print(".");
        client.print(year);
        
        Serial.println("RTC test: OK");
      } else {
        client.print("RTC read error");
        Serial.println("RTC test: READ FAIL");
      }
    } else {
      client.print("RV8803 RTC not found");
        Serial.println("RTC test: NOT FOUND");
      }
      
      return true;
    }
  }
  
  else if (header.indexOf("GET /cmd=RTC:SYNC") >= 0) {
    sendHTTPHeader(client, 200, "text/plain");
    
    if (rtc.isAvailable()) {
      // Get current system time
      time_t now;
      struct tm timeinfo;
      time(&now);
      localtime_r(&now, &timeinfo);
      
      // Sync RTC from system time
      if (rtc.setFromSystemTime()) {
        client.print("RTC synchronized with system time\\n");
        client.print("System time: ");
        if (timeinfo.tm_hour < 10) client.print("0");
        client.print(timeinfo.tm_hour);
        client.print(":");
        if (timeinfo.tm_min < 10) client.print("0");
        client.print(timeinfo.tm_min);
        client.print(":");
        if (timeinfo.tm_sec < 10) client.print("0");
        client.print(timeinfo.tm_sec);
        client.print(" ");
        if (timeinfo.tm_mday < 10) client.print("0");
        client.print(timeinfo.tm_mday);
        client.print(".");
        if ((timeinfo.tm_mon + 1) < 10) client.print("0");
        client.print(timeinfo.tm_mon + 1);
        client.print(".");
        client.print(timeinfo.tm_year + 1900);
        
        Serial.println("RTC synchronized with system time");
      } else {
        client.print("RTC sync failed");
        Serial.println("RTC sync failed");
      }
    } else {
      client.print("RV8803 RTC not found");
      Serial.println("RTC not available for sync");
    }
    
    return true;
  }
  
  else {
    sendErrorResponse(client, 404, "404 Not Found");
    return true;
  }
}
