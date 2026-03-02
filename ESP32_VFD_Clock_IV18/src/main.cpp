#include <Arduino.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include "dto.h"
#include "pt63xx.h"
#include <sys/time.h>
#include "WebRoot/html_page.h"
#include "WebRoot/scripts.h"
#include "WebRoot/styles.h"
#include "WebRoot/images.h"
#include <time.h>

// Platform-specific includes
#ifdef ESP32
  #include <esp_task_wdt.h>
  #include <Update.h>
#elif defined(ESP8266)
  #include <Updater.h>
  // ESP8266 використовує стандартне годування watchdog
  #define esp_task_wdt_reset() ESP.wdtFeed()
#endif

#include "FastLED.h"
#include <Wire.h>
#include "RV8803.h"
#include "HDC2010.h"

// Project-specific modules
#include "led_effects.h"
#include "display_utils.h"
#include "network_utils.h"
#include "command_processor.h"

// Platform-specific pin definitions
#ifdef ESP32
  #define LED_HTTP 16
  #define LED_WIFI 17
  #define INV_ENABLE 21
  #define LED_STRIP_PIN 33
  #define USR_BTN 4
  #define I2C_SDA 23
  #define I2C_SCL 22
  #define VFD_DATA_PIN 27
#elif defined(ESP8266)
  #define LED_HTTP D5      // GPIO14
  #define LED_WIFI D6      // GPIO12
  #define INV_ENABLE D7    // GPIO13
  #define LED_STRIP_PIN D8 // GPIO15
  #define USR_BTN D3       // GPIO0
  #define I2C_SDA D2       // GPIO4 (default SDA)
  #define I2C_SCL D1       // GPIO5 (default SCL)
  #define VFD_DATA_PIN D4  // GPIO2
#endif

#define LED_STRIP_MAX_NUM_LEDS 16
#define VFD_EN

PT63XX vfd(27, IV18);
CRGB leds[LED_STRIP_MAX_NUM_LEDS];

RV8803 rtc;
HDC2010 tempHumiditySensor;
bool rtcAvailable = false;
bool tempSensorAvailable = false;

struct tm timeinfo;
bool blinkingDot = false;
SystemSetup sysSetupStruc;

bool offlineMode = false;

WiFiServer serverConfigured(80);
WiFiUDP ntpUDP;

#define NTP_UPDATE_INTERVAL 216000000
#define NTP_FIRST_UPDATE_DELAY 5000

uint32_t ntpUpdateInterval = NTP_FIRST_UPDATE_DELAY;


const char *ntpServers[] = {"pool.ntp.org", "time.google.com", "time.windows.com"};
NTPClient timeClient(ntpUDP, ntpServers[0]);

bool screeenUpdateRestricted = false;

float sensorTemp = 23.5;
int sensorPress = 1013;
int sensorHum = 65;
float weatherTemp = 18.0;
float currencyEUR = 43.5;
float currencyUSD = 40.2;
float currencyBTC = 98.3;
char weatherCond[16] = "Cloudy";

int currentScreenIndex = 0;
uint32_t screenSwitchTimer = 0;

void updateSensors();
void syncRTCTime();

void initDefaultConfig()
{
  memset(&sysSetupStruc.ssid, 0, sizeof(sysSetupStruc.ssid));
  memset(&sysSetupStruc.pass, 0, sizeof(sysSetupStruc.pass));

  sysSetupStruc.ntpEN = 1; 
  sysSetupStruc.ntpServerIndex = 0;
  sysSetupStruc.ntpTimeZone = 0;

  strcpy(sysSetupStruc.displayFormat[0], "h *HH* *MM*");
  sysSetupStruc.displayFormatTime[0] = 3;
  sysSetupStruc.displayFormatEnable[0] = true;
  sysSetupStruc.displayFormatBlink[0] = true;
  
  strcpy(sysSetupStruc.displayFormat[1], "t *TEMP* ^C");
  sysSetupStruc.displayFormatTime[1] = 2;
  sysSetupStruc.displayFormatEnable[1] = true;
  sysSetupStruc.displayFormatBlink[1] = false;
  
  strcpy(sysSetupStruc.displayFormat[2], "p *HUM* ^i");
  sysSetupStruc.displayFormatTime[2] = 2;
  sysSetupStruc.displayFormatEnable[2] = true;
  sysSetupStruc.displayFormatBlink[2] = false;
  

  sysSetupStruc.blinkMask = 0x80;
  sysSetupStruc.blinkPosition = 4;
  

  sysSetupStruc.sensorPressure = true;
  sysSetupStruc.sensorTemperature = true;
  sysSetupStruc.sensorAutoBrightness = false;
  sysSetupStruc.sensorWeatherApi = true;
  sysSetupStruc.sensorCurrency = true;
  
  sysSetupStruc.displayBrightness = 1;
  sysSetupStruc.ambLightColr[0] = 22;  // r
  sysSetupStruc.ambLightColr[1] = 254; // g
  sysSetupStruc.ambLightColr[2] = 80;  // b
  sysSetupStruc.ambLightBrightness = 255;
  sysSetupStruc.ledCount = 4;
  sysSetupStruc.ledEffect = 0;  // Default to Solid Color
  
  sysSetupStruc.FirstStart = 55;
  
  EEPROM.put(0, sysSetupStruc);
  EEPROM.commit();
}

