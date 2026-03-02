#include <Arduino.h>
#include <EEPROM.h>
#include <WiFiClientSecure.h>
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
#include "setup.h"
#include <time.h>
#include <esp_task_wdt.h>
#include <Update.h>
#include "FastLED.h"
#include <Wire.h>
#include "RV8803.h"
#include "HDC2010.h"


#define LED_HTTP 16
#define LED_WIFI 17
#define INV_ENABLE 21
#define LED_STRIP_MAX_NUM_LEDS 16
#define LED_STRIP_PIN 33
#define USR_BTN 4
#define VFD_EN

// I2C pins
#define I2C_SDA 23
#define I2C_SCL 22


PT63XX vfd(27, IV18);
CRGB leds[LED_STRIP_MAX_NUM_LEDS];

// RTC and sensors
RV8803 rtc;
HDC2010 tempHumiditySensor;
bool rtcAvailable = false;
bool tempSensorAvailable = false;

// Змінні для зберігання часу
struct tm timeinfo;
bool blinkingDot = false;
SystemSetup sysSetupStruc;

// Offline mode and AP mode variables
bool offlineMode = false;
bool apModeActive = false;
DNSServer dnsServerAP;

// System state for LED indication
enum SystemState {
  STATE_NORMAL,           // Normal operation - use user selected effect
  STATE_WIFI_CONNECTING,  // Connecting to WiFi - chase animation
  STATE_WIFI_FAILED,      // WiFi connection failed - red pulse
  STATE_AP_MODE,          // AP mode active - blue pulse
  STATE_SCANNING          // Scanning networks - orange blink
};
SystemState systemState = STATE_NORMAL;

WiFiServer serverConfigured(80);
WiFiUDP ntpUDP;

#define NTP_UPDATE_INTERVAL 216000000  // Оновлення часу кожні 60*60*60 секунд
#define NTP_FIRST_UPDATE_DELAY 5000  // Затримка перед першим оновленням часу

uint32_t ntpUpdateInterval = NTP_FIRST_UPDATE_DELAY;  // Оновлення часу кожні 60 секунд


const char *ntpServers[] = {"pool.ntp.org", "time.google.com", "time.windows.com"};
NTPClient timeClient(ntpUDP, ntpServers[0]);

bool screeenUpdateRestricted = false;

// Тестові дані для сенсорів та API
float sensorTemp = 23.5;       // Температура з датчика
int sensorPress = 1013;         // Тиск з датчика
int sensorHum = 65;             // Вологість з датчика
float weatherTemp = 18.0;       // Температура з Weather API
float currencyEUR = 43.5;       // Курс євро
float currencyUSD = 40.2;       // Курс долара
float currencyBTC = 98.3;       // Курс біткоіна (в тисячах)
char weatherCond[16] = "Cloudy"; // Умови погоди

// Параметри секвенсера екранів
int currentScreenIndex = 0;     // Поточний екран (0, 1, 2)
uint32_t screenSwitchTimer = 0; // Таймер перемикання екранів

// Функція для парсингу формату і заміни змінних
String parseDisplayFormat(String format, struct tm &timeinfo) {
  String result = format;
  char buffer[16];
  
  // Час
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


void updateLEDEffect();
void startAPMode();
void updateSensors();
void syncRTCTime();

// I2C scanner function for diagnostics
void scanI2C() {
  Serial.println("\n=== I2C Scanner ===");
  Serial.println("Scanning I2C bus...");
  
  byte count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print(" (");
      Serial.print(address);
      Serial.println(")");
      count++;
    }
  }
  
  if (count == 0) {
    Serial.println("No I2C devices found!");
  } else {
    Serial.print("Found ");
    Serial.print(count);
    Serial.println(" device(s)");
  }
  Serial.println("==================\n");
}

