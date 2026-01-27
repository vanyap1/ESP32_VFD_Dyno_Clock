#include <Arduino.h>
#include <EEPROM.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include "dto.h"
#include "pt63xx.h"
#include <sys/time.h>
#include "WebRoot/html_page.h"
#include "WebRoot/scripts.h"
#include "WebRoot/styles.h"
#include "WebRoot/images.h"
#include "setup.h"
#include <time.h>
#include <esp_task_wdt.h>

#define LED_HTTP 16
#define LED_WIFI 17
#define INV_ENABLE 21

#define USR_BTN 4
#define VFD_EN


#define USE_PREDEFINED_SCREEN

PT63XX vfd(27, IV18);

// Змінні для зберігання часу
struct tm timeinfo;
bool blinkingDot = false;
SystemSetup sysSetupStruc;


WiFiServer serverConfigured(80);
WiFiUDP ntpUDP;

#define NTP_UPDATE_INTERVAL 60000  // Оновлення часу кожні 60 секунд
#define NTP_FIRST_UPDATE_DELAY 5000  // Затримка перед першим оновленням часу

uint32_t ntpUpdateInterval = NTP_FIRST_UPDATE_DELAY;  // Оновлення часу кожні 60 секунд


const char *ntpServers[] = {"pool.ntp.org", "time.google.com", "time.windows.com"};
NTPClient timeClient(ntpUDP, ntpServers[0]);

bool screeenUpdateRestricted = false;

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

void setup()
{
  pinMode(LED_HTTP, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(INV_ENABLE, OUTPUT);
  digitalWrite(INV_ENABLE, LOW); // Enable inverter power
  pinMode(USR_BTN, INPUT_PULLUP);

  sysSetupStruc.ntpServerIndex = 0;
  sysSetupStruc.ntpTimeZone = 0;
  
  Serial.begin(115200);
  //Serial.println("Hello, Arduino!");

  for (int i = 0; i < 2; i++)
  {
    digitalWrite(LED_WIFI, HIGH);
    delay(200);
    digitalWrite(LED_WIFI, LOW);
    delay(200);
  }
  vfd.begin();
  vfd.clearDisplay();
  delay(200);
  digitalWrite(INV_ENABLE, HIGH); // Enable inverter power

  EEPROM.begin(sizeof(sysSetupStruc) + 1);
  EEPROM.get(0, sysSetupStruc);
  
  // Завантажити customCharData з EEPROM
  memcpy(customCharData, sysSetupStruc.customCharData, sizeof(customCharData));

  if (digitalRead(USR_BTN) == LOW)
  {
    sysSetupStruc.FirstStart = 1;
  }

  if (sysSetupStruc.FirstStart != 55)
  {
    Serial.println("First start detected. Starting AP mode...");
    memset(&sysSetupStruc.ssid, 0, sizeof(sysSetupStruc.ssid));
    memset(&sysSetupStruc.pass, 0, sizeof(sysSetupStruc.pass));
    sysSetupStruc.FirstStart = 55;
    EEPROM.put(0, sysSetupStruc);
    EEPROM.commit();
    digitalWrite(LED_HTTP, HIGH);
    #if defined(USE_PREDEFINED_SCREEN)
      vfd.writeString("Conf", 1);
    #else
      vfd.writeStringUniverslaChrTab("Conf", 1);
    #endif
    ClientSetup();
  }

  #if defined(USE_PREDEFINED_SCREEN)
    vfd.writeString("HELLO", 1);
  #else
    vfd.writeStringUniverslaChrTab("HELLO", 1);
  #endif
  Serial.println("System Setup Data:");
  Serial.print("FirstStart: ");
  Serial.println(sysSetupStruc.FirstStart);
  Serial.print("SSID: ");
  Serial.println(sysSetupStruc.ssid);
  Serial.print("SSID LENGTH:  ");
  Serial.println(sizeof(sysSetupStruc.ssid));
  Serial.print("PASS: ");
  Serial.println(sysSetupStruc.pass);
  Serial.print("PASS LENGTH:  ");
  Serial.println(sizeof(sysSetupStruc.pass));
  digitalWrite(LED_HTTP, LOW);

  for (int i = 0; i < 3; i++)
  {
    digitalWrite(LED_WIFI, HIGH);
    delay(200);
    digitalWrite(LED_WIFI, LOW);
    delay(200);
  }
  Serial.println("Setup complete.");
  Serial.println("Connecting to WiFi...");
  digitalWrite(LED_WIFI, HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.begin(sysSetupStruc.ssid, sysSetupStruc.pass);

  int attempts = 0;
  const int maxAttempts = 20; // 20 * 500ms = 10 секунд

  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("\nWiFi connection failed! Starting AP mode...");
    digitalWrite(LED_WIFI, LOW);
    digitalWrite(LED_HTTP, HIGH);
    ClientSetup();
  }
  else
  {
    Serial.println("\nWiFi connected.");
  }
  digitalWrite(LED_WIFI, LOW);
  digitalWrite(LED_HTTP, LOW);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  serverConfigured.begin();

  timeClient.setTimeOffset(sysSetupStruc.ntpTimeZone * 3600);
  configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);


  // Вивести IP-адресу на дисплей
    
  #if defined(USE_PREDEFINED_SCREEN)
    vfd.writeString("                ", 0);
  #else
    vfd.writeStringUniverslaChrTab("                ", 0);
  #endif
    IPAddress ip = WiFi.localIP();
    char ipPart[7];
    
    // Перший октет: -192
    sprintf(ipPart, "-%d", ip[0]);
  #if defined(USE_PREDEFINED_SCREEN)
    vfd.writeString(ipPart, 0);
  #else
    vfd.writeStringUniverslaChrTab(ipPart, 0);
  #endif
    delay(500);
    
    // Другий октет: -168
    sprintf(ipPart, "-%d", ip[1]);
  #if defined(USE_PREDEFINED_SCREEN)
    vfd.writeString(ipPart, 0);
  #else
    vfd.writeStringUniverslaChrTab(ipPart, 0);
  #endif
    delay(500);
    
    // Третій октет: -001 (з нулями)
    sprintf(ipPart, "-%03d", ip[2]);
  #if defined(USE_PREDEFINED_SCREEN)
    vfd.writeString(ipPart, 0);
  #else
    vfd.writeStringUniverslaChrTab(ipPart, 0);
  #endif
    delay(500);
    
    // Четвертий октет: -099 (з нулями)
    sprintf(ipPart, "-%03d", ip[3]);
  #if defined(USE_PREDEFINED_SCREEN)
    vfd.writeString(ipPart, 0);
  #else
    vfd.writeStringUniverslaChrTab(ipPart, 0);
  #endif
    delay(500);

    

  


}
uint32_t screenUpdateTimer = 0;
uint32_t ntpUpdateTimer = 0;
uint32_t ntpRequestStartTime = 0;
bool ntpRequestInProgress = false;

