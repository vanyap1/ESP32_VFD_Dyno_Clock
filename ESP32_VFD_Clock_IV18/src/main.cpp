#include <Arduino.h>
#include <EEPROM.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include "dto.h"
#include "pt63xx.h"
#include <sys/time.h>
#include "html_page.h"
#include "setup.h"
#include <time.h>

#define LED_HTTP 16
#define LED_WIFI 17
#define INV_ENABLE 21

#define USR_BTN 4

#define VFD_EN

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

void setup()
{
  pinMode(LED_HTTP, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(INV_ENABLE, OUTPUT);
  digitalWrite(INV_ENABLE, HIGH); // Enable inverter power
  pinMode(USR_BTN, INPUT_PULLUP);

  sysSetupStruc.ntpServerIndex = 0;
  sysSetupStruc.ntpTimeZone = 0;
  
  Serial.begin(115200);
  Serial.println("Hello, Arduino!");

  vfd.begin();
  vfd.clearDisplay();

  EEPROM.begin(sizeof(sysSetupStruc) + 1);
  EEPROM.get(0, sysSetupStruc);

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
    ClientSetup();
  }

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


}
uint32_t screenUpdateTimer = 0;
uint32_t ntpUpdateTimer = 0;
void loop()
{
  if (millis() - screenUpdateTimer > 500)
  {
    screenUpdateTimer = millis();
    blinkingDot = !blinkingDot;
    vfd.writeSpecialCharPlase(IV18_CHAR_DOT, blinkingDot);

    time_t now;
  
    time(&now);
    localtime_r(&now, &timeinfo);

    char timeStr[10];
    sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    vfd.writeString(timeStr, 0);
  }

  if (sysSetupStruc.ntpEN)
  {
    if (millis() - ntpUpdateTimer > ntpUpdateInterval)
    {
      ntpUpdateTimer = millis();
      ntpUpdateInterval = NTP_UPDATE_INTERVAL;
      Serial.println("Updating time from NTP server...");

      timeClient.update();
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
        // Serial.write(c);
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
            else
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              client.println(index_html);
            }
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