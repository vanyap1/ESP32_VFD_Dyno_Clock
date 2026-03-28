#include <Arduino.h>
#include <EEPROM.h>

// Platform-specific WiFi includes
#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif

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
  // WARNING: Adjust pins according to your hardware!
  // Hardware SPI pins (used by VFD): MOSI=D7, MISO=D6, SCK=D5 - DO NOT USE for other purposes!
  // Hardware I2C pins (used by RTC/sensors): SDA=D2, SCL=D1
  #define LED_HTTP D0      // GPIO16
  #define LED_WIFI D3      // GPIO0 (boot pin, has pull-up)
  #define INV_ENABLE D8    // GPIO15 (boot pin, has pull-down)
  #define LED_STRIP_PIN D4 // GPIO2 (boot pin, has pull-up, built-in LED)
  #define USR_BTN D3       // GPIO0 (shared with LED_WIFI, boot/flash button)
  #define I2C_SDA D2       // GPIO4 (hardware I2C SDA)
  #define I2C_SCL D1       // GPIO5 (hardware I2C SCL)
  #define VFD_DATA_PIN D0  // GPIO16 (SPI CS/latch pin, shared with LED_HTTP)
#endif

#define LED_STRIP_MAX_NUM_LEDS 16
#define VFD_EN

PT63XX vfd(VFD_DATA_PIN, IV18);
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
WiFiUDP *ntpUDP = nullptr;
NTPClient *timeClient = nullptr;

#define NTP_UPDATE_INTERVAL 216000000
#define NTP_FIRST_UPDATE_DELAY 5000

uint32_t ntpUpdateInterval = NTP_FIRST_UPDATE_DELAY;


const char *ntpServers[] = {"pool.ntp.org", "time.google.com", "time.windows.com"};

bool screeenUpdateRestricted = false;