void initDefaultConfig()
{

  memset(&sysSetupStruc.ssid, 0, sizeof(sysSetupStruc.ssid));
  memset(&sysSetupStruc.pass, 0, sizeof(sysSetupStruc.pass));
  

  sysSetupStruc.ntpEN = 1; 
  sysSetupStruc.ntpServerIndex = 0;
  sysSetupStruc.ntpTimeZone = 0;
  
  
  strcpy(sysSetupStruc.displayFormat[0], "  *HH* *MM*");
  sysSetupStruc.displayFormatTime[0] = 3;
  sysSetupStruc.displayFormatEnable[0] = true;
  sysSetupStruc.displayFormatBlink[0] = true;
  
  strcpy(sysSetupStruc.displayFormat[1], "1 1111111");
  sysSetupStruc.displayFormatTime[1] = 2;
  sysSetupStruc.displayFormatEnable[1] = true;
  sysSetupStruc.displayFormatBlink[1] = false;
  
  strcpy(sysSetupStruc.displayFormat[2], "2 222222");
  sysSetupStruc.displayFormatTime[2] = 2;
  sysSetupStruc.displayFormatEnable[2] = true;
  sysSetupStruc.displayFormatBlink[2] = false;
  

  sysSetupStruc.blinkMask = 0xFFFFFF;
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
    systemState = STATE_AP_MODE;
  }
  FastLED.addLeds<WS2811, LED_STRIP_PIN, GRB>(leds, sysSetupStruc.ledCount).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(sysSetupStruc.ambLightBrightness);

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
  systemState = STATE_WIFI_CONNECTING;
  
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
    systemState = STATE_AP_MODE;
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
    systemState = STATE_NORMAL; // Switch to normal operation
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
uint32_t ledUpdateTimer = 0;
uint8_t ledAnimationPhase = 0;

// LED state indication functions
void ledIndicateWiFiConnecting() {
  // Chase effect for WiFi connecting
  int position = (ledAnimationPhase / 8) % 4;
  for (int i = 0; i < 4; i++) {
    if (i == position) {
      leds[i] = CRGB(0, 100, 255); // Blue leading LED
    } else if (i == (position - 1 + 4) % 4) {
      leds[i] = CRGB(0, 30, 80); // Dimmer trail
    } else {
      leds[i] = CRGB(0, 0, 0);
    }
  }
}

void ledIndicateWiFiFailed() {
  // Red pulsing for WiFi error
  uint8_t brightness = beatsin8(30, 50, 255);
  for (int i = 0; i < 4; i++) {
    leds[i] = CRGB(brightness, 0, 0);
  }
}

void ledIndicateAPMode() {
  // Slow blue pulse for AP mode
  uint8_t brightness = beatsin8(20, 30, 200);
  for (int i = 0; i < 4; i++) {
    leds[i] = CRGB(0, brightness / 3, brightness);
  }
}

void ledIndicateScanning() {
  // Fast orange blink for scanning
  uint8_t blink = (ledAnimationPhase / 10) % 2;
  for (int i = 0; i < 4; i++) {
    if (blink) {
      leds[i] = CRGB(255, 100, 0);
    } else {
      leds[i] = CRGB(0, 0, 0);
    }
  }
}

