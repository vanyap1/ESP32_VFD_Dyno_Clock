#include <Arduino.h>
#include <EEPROM.h>

// Platform-specific WiFi includes
#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WiFiClientSecure.h>
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecureBearSSL.h>
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
#include <math.h>

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
  // #define LED_HTTP D0      // GPIO16 - NOT USED (no hardware connection)
  // #define LED_WIFI D3      // GPIO0 (boot pin, has pull-up) - NOT USED
  // #define INV_ENABLE D8    // GPIO15 (boot pin, has pull-down) - NOT USED
  #define LED_STRIP_PIN D4 // GPIO2 (boot pin, has pull-up, built-in LED)
  // #define USR_BTN D3       // GPIO0 (shared with LED_WIFI, boot/flash button) - NOT USED
  #define I2C_SDA D2       // GPIO4 (hardware I2C SDA)
  #define I2C_SCL D1       // GPIO5 (hardware I2C SCL)
  #define VFD_DATA_PIN D8  // GPIO16 (SPI CS/latch pin)
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

float sensorTemp = NAN;
int sensorPress = -1;
int sensorHum = -1;
float weatherTemp = NAN;
float currencyEUR = NAN;
float currencyUSD = NAN;
float currencyBTC = NAN;
char weatherCond[16] = "--";

int currentScreenIndex = 0;
uint32_t screenSwitchTimer = 0;

void updateSensors();
void syncRTCTime();

static const uint32_t LOCAL_SENSOR_UPDATE_MS = 10000;
static const uint32_t WEATHER_UPDATE_MS = 15UL * 60UL * 1000UL;
static const uint32_t CURRENCY_UPDATE_MS = 30UL * 60UL * 1000UL;

bool isInternetAvailable()
{
  return !offlineMode && WiFi.status() == WL_CONNECTED;
}

String getConfiguredCurrencyCode()
{
  String currency = sysSetupStruc.myCurrency;
  currency.trim();
  currency.toUpperCase();

  if (currency.length() != 3) {
    currency = "UAH";
  }

  return currency;
}

float extractFloatValue(const String& source)
{
  String numeric = "";
  bool started = false;
  bool hasDecimalPoint = false;

  for (size_t index = 0; index < source.length(); index++) {
    char current = source.charAt(index);

    if (!started && (current == '+' || current == '-')) {
      numeric += current;
      started = true;
      continue;
    }

    if (current >= '0' && current <= '9') {
      numeric += current;
      started = true;
      continue;
    }

    if (started && current == '.' && !hasDecimalPoint) {
      numeric += current;
      hasDecimalPoint = true;
      continue;
    }

    if (started) {
      break;
    }
  }

  if (numeric.length() == 0 || numeric == "+" || numeric == "-") {
    return NAN;
  }

  return numeric.toFloat();
}

String normalizeWeatherCondition(const String& source)
{
  String condition = source;
  condition.trim();
  condition.toUpperCase();

  condition.replace("PATCHY RAIN NEARBY", "RAIN NEAR");
  condition.replace("PARTLY CLOUDY", "P CLOUDY");
  condition.replace("LIGHT DRIZZLE", "DRIZZLE");
  condition.replace("LIGHT RAIN", "L RAIN");
  condition.replace("MODERATE RAIN", "M RAIN");
  condition.replace("HEAVY RAIN", "H RAIN");
  condition.replace("THUNDERY OUTBREAKS IN NEARBY", "THUNDER");
  condition.replace("OVERCAST", "OVRCAST");

  if (condition.length() == 0) {
    condition = "--";
  }

  if (condition.length() >= sizeof(weatherCond)) {
    condition = condition.substring(0, sizeof(weatherCond) - 1);
  }

  return condition;
}