float sensorTemp = 0.0;
int sensorPress = 0;
int sensorHum = 0;
float weatherTemp = 0.0;
float currencyEUR = 0.0;
float currencyUSD = 0.0;
float currencyBTC = 0.0;
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
  sysSetupStruc.screenDriver = PT6315_DIG12_SEG16; // Default to PT6315: 12 digits x 16 segments
  

  sysSetupStruc.sensorPressure = true;
  sysSetupStruc.sensorTemperature = true;
  sysSetupStruc.sensorAutoBrightness = false;
  sysSetupStruc.sensorWeatherApi = true;
  sysSetupStruc.sensorCurrency = true;
  strcpy(sysSetupStruc.myCurrency, "UAH");  // Default currency
  
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
  
  // Load EEPROM settings before initializing VFD
  EEPROM.begin(sizeof(sysSetupStruc) + 1);
  EEPROM.get(0, sysSetupStruc);
  
  // Initialize VFD with saved screen driver settings
  if (sysSetupStruc.FirstStart == 55) {
    vfd.begin(sysSetupStruc.screenDriver);
  } else {
    vfd.begin(); // Use default on first start
  }
  vfd.clearDisplay();
   
  delay(200);
  digitalWrite(INV_ENABLE, HIGH); 
  
  memcpy(customCharData, sysSetupStruc.customCharData, sizeof(customCharData));

  // Scan I2C bus for diagnostics
  scanI2C();

  // Initialize RTC if enabled in config
  // RTC hierarchy:
  // 1. Try to use external I2C RTC (RV8803) - more accurate, battery backup
  // 2. Fallback to ESP32 built-in RTC if I2C RTC not available
  // Both are synchronized with NTP when online
  Serial.print("Initializing RTC... ");
  rtcAvailable = rtc.begin(Wire);
  if (rtcAvailable) {
    Serial.println("OK (I2C RV8803)");
    // Load time from I2C RTC on startup
    rtc.updateSystemTime();
  } else {
    Serial.println("I2C RTC not found - using ESP32 built-in RTC");
    // ESP32 RTC will be used by default (time() calls)
    // Time will be set from NTP when online or manually via DATETIME command
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
  
  // Start LED animation in separate FreeRTOS task
  startLEDAnimationTask();
  
  // If first start, start AP mode immediately
  if (sysSetupStruc.FirstStart != 55) {
    startAPMode();
  }

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
  
  // Skip WiFi setup if already in offline mode (e.g., FirstStart)
  if (!offlineMode) {
    // Check if SSID is valid before attempting connection
    size_t ssidLen = strlen(sysSetupStruc.ssid);
    bool validSSID = (ssidLen > 0 && ssidLen <= 32);
    
    Serial.println("\n=== WiFi Setup Check ===");
    Serial.print("SSID length: ");
    Serial.println(ssidLen);
    Serial.print("SSID valid: ");
    Serial.println(validSSID ? "YES" : "NO");
    
    if (!validSSID) {
    Serial.println("========================\n");
    Serial.println("[ERROR] SSID is empty or invalid!");
    Serial.println("SSID must be 1-32 characters long.");
    Serial.println("Starting AP mode for configuration...");
    
    vfd.writeStringUniverslaChrTab("NO-SSID", 1);
    delay(1000);
    
    digitalWrite(LED_WIFI, LOW);
    digitalWrite(LED_HTTP, HIGH);
    offlineMode = true;
    
    setSystemState(STATE_AP_MODE);
    startAPMode();
    
    // If RTC is available, use it for time in offline mode
    // If I2C RTC not available, ESP32 built-in RTC is used automatically
    if (rtcAvailable) {
      rtc.updateSystemTime();
      Serial.println("Time loaded from I2C RTC");
    } else {
      Serial.println("Using ESP32 built-in RTC (set time manually via web interface)");
    }
  } else {
    Serial.println("========================\n");
    Serial.print("Attempting to connect to: ");
    Serial.println(sysSetupStruc.ssid);
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
      // If I2C RTC not available, ESP32 built-in RTC is used automatically
      if (rtcAvailable) {
        rtc.updateSystemTime();
        Serial.println("Time loaded from I2C RTC");
      } else {
        Serial.println("Using ESP32 built-in RTC (set time manually via web interface)");
      }
    }
    else
    {
      Serial.println("\nWiFi connected.");
      offlineMode = false;
      
      // Show connection success animation for 1.5 seconds
      setSystemState(STATE_WIFI_CONNECTED);
      delay(1500);
    }
  }
  }  // End of if (!offlineMode)
  
  digitalWrite(LED_WIFI, LOW);
  digitalWrite(LED_HTTP, LOW);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  serverConfigured.begin();

  // Configure NTP only if not in offline mode
  if (!offlineMode && sysSetupStruc.ntpEN) {
    // Create UDP and NTP client dynamically
    ntpUDP = new WiFiUDP();
    ntpUDP->begin(123);  // NTP uses port 123
    
    // NTPClient with 0 offset - we'll use configTime for timezone
    timeClient = new NTPClient(*ntpUDP, ntpServers[sysSetupStruc.ntpServerIndex], 
                                0, NTP_UPDATE_INTERVAL);
    timeClient->begin();
    
    // Configure ESP32 built-in NTP with timezone
    configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);
    Serial.print("NTP configured with server: ");
    Serial.println(ntpServers[sysSetupStruc.ntpServerIndex]);
    Serial.print("Timezone offset: ");
    Serial.print(sysSetupStruc.ntpTimeZone);
    Serial.println(" hours");
    
    // Perform initial NTP sync using NTPClient
    Serial.println("Performing initial NTP sync...");
    int retries = 0;
    while (!timeClient->update() && retries < 5) {
      delay(1000);
      retries++;
      Serial.print(".");
    }
    
    if (retries < 5) {
      Serial.println("\nInitial NTP sync successful!");
      
      // Get UTC time from NTPClient and set system time
      unsigned long epochTime = timeClient->getEpochTime();
      struct timeval tv = { .tv_sec = (time_t)epochTime, .tv_usec = 0 };
      settimeofday(&tv, NULL);
      
      // Wait a bit for time to stabilize
      delay(100);
      
      // Print current time with timezone
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        #ifdef ESP32
          Serial.println(&timeinfo, "System time with timezone: %H:%M:%S %d.%m.%Y");
        #else
          char timeStr[50];
          strftime(timeStr, sizeof(timeStr), "System time with timezone: %H:%M:%S %d.%m.%Y", &timeinfo);
          Serial.println(timeStr);
        #endif
        
        // Update I2C RTC with local time (if available)
        // ESP32 built-in RTC is already set by settimeofday()
        if (rtcAvailable) {
          rtc.setFromSystemTime();
          Serial.println("I2C RTC initialized with NTP time");
        } else {
          Serial.println("Using ESP32 built-in RTC (time synchronized with NTP)");
        }
      } else {
        Serial.println("Failed to get local time!");
        setSystemState(STATE_ERROR);
        delay(2000);  // Show error indication for 2 seconds
        setSystemState(STATE_NORMAL);
      }
    } else {
      Serial.println("\nInitial NTP sync failed! Will retry in loop.");
      setSystemState(STATE_ERROR);
      delay(2000);  // Show error indication for 2 seconds
      setSystemState(STATE_NORMAL);
    }
  } else {
    Serial.println("NTP disabled (offline mode)");
  }

  // Display IP address on VFD
  vfd.writeStringUniverslaChrTab("                ", 0);
  if (!offlineMode) {
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
  } else {
    vfd.writeStringUniverslaChrTab("OFFLINE", 0);
    delay(2000);
  }
  vfd.setBlinkCharData(sysSetupStruc.blinkMask, sysSetupStruc.blinkPosition);
  
  // Always set to normal mode at the end of setup
  // This ensures user LED effects are active regardless of online/offline mode
  setSystemState(STATE_NORMAL);
  Serial.println("Setup complete - LED effects switched to user configuration");
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