// LED Effect Functions
void updateLEDEffect() {
  // Update LEDs at ~50Hz for smooth animations
  if (millis() - ledUpdateTimer < 20) {
    return;
  }
  ledUpdateTimer = millis();
  
  // Increment animation phase (0-255)
  ledAnimationPhase++;
  
  // Check if we're in a special system state
  if (systemState != STATE_NORMAL) {
    FastLED.setBrightness(255); // Full brightness for status indication
    
    switch(systemState) {
      case STATE_WIFI_CONNECTING:
        ledIndicateWiFiConnecting();
        break;
      case STATE_WIFI_FAILED:
        ledIndicateWiFiFailed();
        break;
      case STATE_AP_MODE:
        ledIndicateAPMode();
        break;
      case STATE_SCANNING:
        ledIndicateScanning();
        break;
      default:
        break;
    }
    
    FastLED.show();
    return;
  }
  
  FastLED.setBrightness(sysSetupStruc.ambLightBrightness);
  
  switch(sysSetupStruc.ledEffect) {
    case 0: // Solid Color
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        leds[i] = CRGB(sysSetupStruc.ambLightColr[0], 
                       sysSetupStruc.ambLightColr[1], 
                       sysSetupStruc.ambLightColr[2]);
      }
      break;
      
    case 1: // Rainbow
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        leds[i] = CHSV((ledAnimationPhase + i * (256 / sysSetupStruc.ledCount)) % 256, 255, 255);
      }
      break;
      
    case 2: // Breathing
      {
        uint8_t brightness = (exp(sin(ledAnimationPhase / 20.0 * PI)) - 0.36787944) * 108.0;
        for (int i = 0; i < sysSetupStruc.ledCount; i++) {
          leds[i] = CRGB(
            (sysSetupStruc.ambLightColr[0] * brightness) / 255,
            (sysSetupStruc.ambLightColr[1] * brightness) / 255,
            (sysSetupStruc.ambLightColr[2] * brightness) / 255
          );
        }
      }
      break;
      
    case 3: // Pulse
      {
        uint8_t pulse = beatsin8(60, 0, 255);
        for (int i = 0; i < sysSetupStruc.ledCount; i++) {
          leds[i] = CRGB(
            (sysSetupStruc.ambLightColr[0] * pulse) / 255,
            (sysSetupStruc.ambLightColr[1] * pulse) / 255,
            (sysSetupStruc.ambLightColr[2] * pulse) / 255
          );
        }
      }
      break;
      
    case 4: // Wave
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        uint8_t brightness = sin8((ledAnimationPhase * 2) + (i * 32));
        leds[i] = CRGB(
          (sysSetupStruc.ambLightColr[0] * brightness) / 255,
          (sysSetupStruc.ambLightColr[1] * brightness) / 255,
          (sysSetupStruc.ambLightColr[2] * brightness) / 255
        );
      }
      break;
      
    case 5: // Chase
      {
        int position = (ledAnimationPhase / 8) % sysSetupStruc.ledCount;
        for (int i = 0; i < sysSetupStruc.ledCount; i++) {
          if (i == position) {
            leds[i] = CRGB(sysSetupStruc.ambLightColr[0], 
                           sysSetupStruc.ambLightColr[1], 
                           sysSetupStruc.ambLightColr[2]);
          } else if (i == (position - 1 + sysSetupStruc.ledCount) % sysSetupStruc.ledCount) {
            leds[i] = CRGB(sysSetupStruc.ambLightColr[0] / 4, 
                           sysSetupStruc.ambLightColr[1] / 4, 
                           sysSetupStruc.ambLightColr[2] / 4);
          } else {
            leds[i] = CRGB(0, 0, 0);
          }
        }
      }
      break;
      
    case 6: // Twinkle
      {
        static uint32_t lastTwinkle = 0;
        if (millis() - lastTwinkle > 100) {
          lastTwinkle = millis();
          int led = random(sysSetupStruc.ledCount);
          if (random(100) < 30) { // 30% chance to twinkle
            leds[led] = CRGB(sysSetupStruc.ambLightColr[0], 
                            sysSetupStruc.ambLightColr[1], 
                            sysSetupStruc.ambLightColr[2]);
          } else {
            leds[led].fadeToBlackBy(64);
          }
          // Fade all LEDs slightly
          for (int i = 0; i < sysSetupStruc.ledCount; i++) {
            leds[i].fadeToBlackBy(16);
          }
        }
      }
      break;
      
    default:
      // Fallback to solid color
      for (int i = 0; i < sysSetupStruc.ledCount; i++) {
        leds[i] = CRGB(sysSetupStruc.ambLightColr[0], 
                       sysSetupStruc.ambLightColr[1], 
                       sysSetupStruc.ambLightColr[2]);
      }
      break;
  }
  
  FastLED.show();
}

