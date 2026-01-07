#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "pt6324.h"

// WiFi credentials
const char* ssid = "Internet2";
const char* password = "vp198808";

// UDP
WiFiUDP udp;
const int udpPort = 5005;
char packetBuffer[255]; // Buffer for incoming packets

// Вбудований LED зазвичай на GPIO2 (D4 на платі NodeMCU/WeMos)
// 5 - clk
// 7 - data
// 16 - STDBY
const int ledPin = 2; // D4

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  pinMode(ledPin, OUTPUT); // Встановлюємо пін як вихід
  digitalWrite(ledPin, HIGH);
  
  // Ініціалізація дисплея
  pt63Init();
  pt63SetBrightness(7);
  
  // Показуємо статус підключення
  writeChars((uint8_t*)"Connecting WiFi ", 16);
  
  // Підключення до WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  
  // Показуємо IP на дисплеї
  char ipBuffer[17];
  sprintf(ipBuffer, "IP%s  ", WiFi.localIP().toString().c_str());
  writeChars((uint8_t*)ipBuffer, 16);
  delay(2000);
  
  // Запускаємо UDP сервер
  udp.begin(udpPort);
  Serial.printf("UDP server started on port %d\n", udpPort);
  
  writeChars((uint8_t*)"UDP Ready 5005  ", 16);
  delay(1000);
  writeChars((uint8_t*)"Waiting for msg..", 16);
}

void loop() {
  // Перевіряємо наявність UDP пакетів
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.printf("Received packet of size %d from %s:%d\n", 
                  packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    
    // Читаємо пакет
    int len = udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0; // Додаємо нульовий термінатор
    }
    
    Serial.printf("UDP packet contents: %s\n", packetBuffer);
    
    // Парсимо JSON та витягуємо total_power
    char displayBuffer[17];
    char* totalPowerPos = strstr(packetBuffer, "\"total_power\":");
    
    if (totalPowerPos != NULL) {
      // Знаходимо позицію після "total_power":
      totalPowerPos += strlen("\"total_power\":");
      
      // Пропускаємо пробіли
      while (*totalPowerPos == ' ') {
        totalPowerPos++;
      }
      
      // Витягуємо число
      int totalPower = atoi(totalPowerPos);
      
      // Форматуємо для відображення на дисплеї
      sprintf(displayBuffer, "Power %4dW    ", totalPower);
      
      Serial.printf("Total power: %d W\n", totalPower);
    } else {
      // Якщо не знайшли total_power, показуємо помилку
      sprintf(displayBuffer, "Parse Error     ");
      Serial.println("Could not find total_power in JSON");
    }
    
    writeChars((uint8_t*)displayBuffer, 16);
    
    // Мигаємо LED для індикації отримання повідомлення
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
  }
  
  delay(10); // Невелика затримка для стабільності
}