void setup()
{
  pinMode(LED_HTTP, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(INV_ENABLE, OUTPUT);
  digitalWrite(INV_ENABLE, LOW); // Enable inverter power
  pinMode(USR_BTN, INPUT_PULLUP);
  pinMode(LED_STRIP_PIN, OUTPUT);
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000); // 100kHz for stability

  sysSetupStruc.ntpServerIndex = 0;
  sysSetupStruc.ntpTimeZone = 0;
  
  Serial.begin(115200);

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
  digitalWrite(INV_ENABLE, HIGH); 
  EEPROM.begin(sizeof(sysSetupStruc) + 1);
  EEPROM.get(0, sysSetupStruc);
  
  memcpy(customCharData, sysSetupStruc.customCharData, sizeof(customCharData));

  // Scan I2C bus for diagnostics
  scanI2C();

  // Initialize RTC if enabled in config
  Serial.print("Initializing RTC... ");
  rtcAvailable = rtc.begin(Wire);
  if (rtcAvailable) {
    Serial.println("OK");
    // Load time from RTC on startup
    rtc.updateSystemTime();
  } else {
    Serial.println("Not found");
  }
  
  // Initialize temperature/humidity sensor if enabled in config
  if (sysSetupStruc.sensorTemperature) {
    Serial.print("Initializing HDC2010... ");
    tempSensorAvailable = tempHumiditySensor.begin(Wire);
    if (tempSensorAvailable) {
      Serial.println("OK");
    } else {
      Serial.println("Not found");
    }
  }

  if (digitalRead(USR_BTN) == LOW)
  {
    sysSetupStruc.FirstStart = 1;
  }

  if (sysSetupStruc.FirstStart != 55)
  {
    Serial.println("First start detected. Initializing default configuration...");
    initDefaultConfig();
    digitalWrite(LED_HTTP, HIGH);
    vfd.writeStringUniverslaChrTab("Conf", 1);
    offlineMode = true;
    setSystemState(STATE_AP_MODE);
  }
  FastLED.addLeds<WS2811, LED_STRIP_PIN, GRB>(leds, sysSetupStruc.ledCount).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(sysSetupStruc.ambLightBrightness);
  
  // Initialize LED effects
  initLEDEffects(leds, sysSetupStruc.ledCount);

  updateLEDEffect();
  vfd.writeStringUniverslaChrTab("HELLO", 1);
  Serial.println("System Setup Data:");
  Serial.print("FirstStart: ");
  Serial.println(sysSetupStruc.FirstStart);
  Serial.print("SSID: ");
  Serial.println(sysSetupStruc.ssid);
  Serial.print("SSID LENGTH:  ");
  Serial.println(strlen(sysSetupStruc.ssid));
  Serial.print("PASS: ");
  Serial.println(sysSetupStruc.pass);
  Serial.print("PASS LENGTH:  ");
  Serial.println(strlen(sysSetupStruc.pass));
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

  // Set LED state to connecting
  setSystemState(STATE_WIFI_CONNECTING);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(sysSetupStruc.ssid, sysSetupStruc.pass);

  int attempts = 0;
  const int maxAttempts = 20;

  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts)
  {
    updateLEDEffect(); // Update LED animation during connection
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("\nWiFi connection failed! Starting AP mode...");
    digitalWrite(LED_WIFI, LOW);
    digitalWrite(LED_HTTP, HIGH);
    offlineMode = true;
    setSystemState(STATE_AP_MODE);
    startAPMode();
    
    // If RTC is available, use it for time in offline mode
    if (rtcAvailable) {
      rtc.updateSystemTime();
      Serial.println("Time loaded from RTC");
    }
  }
  else
  {
    Serial.println("\nWiFi connected.");
    offlineMode = false;
    setSystemState(STATE_NORMAL); // Switch to normal operation
  }
  digitalWrite(LED_WIFI, LOW);
  digitalWrite(LED_HTTP, LOW);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  serverConfigured.begin();

  timeClient.setTimeOffset(sysSetupStruc.ntpTimeZone * 3600);
  configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);


    
  
  vfd.writeStringUniverslaChrTab("                ", 0);
  IPAddress ip = WiFi.localIP();
  char ipPart[7];
  sprintf(ipPart, "-%d", ip[0]);
  vfd.writeStringUniverslaChrTab(ipPart, 0);
  delay(500);
  sprintf(ipPart, "-%d", ip[1]);
  vfd.writeStringUniverslaChrTab(ipPart, 0);
  delay(500);
  sprintf(ipPart, "-%03d", ip[2]);
  vfd.writeStringUniverslaChrTab(ipPart, 0);
  delay(500);
  sprintf(ipPart, "-%03d", ip[3]);
  vfd.writeStringUniverslaChrTab(ipPart, 0);
  delay(500);
  vfd.setBlinkCharData(sysSetupStruc.blinkMask, sysSetupStruc.blinkPosition);
    

  


}
uint32_t screenUpdateTimer = 0;
uint32_t ntpUpdateTimer = 0;
uint32_t ntpRequestStartTime = 0;
bool ntpRequestInProgress = false;