void loop()
{
  // Перевірка watchdog для NTP - якщо запит триває більше 15 секунд, перезавантажити
  //if (ntpRequestInProgress && (millis() - ntpRequestStartTime > 15000)) {
  //  Serial.println("NTP request timeout! Restarting...");
  //  delay(1000);
  //  ESP.restart();
  //}
  // wdt reset
  esp_task_wdt_reset();
  
  if (millis() - screenUpdateTimer > 500)
  {
    screenUpdateTimer = millis();
    blinkingDot = !blinkingDot;
    
    
    
    

    time_t now;
  
    time(&now);
    localtime_r(&now, &timeinfo);

    char timeStr[10];
    sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    if (!screeenUpdateRestricted)
    {

      vfd.writeSpecialCharPlase(IV18_CHAR_DOT, blinkingDot);
      #if defined(USE_PREDEFINED_SCREEN)
        vfd.writeString(timeStr, 0);
      #else
        vfd.writeStringUniverslaChrTab(timeStr, 1);
      #endif
    }
  }

  if (sysSetupStruc.ntpEN)
  {
    if (millis() - ntpUpdateTimer > ntpUpdateInterval)
    {
      ntpUpdateTimer = millis();
      ntpUpdateInterval = NTP_UPDATE_INTERVAL;
      Serial.println("Updating time from NTP server...");

      ntpRequestInProgress = true;
      ntpRequestStartTime = millis();
      
      timeClient.update();
      
      ntpRequestInProgress = false;
      
      Serial.print("Current time (NTP): ");
      Serial.println(timeClient.getFormattedTime());
      if (!getLocalTime(&timeinfo))
      {
        Serial.println("Failed to obtain time");
      }
      else
      {
        Serial.println(&timeinfo, "Current time (RTC): %H:%M:%S");
      }
    }
  }

  WiFiClient client = serverConfigured.available(); // Очікуємо нових клієнтів
  if (client)
  {
    // Serial.println("New Client.");
    String currentLine = "";
    String header = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            if (header.indexOf("GET /settings") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:application/json");
              client.println("Connection: close");
              client.println();

              StaticJsonDocument<500> jsonDoc;
              jsonDoc["ntpServer"] = sysSetupStruc.ntpServerIndex;
              jsonDoc["demoConf"] = sysSetupStruc.screenDemoMode;
              jsonDoc["timezone"] = sysSetupStruc.ntpTimeZone;
              jsonDoc["manualTime"] = sysSetupStruc.ntpEN ? "on" : "off";
              jsonDoc["datetime"] = "";
              char colorBuffer[10];
              sprintf(colorBuffer, "#%02X%02X%02X", sysSetupStruc.ambLightColr[0], sysSetupStruc.ambLightColr[1], sysSetupStruc.ambLightColr[2]);
              jsonDoc["ambiLightColor"] = colorBuffer;
              jsonDoc["brightness"] = sysSetupStruc.ambLightBrightness;
              jsonDoc["ambiLight"] = sysSetupStruc.ambLightEnable ? "on" : "off";
              String jsonResponse;
              serializeJson(jsonDoc, jsonResponse);
              client.println(jsonResponse);
            }
            else if (header.indexOf("POST /submit") >= 0)
            {

              String requestBody = "";
              while (client.available())
              {
                char c = client.read();
                requestBody += c;
              }
              Serial.println(requestBody);
              StaticJsonDocument<500> jsonDoc;
              deserializeJson(jsonDoc, requestBody);
              if (jsonDoc.containsKey("demoConf"))
              {
                sysSetupStruc.screenDemoMode = jsonDoc["demoConf"];
              }
              if (jsonDoc.containsKey("ntpServer"))
              {
                sysSetupStruc.ntpServerIndex = jsonDoc["ntpServer"];
                timeClient.setTimeOffset(sysSetupStruc.ntpTimeZone * 3600);
                configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);
              }
              if (jsonDoc.containsKey("timezone"))
              {
                sysSetupStruc.ntpTimeZone = jsonDoc["timezone"];
                timeClient.setTimeOffset(sysSetupStruc.ntpTimeZone * 3600);
                configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);
              }
              if (jsonDoc.containsKey("manualTime"))
              {
                sysSetupStruc.ntpEN = (jsonDoc["manualTime"] == "on") ? 1 : 0;
              }
              if (jsonDoc.containsKey("ambiLightColor"))
              {
                String color = jsonDoc["ambiLightColor"];
                sysSetupStruc.ambLightColr[0] = strtol(color.substring(1, 3).c_str(), NULL, 16);
                sysSetupStruc.ambLightColr[1] = strtol(color.substring(3, 5).c_str(), NULL, 16);
                sysSetupStruc.ambLightColr[2] = strtol(color.substring(5, 7).c_str(), NULL, 16);
              }
              if (jsonDoc.containsKey("brightness"))
              {
                sysSetupStruc.ambLightBrightness = jsonDoc["brightness"];
              }
              if (jsonDoc.containsKey("ambiLight"))
              {
                sysSetupStruc.ambLightEnable = (jsonDoc["ambiLight"] == "on") ? 1 : 0;
              }
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              // sysSetupUpdate(sysSetupStruc);
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Settings updated");
            }
            else if(header.indexOf("GET /chargen") >= 0)
            {
              //send page charGen to client
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              
              // Відправити порціями по 1024 байта
              const char* ptr = charGen;
              size_t len = strlen(charGen);
              const size_t chunkSize = 1024;
              while (len > 0) {
                size_t toSend = (len > chunkSize) ? chunkSize : len;
                client.write((const uint8_t*)ptr, toSend);
                ptr += toSend;
                len -= toSend;
                delay(1); // Невелика затримка для стабільності
              }
            }
            //GET /styles.css
            else if(header.indexOf("GET /styles.css") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/css");
              client.println("Connection: close");
              client.println();
              
              // Відправити порціями
              const char* ptr = commonStyles;
              size_t len = strlen(commonStyles);
              const size_t chunkSize = 1024;
              while (len > 0) {
                size_t toSend = (len > chunkSize) ? chunkSize : len;
                client.write((const uint8_t*)ptr, toSend);
                ptr += toSend;
                len -= toSend;
                delay(1);
              }
            }
            //GET /commonRest.js
            else if(header.indexOf("GET /commonRest.js") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/javascript");
              client.println("Connection: close");
              client.println();
              
              // Відправити порціями
              const char* ptr = commonRest;
              size_t len = strlen(commonRest);
              const size_t chunkSize = 1024;
              while (len > 0) {
                size_t toSend = (len > chunkSize) ? chunkSize : len;
                client.write((const uint8_t*)ptr, toSend);
                ptr += toSend;
                len -= toSend;
                delay(1);
              }
            }
            
            else if(header.indexOf("GET /favicon.ico") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:image/x-icon");
              client.println("Connection: close");
              client.println("Content-Transfer-Encoding: binary");
              client.println();
              client.write(favicon_ico, sizeof(favicon_ico));
            }
            //GET /cmd=GET_DEVICE_INFO
            //Ansver: "192.168.1.99,80,8080,Sample Server,0,0,34,49,READY"
            else if(header.indexOf("GET /cmd=GET_DEVICE_INFO") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              String deviceInfo = String(WiFi.localIP().toString()) + "," +
                                  String(80) + "," +
                                  String(8080) + "," +
                                  "ESP32 VFD Clock," +
                                  String(0) + "," +
                                  String(0) + "," +
                                  String(sysSetupStruc.screenDemoMode) + "," +
                                  String(sysSetupStruc.ntpServerIndex) + "," +
                                  "READY";
              client.println(deviceInfo);
            }
            else if(header.indexOf("GET /cmd=CLOCK") >= 0){
              screeenUpdateRestricted = false;
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Switched to CLOCK mode");
            }
            //GET /cmd=RESTART
            else if (header.indexOf("GET /cmd=RESTART") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Restarting device...");
              delay(1000);
              ESP.restart();
            }
            //GET /cmd=TEXT 1234567890
            //display text "1234567890" on VFD
            else if (header.indexOf("GET /cmd=TEXT") >= 0)
            {
              screeenUpdateRestricted = true;
              int paramStart = header.indexOf("GET /cmd=TEXT") + strlen("GET /cmd=TEXT");
              String textToDisplay = header.substring(paramStart, header.indexOf(" ", paramStart));
              textToDisplay.trim();
              
              // Пропустити початковий пробіл або інші символи
              if (textToDisplay.startsWith("%20") || textToDisplay.startsWith(" ")) {
                textToDisplay = textToDisplay.substring(textToDisplay.startsWith("%20") ? 3 : 1);
              }
              
              textToDisplay = urlDecode(textToDisplay);
              
              Serial.print("Displaying text: ");
              Serial.println(textToDisplay);
              vfd.writeStringUniverslaChrTab(textToDisplay.c_str(), 0);
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Text displayed");
            }
            //GET /cmd=CHARSET,1,20,0000DF
            //1(0x01) - position (ignore this parameter)
            //20(0x20) - ascii code ()
            //0000DF(0x0000DF) - 3 bytes of used segments
            else if (header.indexOf("GET /cmd=CHARSET") >= 0)
            {
              screeenUpdateRestricted = true;
              int paramStart = header.indexOf("GET /cmd=CHARSET") + strlen("GET /cmd=CHARSET");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              params.trim();
              
              // Пропустити початкову кому
              if (params.startsWith(",")) {
                params = params.substring(1);
              }
              
              int firstComma = params.indexOf(",");
              int secondComma = params.indexOf(",", firstComma + 1);
              if (firstComma != -1 && secondComma != -1)
              {
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
                
                // Зберегти customCharData в EEPROM
                memcpy(sysSetupStruc.customCharData, customCharData, sizeof(customCharData));
                EEPROM.put(0, sysSetupStruc);
                EEPROM.commit();

                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Character set in custom table");
              }
              else
              {
                client.println("HTTP/1.1 400 Bad Request");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Invalid parameters");
              }
            }


            //return index page
            else if (header.indexOf("GET / ") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              
              // Відправити порціями
              const char* ptr = index_html;
              size_t len = strlen(index_html);
              const size_t chunkSize = 1024;
              while (len > 0) {
                size_t toSend = (len > chunkSize) ? chunkSize : len;
                client.write((const uint8_t*)ptr, toSend);
                ptr += toSend;
                len -= toSend;
                delay(1);
              }
            }
            //GET /cmd=CHARTEST,0,20,020000
            //0(0x00) - start position
            //20(0x20) - ascii code
            //020000(0x020000) - 3 bytes of used segments
            else if (header.indexOf("GET /cmd=CHARTEST") >= 0)
            {
              screeenUpdateRestricted = true;
              int paramStart = header.indexOf("GET /cmd=CHARTEST") + strlen("GET /cmd=CHARTEST");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              params.trim();
              
              // Пропустити початкову кому
              if (params.startsWith(",")) {
                params = params.substring(1);
              }
              
              int firstComma = params.indexOf(",");
              int secondComma = params.indexOf(",", firstComma + 1);
              if (firstComma != -1 && secondComma != -1)
              {
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

                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Character defined and displayed");
              }
              else
              {
                client.println("HTTP/1.1 400 Bad Request");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Invalid parameters");
              }
            }
            //return 404 not found
            //else{
            //  client.println("HTTP/1.1 404 Not Found");
            //  client.println("Content-type:text/plain");
            //  client.println("Connection: close");
            //  client.println();
            // client.println("404 Not Found");
            //}
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}