// Non-blocking AP mode startup
void startAPMode() {
  Serial.println("Starting non-blocking AP mode...");
  
  // Setup AP mode (keeps existing WiFi STA mode if connected)
  if (WiFi.status() == WL_CONNECTED) {
    // Already connected, add AP mode
    WiFi.mode(WIFI_AP_STA);
  } else {
    // Not connected, AP only
    WiFi.mode(WIFI_AP);
  }
  
  WiFi.softAP(FIRST_SETUP_AP_NAME, NULL, 3);
  
  // Start DNS server for captive portal
  dnsServerAP.start(53, "*", WiFi.softAPIP());
  
  apModeActive = true;
  
  Serial.print("AP SSID: ");
  Serial.println(FIRST_SETUP_AP_NAME);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("Web interface available at: http://" + WiFi.softAPIP().toString());
}

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
        String displayText = parseDisplayFormat(String(sysSetupStruc.displayFormat[currentScreenIndex]), timeinfo);
        
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
  if (apModeActive) {
    dnsServerAP.processNextRequest();
  }

  WiFiClient client = serverConfigured.available(); // Очікуємо нових клієнтів
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
            if (header.indexOf("GET /cmd=GET:SETTINGS?") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println("Connection: close");
  client.println();

  StaticJsonDocument<1024> jsonDoc;
  
  JsonObject wifi = jsonDoc.createNestedObject("wifi");
  wifi["ssid"] = sysSetupStruc.ssid;
  wifi["security"] = 0; // WPA2
  
  JsonObject ntp = jsonDoc.createNestedObject("ntp");
  ntp["server"] = sysSetupStruc.ntpServerIndex;
  ntp["enabled"] = sysSetupStruc.ntpEN ? true : false;
  
  jsonDoc["timezone"] = sysSetupStruc.ntpTimeZone-12;
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
            }
            else if (header.indexOf("POST /cmd=FIRMWARE") >= 0)
            {
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
                  client.println("HTTP/1.1 500 Internal Server Error");
                  client.println("Content-type:text/plain");
                  client.println("Connection: close");
                  client.println();
                  client.println("Not enough space");
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
                      
                      client.println("HTTP/1.1 200 OK");
                      client.println("Content-type:text/plain");
                      client.println("Connection: close");
                      client.println();
                      client.println("Firmware updated successfully. Restarting...");
                      
                      delay(1000);
                      ESP.restart();
                    } else {
                      Serial.print("Update end error: ");
                      Serial.println(Update.errorString());
                      
                      client.println("HTTP/1.1 500 Internal Server Error");
                      client.println("Content-type:text/plain");
                      client.println("Connection: close");
                      client.println();
                      client.print("Update error: ");
                      client.println(Update.errorString());
                    }
                  } else {
                    Serial.println("Incomplete firmware upload");
                    Update.abort();
                    
                    client.println("HTTP/1.1 400 Bad Request");
                    client.println("Content-type:text/plain");
                    client.println("Connection: close");
                    client.println();
                    client.println("Incomplete firmware data");
                  }
                }
              } else {
                client.println("HTTP/1.1 400 Bad Request");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Invalid content length");
              }
            }
            else if (header.indexOf("POST /cmd=LOAD") >= 0)
            {
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
                
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Character table loaded successfully");
              } else {
                Serial.print("Error: Expected ");
                Serial.print(expectedBytes);
                Serial.print(" bytes, got ");
                Serial.println(bytesRead);
                
                client.println("HTTP/1.1 400 Bad Request");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Error: Incomplete data received");
              }
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
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Settings updated");
            }
            else if(header.indexOf("GET /chargen") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              
              const char* ptr = charGen;
              size_t len = strlen(charGen);
              const size_t chunkSize = 1024;
              while (len > 0) {
                size_t toSend = (len > chunkSize) ? chunkSize : len;
                client.write((const uint8_t*)ptr, toSend);
                ptr += toSend;
                len -= toSend;
                delay(1); 
              }
            }
            else if(header.indexOf("GET /styles.css") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/css");
              client.println("Connection: close");
              client.println();
              
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
            else if(header.indexOf("GET /commonRest.js") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/javascript");
              client.println("Connection: close");
              client.println();
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
                                  String(offlineMode ? "OFFLINE" : "READY");
              client.println(deviceInfo);
            }
            else if(header.indexOf("GET /cmd=DUMP?") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:application/octet-stream");
              client.println("Content-Disposition: attachment; filename=\"chartab.bin\"");
              client.println("Connection: close");
              client.println();
              
              client.write((const uint8_t*)sysSetupStruc.customCharData, sizeof(sysSetupStruc.customCharData));
              
              Serial.println("Custom character table dumped (384 bytes)");
            }
            else if(header.indexOf("GET /cmd=SEG?") >= 0){
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
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
            }
            
            else if(header.indexOf("GET /cmd=SEG=") >= 0){
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
  
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Segment bitmask updated");
            }

            
            else if(header.indexOf("GET /cmd=CLOCK") >= 0){
              screeenUpdateRestricted = false;
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Switched to CLOCK mode");
            }
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

            else if (header.indexOf("GET /cmd=TEXT") >= 0)
            {
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
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Text displayed");
            }

            else if (header.indexOf("GET /cmd=WIFI:SCAN") >= 0)
            {
              Serial.println("Scanning WiFi networks...");
              
              // Set LED state to scanning
              SystemState previousState = systemState;
              systemState = STATE_SCANNING;
              
              // Set to STA+AP mode for scanning
              WiFiMode_t currentMode = WiFi.getMode();
              if (currentMode == WIFI_AP) {
                WiFi.mode(WIFI_AP_STA);
              }
              
              int n = WiFi.scanNetworks();
              
              // Restore previous state
              systemState = previousState;
              Serial.print("Networks found: ");
              Serial.println(n);
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:application/json");
              client.println("Connection: close");
              client.println();
              
              // Build JSON array of networks
              client.print("[");
              for (int i = 0; i < n; ++i) {
                if (i > 0) client.print(",");
                client.print("{\"ssid\":\"");
                client.print(WiFi.SSID(i));
                client.print("\",\"rssi\":");
                client.print(WiFi.RSSI(i));
                client.print(",\"secure\":");
                client.print(WiFi.encryptionType(i) != WIFI_AUTH_OPEN ? "true" : "false");
                client.print("}");
              }
              client.print("]");
              
              // Restore previous mode
              if (currentMode == WIFI_AP) {
                WiFi.mode(WIFI_AP);
              }
            }

            else if (header.indexOf("GET /cmd=CHARSET") >= 0)
            {
              screeenUpdateRestricted = true;
              int paramStart = header.indexOf("GET /cmd=CHARSET") + strlen("GET /cmd=CHARSET");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              params.trim();
              
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

            else if (header.indexOf("GET / ") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              
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

            else if (header.indexOf("GET /cmd=CHARTEST") >= 0)
            {
              screeenUpdateRestricted = true;
              int paramStart = header.indexOf("GET /cmd=CHARTEST") + strlen("GET /cmd=CHARTEST");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              params.trim();
              
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
            else if (header.indexOf("GET /cmd=WIFI:SAVE=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=WIFI:SAVE=") + strlen("GET /cmd=WIFI:SAVE=");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              params = urlDecode(params);
              
              Serial.print("WIFI:SAVE received params: ");
              Serial.println(params);
              
              int firstComma = params.indexOf(",");
              int secondComma = params.indexOf(",", firstComma + 1);
              
              if (firstComma != -1 && secondComma != -1)
              {
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
                
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("WiFi settings saved. Device will restart...");
                client.flush(); // Ensure response is sent
                client.stop();  // Close connection
                
                Serial.println("Restarting in 2 seconds...");
                delay(2000);
                ESP.restart();
              }
              else
              {
                Serial.println("Invalid WIFI:SAVE parameters!");
                client.println("HTTP/1.1 400 Bad Request");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Invalid parameters");
              }
            }
            else if (header.indexOf("GET /cmd=DATETIME,") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=DATETIME,") + strlen("GET /cmd=DATETIME,");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              int year, month, day, hour, minute, second;
              if (sscanf(params.c_str(), "%d,%d,%d,%d,%d,%d", &year, &month, &day, &hour, &minute, &second) == 6)
              {
                struct tm manualTime;
                manualTime.tm_year = year - 1900;
                manualTime.tm_mon = month - 1;
                manualTime.tm_mday = day;
                manualTime.tm_hour = hour;
                manualTime.tm_min = minute;
                manualTime.tm_sec = second;
                
                time_t t = mktime(&manualTime);
                struct timeval now = { .tv_sec = t };
                settimeofday(&now, NULL);
                
                // Update RTC with new time
                if (rtcAvailable) {
                  rtc.setFromSystemTime();
                  Serial.println("RTC updated with manual time");
                }
                
                Serial.println("Manual time set");
                
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Time set successfully");
              }
              else
              {
                client.println("HTTP/1.1 400 Bad Request");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Invalid date format");
              }
            }
            else if (header.indexOf("GET /cmd=NTP:SERVER=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=NTP:SERVER=") + strlen("GET /cmd=NTP:SERVER=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.ntpServerIndex = value.toInt();
              configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("NTP server set to: ");
              Serial.println(sysSetupStruc.ntpServerIndex);
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("NTP server updated");
            }
            else if (header.indexOf("GET /cmd=TIMEZONE=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=TIMEZONE=") + strlen("GET /cmd=TIMEZONE=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.ntpTimeZone = value.toInt()+12;
              timeClient.setTimeOffset(sysSetupStruc.ntpTimeZone * 3600);
              configTime(sysSetupStruc.ntpTimeZone * 3600, 0, ntpServers[sysSetupStruc.ntpServerIndex]);
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("Timezone set to: ");
              Serial.println(sysSetupStruc.ntpTimeZone);
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Timezone updated");
            }
            else if (header.indexOf("GET /cmd=DISPLAY:SCREEN1=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=DISPLAY:SCREEN1=") + strlen("GET /cmd=DISPLAY:SCREEN1=");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              params = urlDecode(params);
              
              int comma1 = params.indexOf(",");
              int comma2 = params.indexOf(",", comma1 + 1);
              int comma3 = params.indexOf(",", comma2 + 1);
              
              if (comma1 != -1 && comma2 != -1 && comma3 != -1)
              {
                String format = params.substring(0, comma1);
                format.toCharArray(sysSetupStruc.displayFormat[0], sizeof(sysSetupStruc.displayFormat[0]));
                sysSetupStruc.displayFormatTime[0] = params.substring(comma1 + 1, comma2).toInt();
                sysSetupStruc.displayFormatEnable[0] = params.substring(comma2 + 1, comma3).toInt();
                sysSetupStruc.displayFormatBlink[0] = params.substring(comma3 + 1).toInt();
                
                EEPROM.put(0, sysSetupStruc);
                EEPROM.commit();
                
                Serial.println("Display Screen 1 settings saved");
                
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Screen 1 updated");
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
            else if (header.indexOf("GET /cmd=DISPLAY:SCREEN2=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=DISPLAY:SCREEN2=") + strlen("GET /cmd=DISPLAY:SCREEN2=");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              params = urlDecode(params);
              
              int comma1 = params.indexOf(",");
              int comma2 = params.indexOf(",", comma1 + 1);
              int comma3 = params.indexOf(",", comma2 + 1);
              
              if (comma1 != -1 && comma2 != -1 && comma3 != -1)
              {
                String format = params.substring(0, comma1);
                format.toCharArray(sysSetupStruc.displayFormat[1], sizeof(sysSetupStruc.displayFormat[1]));
                sysSetupStruc.displayFormatTime[1] = params.substring(comma1 + 1, comma2).toInt();
                sysSetupStruc.displayFormatEnable[1] = params.substring(comma2 + 1, comma3).toInt();
                sysSetupStruc.displayFormatBlink[1] = params.substring(comma3 + 1).toInt();
                
                EEPROM.put(0, sysSetupStruc);
                EEPROM.commit();
                
                Serial.println("Display Screen 2 settings saved");
                
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Screen 2 updated");
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
            else if (header.indexOf("GET /cmd=DISPLAY:SCREEN3=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=DISPLAY:SCREEN3=") + strlen("GET /cmd=DISPLAY:SCREEN3=");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              params = urlDecode(params);
              
              int comma1 = params.indexOf(",");
              int comma2 = params.indexOf(",", comma1 + 1);
              int comma3 = params.indexOf(",", comma2 + 1);
              
              if (comma1 != -1 && comma2 != -1 && comma3 != -1)
              {
                String format = params.substring(0, comma1);
                format.toCharArray(sysSetupStruc.displayFormat[2], sizeof(sysSetupStruc.displayFormat[2]));
                sysSetupStruc.displayFormatTime[2] = params.substring(comma1 + 1, comma2).toInt();
                sysSetupStruc.displayFormatEnable[2] = params.substring(comma2 + 1, comma3).toInt();
                sysSetupStruc.displayFormatBlink[2] = params.substring(comma3 + 1).toInt();
                
                EEPROM.put(0, sysSetupStruc);
                EEPROM.commit();
                
                Serial.println("Display Screen 3 settings saved");
                
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Screen 3 updated");
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
            else if (header.indexOf("GET /cmd=BLINK:POINT=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=BLINK:POINT=") + strlen("GET /cmd=BLINK:POINT=");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              int comma = params.indexOf(",");
              if (comma != -1)
              {
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
                
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Blink settings updated");
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
            else if (header.indexOf("GET /cmd=SENSOR:PRESSURE=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=SENSOR:PRESSURE=") + strlen("GET /cmd=SENSOR:PRESSURE=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.sensorPressure = (value.toInt() == 1);
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("Pressure sensor: ");
              Serial.println(sysSetupStruc.sensorPressure ? "ON" : "OFF");
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Pressure sensor updated");
            }
            else if (header.indexOf("GET /cmd=SENSOR:TEMPERATURE=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=SENSOR:TEMPERATURE=") + strlen("GET /cmd=SENSOR:TEMPERATURE=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.sensorTemperature = (value.toInt() == 1);
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("Temperature sensor: ");
              Serial.println(sysSetupStruc.sensorTemperature ? "ON" : "OFF");
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Temperature sensor updated");
            }
            else if (header.indexOf("GET /cmd=SENSOR:AUTOBRIGHTNESS=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=SENSOR:AUTOBRIGHTNESS=") + strlen("GET /cmd=SENSOR:AUTOBRIGHTNESS=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.sensorAutoBrightness = (value.toInt() == 1);
              
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
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("Auto brightness: ");
              Serial.println(sysSetupStruc.sensorAutoBrightness ? "ON" : "OFF");
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Auto brightness updated");
            }
            else if (header.indexOf("GET /cmd=SENSOR:WEATHERAPI=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=SENSOR:WEATHERAPI=") + strlen("GET /cmd=SENSOR:WEATHERAPI=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.sensorWeatherApi = (value.toInt() == 1);
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("Weather API: ");
              Serial.println(sysSetupStruc.sensorWeatherApi ? "ON" : "OFF");
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Weather API updated");
            }
            else if (header.indexOf("GET /cmd=SENSOR:CURRENCY=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=SENSOR:CURRENCY=") + strlen("GET /cmd=SENSOR:CURRENCY=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.sensorCurrency = (value.toInt() == 1);
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("Currency: ");
              Serial.println(sysSetupStruc.sensorCurrency ? "ON" : "OFF");
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Currency updated");
            }
            else if (header.indexOf("GET /cmd=LED:COLOR=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=LED:COLOR=") + strlen("GET /cmd=LED:COLOR=");
              String params = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              int r, g, b;
              if (sscanf(params.c_str(), "%d,%d,%d", &r, &g, &b) == 3)
              {
                sysSetupStruc.ambLightColr[0] = r;
                sysSetupStruc.ambLightColr[1] = g;
                sysSetupStruc.ambLightColr[2] = b;
                
                EEPROM.put(0, sysSetupStruc);
                EEPROM.commit();
                
                Serial.print("LED color: R=");
                Serial.print(r);
                Serial.print(", G=");
                Serial.print(g);
                Serial.print(", B=");
                Serial.println(b);
                
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("LED color updated");
              }
              else
              {
                client.println("HTTP/1.1 400 Bad Request");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Invalid color format");
              }
            }
            else if (header.indexOf("GET /cmd=DISPLAY:BRIGHTNESS=") >= 0)
            {
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
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Display brightness updated");
            }
            else if (header.indexOf("GET /cmd=LED:BRIGHTNESS=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=LED:BRIGHTNESS=") + strlen("GET /cmd=LED:BRIGHTNESS=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.ambLightBrightness = constrain(value.toInt(), 0, 255);
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("LED brightness: ");
              Serial.println(sysSetupStruc.ambLightBrightness);
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("LED brightness updated");
            }
            else if (header.indexOf("GET /cmd=LED:COUNT=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=LED:COUNT=") + strlen("GET /cmd=LED:COUNT=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.ledCount = value.toInt();
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("LED count: ");
              Serial.println(sysSetupStruc.ledCount);
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("LED count updated");
            }

            else if (header.indexOf("GET /cmd=LED:EFFECT=") >= 0)
            {
              int paramStart = header.indexOf("GET /cmd=LED:EFFECT=") + strlen("GET /cmd=LED:EFFECT=");
              String value = header.substring(paramStart, header.indexOf(" ", paramStart));
              
              sysSetupStruc.ledEffect = value.toInt();
              
              EEPROM.put(0, sysSetupStruc);
              EEPROM.commit();
              
              Serial.print("LED effect: ");
              Serial.println(sysSetupStruc.ledEffect);
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("LED effect updated");
            }
            //return 404 not found
            else{
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
             client.println("404 Not Found");
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