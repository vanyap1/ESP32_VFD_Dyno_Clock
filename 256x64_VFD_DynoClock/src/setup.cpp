
#include <Arduino.h>
#include <EEPROM.h>
#include <esp_task_wdt.h>
#include <WiFi.h>
#include "dto.h"
#include <html_page.h>

void httpLoop();

WiFiServer server(80);  


void saveLoginData(String ssid, String pass) {
    ssid.toCharArray(sysSetupStruc.ssid, sizeof(sysSetupStruc.ssid));
    pass.toCharArray(sysSetupStruc.pass, sizeof(sysSetupStruc.pass));
    
    sysSetupStruc.FirstStart = 1;  
    
    
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
    WiFi.mode(WIFI_AP);
    WiFi.softAP("VfdDyno");
    esp_task_wdt_reset();
    
    EEPROM.get(0, sysSetupStruc);

    server.begin(); 

    while(true){

        httpLoop(); 
    }


}

void httpLoop() {
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

                        // Скануємо доступні Wi-Fi мережі
                        int n = WiFi.scanNetworks();
                        Serial.println("Scanning WiFi networks...");
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
                        }

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