// Sync time with I2C RTC (if available)
void syncRTCTime() {
  static uint32_t lastRTCSync = 0;
  
  // Sync with I2C RTC every hour if available and NTP is enabled
  if (rtcAvailable && sysSetupStruc.ntpEN) {
    if (millis() - lastRTCSync > 3600000) {  // 1 hour
      lastRTCSync = millis();
      
      // If we're online, update I2C RTC from system time (which gets updated via NTP)
      // ESP32 built-in RTC is automatically updated by settimeofday() during NTP sync
      if (!offlineMode) {
        rtc.setFromSystemTime();
        Serial.println("I2C RTC updated from system time");
      } else {
        // If offline, update system time from I2C RTC (more accurate with battery backup)
        // This also updates ESP32 built-in RTC via updateSystemTime()
        rtc.updateSystemTime();
        Serial.println("System time updated from I2C RTC");
      }
    }
  }
}

void loop()
{
  esp_task_wdt_reset();
  
  // For ESP8266, update LED effects in main loop (no FreeRTOS task)
  #ifdef ESP8266
    static uint32_t lastLedUpdate = 0;
    if (millis() - lastLedUpdate >= 20) {  // 50Hz update rate
      updateLEDEffect();
      lastLedUpdate = millis();
    }
  #endif
  
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
        
          vfd.blinkState(sysSetupStruc.displayFormatBlink[currentScreenIndex] ? blinkingDot : 0, sysSetupStruc.displayFormatBlink[currentScreenIndex]);
        
        vfd.writeStringUniverslaChrTab(displayText.c_str(), 0);
      }
    }
  }

  // Only update NTP if online and NTP client is initialized
  if (sysSetupStruc.ntpEN && !offlineMode && timeClient != nullptr)
  {
    if (millis() - ntpUpdateTimer > ntpUpdateInterval)
    {
      ntpUpdateTimer = millis();
      ntpUpdateInterval = NTP_UPDATE_INTERVAL;
      Serial.println("Updating time from NTP server...");

      ntpRequestInProgress = true;
      ntpRequestStartTime = millis();
      
      bool ntpSuccess = timeClient->update();
      
      ntpRequestInProgress = false;
      
      if (ntpSuccess) {
        Serial.println("NTP update successful");
        
        // Get UTC time from NTPClient and set system time
        unsigned long epochTime = timeClient->getEpochTime();
        struct timeval tv = { .tv_sec = (time_t)epochTime, .tv_usec = 0 };
        settimeofday(&tv, NULL);
        
        delay(100);  // Wait for time to stabilize
        
        // Print formatted time
        if (getLocalTime(&timeinfo)) {
          #ifdef ESP32
            Serial.println(&timeinfo, "System time updated: %H:%M:%S %d.%m.%Y");
          #else
            char timeStr[50];
            strftime(timeStr, sizeof(timeStr), "System time updated: %H:%M:%S %d.%m.%Y", &timeinfo);
            Serial.println(timeStr);
          #endif
          
          // Update I2C RTC from system time after NTP sync (if available)
          // ESP32 built-in RTC is updated automatically by settimeofday()
          if (rtcAvailable) {
            rtc.setFromSystemTime();
            Serial.println("I2C RTC synced with NTP time");
          }
        } else {
          Serial.println("Failed to obtain local time after NTP update");
          setSystemState(STATE_ERROR);
          delay(2000);  // Show error indication for 2 seconds
          setSystemState(STATE_NORMAL);
        }
      } else {
        Serial.println("NTP update failed!");
        setSystemState(STATE_ERROR);
        delay(2000);  // Show failure indication for 2 seconds
        setSystemState(STATE_NORMAL);
      }
    }
  }

  // Update sensors
  updateSensors();
  
  // Sync RTC time
  syncRTCTime();

  // Handle AP mode DNS requests if active (for captive portal)
  if (isAPModeActive()) {
    getDNSServer().processNextRequest();
    yield();  // Allow WiFi stack to process
  }

  WiFiClient client = serverConfigured.available();
  if (client)
  {
    if (offlineMode) {
      Serial.println("New AP client connected");
    }
    
    // Longer timeout for AP mode to handle slower mobile devices
    uint32_t timeout = offlineMode ? 10000 : 30000;
    client.setTimeout(timeout);
    
    String currentLine = "";
    String header = "";
    header.reserve(512);  // Pre-allocate memory to avoid fragmentation
    uint32_t startTime = millis();
    
    while (client.connected())
    {
      // Timeout check
      if (millis() - startTime > timeout) {
        Serial.println("Client timeout");
        break;
      }
      
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
    
    // Ensure all data is sent before closing
    client.flush();
    delay(1);  // Give WiFi stack time to finish transmission
    client.stop();
    
    if (offlineMode) {
      delay(10);  // Small delay in AP mode for stability
    }
  }
}