bool httpGetText(const String& url, String& response)
{
  HTTPClient http;
  http.setTimeout(5000);

#ifdef ESP32
  WiFiClientSecure client;
  client.setInsecure();
  if (!http.begin(client, url)) {
    Serial.print("HTTP begin failed: ");
    Serial.println(url);
    return false;
  }
#else
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  if (!http.begin(client, url)) {
    Serial.print("HTTP begin failed: ");
    Serial.println(url);
    return false;
  }
#endif

  int httpCode = http.GET();
  if (httpCode <= 0) {
    Serial.print("HTTP GET failed for ");
    Serial.print(url);
    Serial.print(" code=");
    Serial.println(httpCode);
    http.end();
    return false;
  }

  if (httpCode != HTTP_CODE_OK) {
    Serial.print("Unexpected HTTP code for ");
    Serial.print(url);
    Serial.print(": ");
    Serial.println(httpCode);
    http.end();
    return false;
  }

  response = http.getString();
  http.end();
  return true;
}

bool fetchWeatherData()
{
  String payload;
  if (!httpGetText("https://wttr.in/?format=%25t|%25C", payload)) {
    return false;
  }

  payload.trim();
  int separatorIndex = payload.indexOf('|');
  if (separatorIndex < 0) {
    Serial.println("Weather payload parse error: missing separator");
    return false;
  }

  float parsedTemperature = extractFloatValue(payload.substring(0, separatorIndex));
  if (isnan(parsedTemperature)) {
    Serial.println("Weather payload parse error: invalid temperature");
    return false;
  }

  String parsedCondition = normalizeWeatherCondition(payload.substring(separatorIndex + 1));

  weatherTemp = parsedTemperature;
  parsedCondition.toCharArray(weatherCond, sizeof(weatherCond));

  Serial.print("Weather updated: ");
  Serial.print(weatherTemp, 1);
  Serial.print("C, ");
  Serial.println(weatherCond);

  return true;
}

bool fetchBitcoinRate(const String& currencyCode, float usdToLocalRate)
{
  String payload;
  String url = "https://api.coinbase.com/v2/prices/BTC-" + currencyCode + "/spot";

  if (!httpGetText(url, payload) && currencyCode != "USD") {
    if (!httpGetText("https://api.coinbase.com/v2/prices/BTC-USD/spot", payload)) {
      return false;
    }

    DynamicJsonDocument fallbackDoc(512);
    if (deserializeJson(fallbackDoc, payload) != DeserializationError::Ok) {
      return false;
    }

    float btcUsd = String((const char*)(fallbackDoc["data"]["amount"] | "")).toFloat();
    if (btcUsd <= 0.0f || isnan(btcUsd)) {
      return false;
    }

    currencyBTC = btcUsd * usdToLocalRate;
    return true;
  }

  DynamicJsonDocument doc(512);
  if (deserializeJson(doc, payload) != DeserializationError::Ok) {
    return false;
  }

  float amount = String((const char*)(doc["data"]["amount"] | "")).toFloat();
  if (amount <= 0.0f || isnan(amount)) {
    return false;
  }

  currencyBTC = amount;
  return true;
}

