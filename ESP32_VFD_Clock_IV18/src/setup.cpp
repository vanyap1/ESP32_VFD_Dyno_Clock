
#include <Arduino.h>
#include "setup.h"
#include <EEPROM.h>
#include <esp_task_wdt.h>
#include <WiFi.h>
#include "dto.h"
#include <html_page.h>


WiFiServer server(80);  


void saveLoginData(String ssid, String pass) {
    ssid = ssid.substring(0, sizeof(sysSetupStruc.ssid) - 1);
    pass = pass.substring(0, sizeof(sysSetupStruc.pass) - 1);

    ssid.toCharArray(sysSetupStruc.ssid, sizeof(sysSetupStruc.ssid));
    pass.toCharArray(sysSetupStruc.pass, sizeof(sysSetupStruc.pass));

    sysSetupStruc.FirstStart = 55;
    sysSetupStruc.ntpServerIndex = 0;
    sysSetupStruc.ntpTimeZone = 0;
    EEPROM.put(0, sysSetupStruc);
    EEPROM.commit();

    Serial.println("Дані збережені:");
    Serial.print("SSID: ");
    Serial.println(sysSetupStruc.ssid);
    Serial.print("Password: ");
    Serial.println(sysSetupStruc.pass);
    ESP.restart();
}


void ClientSetup(void) {
    esp_task_wdt_deinit();
    
    // Спочатку скануємо мережі в STA режимі
    WiFi.mode(WIFI_STA);
    Serial.println("Scanning WiFi networks...");
    int n = WiFi.scanNetworks();
    Serial.print("Networks found: ");
    Serial.println(n);
    
    // Формуємо список опцій для HTML
    String options = "";
    for (int i = 0; i < n; ++i) {
        options += "<option value=\"";
        options += WiFi.SSID(i);
        options += "\">";
        options += WiFi.SSID(i);
        options += " (";
        options += WiFi.RSSI(i);
        options += " dBm)";
        options += "</option>";
        Serial.print("Found SSID: ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.println(" dBm)");
    }
    
    // Тепер переходимо в AP режим
    Serial.println("Starting AP mode...");
    Serial.print("AP SSID: ");
    Serial.println(FIRST_SETUP_AP_NAME);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(FIRST_SETUP_AP_NAME, NULL, 3);
    
    EEPROM.get(0, sysSetupStruc);

    server.begin(); 

    while(true){
        esp_task_wdt_reset();
        httpLoop(options);  // Передаємо готовий список
        delay(10);   
    }
}

void httpLoop(String options) {
    WiFiClient client = server.available();

    if (client) {
        Serial.println("New Client.");
        String header = "";
        String currentLine = "";

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                header += c;

                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // Замінюємо %OPTIONS% у HTML-коді на список мереж
                        String page = apConfig;
                        page.replace("%OPTIONS%", options);

                        // Відправляємо сторінку клієнту
                        client.println(page);

                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }

        if (header.indexOf("GET /submit") >= 0) {
            int ssidIndex = header.indexOf("ssid=") + 5;
            int passIndex = header.indexOf("pass=") + 5;

            String ssid = header.substring(ssidIndex, header.indexOf('&', ssidIndex));
            String pass = header.substring(passIndex, header.indexOf(' ', passIndex));

            saveLoginData(ssid, pass);
        }
        client.stop();
        Serial.println("Client Disconnected.");
    }
}