// Update sensor readings
void updateSensors() {
  static uint32_t lastSensorUpdate = 0;
  
  // Update sensors every 10 seconds
  if (millis() - lastSensorUpdate < 10000) {
    return;
  }
  lastSensorUpdate = millis();
  
  // Read temperature and humidity if sensor is available and enabled
  if (tempSensorAvailable && sysSetupStruc.sensorTemperature) {
    float temp, hum;
    if (tempHumiditySensor.read(temp, hum)) {
      sensorTemp = temp;
      sensorHum = (int)hum;
      
      Serial.print("Sensor: Temp=");
      Serial.print(sensorTemp);
      Serial.print("°C, Humidity=");
      Serial.print(sensorHum);
      Serial.println("%");
    }
  }
}

// Sync time with RTC
void syncRTCTime() {
  static uint32_t lastRTCSync = 0;
  
  // Sync with RTC every hour if available and NTP is enabled
  if (rtcAvailable && sysSetupStruc.ntpEN) {
    if (millis() - lastRTCSync > 3600000) {  // 1 hour
      lastRTCSync = millis();
      
      // If we're online, update RTC from system time (which gets updated via NTP)
      if (!offlineMode) {
        rtc.setFromSystemTime();
        Serial.println("RTC updated from system time");
      } else {
        // If offline, update system time from RTC
        rtc.updateSystemTime();
        Serial.println("System time updated from RTC");
      }
    }
  }
}

void loop()
{
  esp_task_wdt_reset();
  
  if (millis() - screenUpdateTimer > 500)
  {
    screenUpdateTimer = millis();
    blinkingDot = !blinkingDot;
    
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Auto-brightness control based on time
    if (sysSetupStruc.sensorAutoBrightness) {
      int hour = timeinfo.tm_hour;
      // Night mode: 21:00 - 7:00 = brightness 1, otherwise = 7
      uint8_t autoBrightness = (hour >= 21 || hour < 7) ? 1 : 7;
      vfd.setBrightness(autoBrightness);
    } else {
      // Use manual brightness setting
      vfd.setBrightness(sysSetupStruc.displayBrightness);
    }

    if (!screeenUpdateRestricted)
    {
      if (millis() - screenSwitchTimer > (sysSetupStruc.displayFormatTime[currentScreenIndex] * 1000)) {
        screenSwitchTimer = millis();
        
        int startIndex = currentScreenIndex;
        do {
          currentScreenIndex = (currentScreenIndex + 1) % 3;
        } while (!sysSetupStruc.displayFormatEnable[currentScreenIndex] && currentScreenIndex != startIndex);
      }
      
      if (sysSetupStruc.displayFormatEnable[currentScreenIndex] && strlen(sysSetupStruc.displayFormat[currentScreenIndex]) > 0) {
        String displayText = parseDisplayFormat(
          String(sysSetupStruc.displayFormat[currentScreenIndex]), 
          timeinfo, 
          sensorTemp, sensorPress, sensorHum,
          weatherTemp, weatherCond,
          currencyEUR, currencyUSD, currencyBTC
        );
        
        vfd.blinkState(sysSetupStruc.displayFormatBlink[currentScreenIndex] ? blinkingDot : 0);
        vfd.writeStringUniverslaChrTab(displayText.c_str(), 0);
      }
    }
  }

  // Only update NTP if online
  if (sysSetupStruc.ntpEN && !offlineMode)
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
      
      // Update RTC from system time after NTP sync
      if (rtcAvailable) {
        rtc.setFromSystemTime();
        Serial.println("RTC synced with NTP time");
      }
      
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

  // Update LED effects
  updateLEDEffect();

  // Update sensors
  updateSensors();
  
  // Sync RTC time
  syncRTCTime();

  // Handle AP mode DNS requests if active (for captive portal)
  if (isAPModeActive()) {
    getDNSServer().processNextRequest();
  }

  WiFiClient client = serverConfigured.available();
  if (client)
  {
    client.setTimeout(30000); // Set 30 second timeout for large firmware uploads
    String currentLine = "";
    String header = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        header += c;

        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            if (header.indexOf("GET /") >= 0){
              int endOfFirstLine = header.indexOf('\r');
              if (endOfFirstLine == -1) endOfFirstLine = header.indexOf('\n');
              String requestLine = header.substring(0, endOfFirstLine);
              Serial.println(requestLine);
            }
            
            // Process HTTP command
            processHTTPCommand(client, header, vfd, rtc, tempHumiditySensor, rtcAvailable, screeenUpdateRestricted);
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