bool fetchCurrencyRates()
{
  String currencyCode = getConfiguredCurrencyCode();
  String payload;
  if (!httpGetText("https://open.er-api.com/v6/latest/USD", payload)) {
    return false;
  }

  DynamicJsonDocument filter(256);
  filter["result"] = true;
  JsonObject ratesFilter = filter.createNestedObject("rates");
  ratesFilter["EUR"] = true;
  ratesFilter["USD"] = true;
  ratesFilter[currencyCode] = true;

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
  if (error) {
    Serial.print("Currency JSON parse failed: ");
    Serial.println(error.c_str());
    return false;
  }

  const char* result = doc["result"] | "error";
  if (strcmp(result, "success") != 0) {
    Serial.println("Currency API returned non-success status");
    return false;
  }

  float usdToLocal = doc["rates"][currencyCode].as<float>();
  float usdToEur = doc["rates"]["EUR"].as<float>();
  if (usdToLocal <= 0.0f || usdToEur <= 0.0f) {
    Serial.println("Currency API returned invalid rates");
    return false;
  }

  currencyUSD = usdToLocal;
  currencyEUR = usdToLocal / usdToEur;

  if (!fetchBitcoinRate(currencyCode, usdToLocal)) {
    Serial.println("BTC rate update failed, keeping previous BTC value");
  }

  Serial.print("Currency updated in ");
  Serial.print(currencyCode);
  Serial.print(": EUR=");
  Serial.print(currencyEUR, 2);
  Serial.print(", USD=");
  Serial.print(currencyUSD, 2);
  Serial.print(", BTC=");
  Serial.println(currencyBTC, 2);

  return true;
}

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
  sysSetupStruc.screenDirection = false;  // Normal direction by default
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
  #ifdef ESP32
    pinMode(LED_HTTP, OUTPUT);
    pinMode(LED_WIFI, OUTPUT);
    pinMode(INV_ENABLE, OUTPUT);
    digitalWrite(INV_ENABLE, LOW); // Enable inverter power
    pinMode(USR_BTN, INPUT_PULLUP);
  #endif
  pinMode(LED_STRIP_PIN, OUTPUT);
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000); // 100kHz for stability

  sysSetupStruc.ntpServerIndex = 0;
  sysSetupStruc.ntpTimeZone = 0;
  
  Serial.begin(115200);
  delay(100);  // Wait for serial to stabilize
  
  Serial.println("\n\n=== VFD Clock Starting ===");
  Serial.print("SystemSetup structure size: ");
  Serial.print(sizeof(sysSetupStruc));
  Serial.println(" bytes");

  #ifdef ESP32
    for (int i = 0; i < 2; i++)
    {
      digitalWrite(LED_WIFI, HIGH);
      delay(200);
      digitalWrite(LED_WIFI, LOW);
      delay(200);
    }
  #endif
  
  // Load EEPROM settings before initializing VFD
  // ESP8266 needs larger EEPROM size
  #ifdef ESP8266
    EEPROM.begin(4096);  // Use maximum EEPROM size for ESP8266
  #else
    EEPROM.begin(sizeof(sysSetupStruc) + 1);
  #endif
  
  EEPROM.get(0, sysSetupStruc);
  
  // Validate EEPROM data integrity
  // Ensure ssid and pass are null-terminated to prevent strlen() overflow
  sysSetupStruc.ssid[31] = '\0';
  sysSetupStruc.pass[63] = '\0';
  
  // Check if this is first start - do this BEFORE using any fields!
  #ifdef ESP32
    if (digitalRead(USR_BTN) == LOW)
    {
      sysSetupStruc.FirstStart = 1;  // Force reconfiguration
    }
  #endif
  
  // Initialize default config if needed (BEFORE using any struct fields!)
  if (sysSetupStruc.FirstStart != 55)
  {
    Serial.println("First start detected. Initializing default configuration...");
    initDefaultConfig();
    // Config is now in memory, but also saved to EEPROM in initDefaultConfig()
    // Reload from EEPROM to ensure consistency
    EEPROM.get(0, sysSetupStruc);
    sysSetupStruc.ssid[31] = '\0';
    sysSetupStruc.pass[63] = '\0';
    
    #ifdef ESP32
      digitalWrite(LED_HTTP, HIGH);
    #endif
    vfd.writeStringUniverslaChrTab("Conf", 1);
    offlineMode = true;
    setSystemState(STATE_AP_MODE);
    // Start AP mode immediately for configuration
    startAPMode();
  }
  
  // At this point, struct is guaranteed to have valid data
  Serial.println("Configuration loaded.");
  
  // Additional validation: check if SSID length is reasonable
  size_t ssidLen = strlen(sysSetupStruc.ssid);
  size_t passLen = strlen(sysSetupStruc.pass);
  if (ssidLen > 32 || passLen > 64) {
    Serial.println("WARNING: String data corrupted, clearing...");
    memset(sysSetupStruc.ssid, 0, sizeof(sysSetupStruc.ssid));
    memset(sysSetupStruc.pass, 0, sizeof(sysSetupStruc.pass));
  }
  
  // Validate and clamp values to safe ranges
  if (sysSetupStruc.ledCount == 0 || sysSetupStruc.ledCount > LED_STRIP_MAX_NUM_LEDS) {
    Serial.print("WARNING: Invalid LED count (");
    Serial.print(sysSetupStruc.ledCount);
    Serial.println("), using default: 4");
    sysSetupStruc.ledCount = 4;
  }
  
  if (sysSetupStruc.displayBrightness > 7) {
    Serial.println("WARNING: Invalid display brightness, using default: 1");
    sysSetupStruc.displayBrightness = 1;
  }
  
  if (sysSetupStruc.ambLightBrightness == 0) {
    Serial.println("WARNING: Invalid ambient light brightness, using default: 255");
    sysSetupStruc.ambLightBrightness = 255;
  }
  
  // Validate screen driver (0-15 valid range for PT63xx)
  if (sysSetupStruc.screenDriver > 15) {
    Serial.print("WARNING: Invalid screen driver (");
    Serial.print(sysSetupStruc.screenDriver);
    Serial.println("), using default: PT6315_DIG12_SEG16");
    sysSetupStruc.screenDriver = PT6315_DIG12_SEG16;
  }
  
  // Initialize VFD with saved screen driver settings
  vfd.begin(sysSetupStruc.screenDriver);
  vfd.setScreenDirection(sysSetupStruc.screenDirection);
  vfd.clearDisplay();
   
  delay(200);
  #ifdef ESP32
    digitalWrite(INV_ENABLE, HIGH);
  #endif
  
  // Copy custom char data
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
  
  FastLED.addLeds<WS2811, LED_STRIP_PIN, GRB>(leds, sysSetupStruc.ledCount).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(sysSetupStruc.ambLightBrightness);
  
  // Initialize LED effects
  initLEDEffects(leds, sysSetupStruc.ledCount);
  
  // Start LED animation in separate FreeRTOS task
  startLEDAnimationTask();

  vfd.writeStringUniverslaChrTab(" HELLO", 0);
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
  #ifdef ESP32
    digitalWrite(LED_HTTP, LOW);

    for (int i = 0; i < 3; i++)
    {
      digitalWrite(LED_WIFI, HIGH);
      delay(200);
      digitalWrite(LED_WIFI, LOW);
      delay(200);
    }
  #endif
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
    
    #ifdef ESP32
      digitalWrite(LED_WIFI, LOW);
      digitalWrite(LED_HTTP, HIGH);
    #endif
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
    #ifdef ESP32
      digitalWrite(LED_WIFI, HIGH);
    #endif

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
      #ifdef ESP32
        digitalWrite(LED_WIFI, LOW);
        digitalWrite(LED_HTTP, HIGH);
      #endif
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
  
  #ifdef ESP32
    digitalWrite(LED_WIFI, LOW);
    digitalWrite(LED_HTTP, LOW);
  #endif
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
  static uint32_t lastLocalSensorUpdate = 0;
  static uint32_t lastWeatherUpdate = 0;
  static uint32_t lastCurrencyUpdate = 0;

  if (sysSetupStruc.sensorTemperature && tempSensorAvailable &&
      (lastLocalSensorUpdate == 0 || millis() - lastLocalSensorUpdate >= LOCAL_SENSOR_UPDATE_MS)) {
    lastLocalSensorUpdate = millis();
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
  } else if (!sysSetupStruc.sensorTemperature) {
    sensorTemp = NAN;
    sensorHum = -1;
  }

  if (!isInternetAvailable()) {
    return;
  }

  if (sysSetupStruc.sensorWeatherApi) {
    if (lastWeatherUpdate == 0 || millis() - lastWeatherUpdate >= WEATHER_UPDATE_MS) {
      lastWeatherUpdate = millis();
      if (!fetchWeatherData()) {
        Serial.println("Weather update skipped due to fetch error");
      }
    }
  } else {
    lastWeatherUpdate = 0;
    weatherTemp = NAN;
    strcpy(weatherCond, "--");
  }

  if (sysSetupStruc.sensorCurrency) {
    if (lastCurrencyUpdate == 0 || millis() - lastCurrencyUpdate >= CURRENCY_UPDATE_MS) {
      lastCurrencyUpdate = millis();
      if (!fetchCurrencyRates()) {
        Serial.println("Currency update skipped due to fetch error");
      }
    }
  } else {
    lastCurrencyUpdate = 0;
    currencyEUR = NAN;
    currencyUSD = NAN;
    currencyBTC = NAN;
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
