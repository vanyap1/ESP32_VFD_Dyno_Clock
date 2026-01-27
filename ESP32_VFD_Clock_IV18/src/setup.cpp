
#include <Arduino.h>
#include "setup.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <DNSServer.h>
#include "dto.h"
#include "WebRoot/html_page.h"


WiFiServer server(80);
DNSServer dnsServer;  


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
    WiFi.mode(WIFI_OFF); 
    delay(100);
    WiFi.mode(WIFI_AP_STA);
    Serial.println("Scanning WiFi networks...");
    int n = WiFi.scanNetworks();
    Serial.print("Networks found: ");
    Serial.println(n);
    
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
    
    
    Serial.println("Starting AP mode...");
    Serial.print("AP SSID: ");
    Serial.println(FIRST_SETUP_AP_NAME);

    WiFi.mode(WIFI_OFF); 
    delay(100);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(FIRST_SETUP_AP_NAME, NULL, 3);
    
    EEPROM.get(0, sysSetupStruc);
    server.begin();
    dnsServer.start(53, "*", WiFi.softAPIP());
    Serial.println("DNS server started");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    while(true){
        dnsServer.processNextRequest(); 
        httpLoop(options); 
        yield();  
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
                        if (header.indexOf("GET /submit") >= 0) {
                            Serial.println("Submit request detected!");
                            int ssidIndex = header.indexOf("ssid=") + 5;
                            int passIndex = header.indexOf("pass=") + 5;

                            if (ssidIndex > 5 && passIndex > 5) {
                                String ssid = header.substring(ssidIndex, header.indexOf('&', ssidIndex));
                                String pass = header.substring(passIndex, header.indexOf(' ', passIndex));
                                
                                ssid.replace("+", " ");
                                pass.replace("+", " ");

                                Serial.print("SSID: ");
                                Serial.println(ssid);

                                client.println("HTTP/1.1 200 OK");
                                client.println("Content-type:text/html; charset=utf-8");
                                client.println("Connection: close");
                                client.println();
                                client.println("<html><body><h1>Connecting...</h1><p>Device is restarting</p></body></html>");
                                
                                delay(100);
                                client.stop();
                                
                                saveLoginData(ssid, pass);
                                return;
                            }
                        }
                        
                        
                        bool isCaptivePortalCheck = false;
                        
                      
                        if (header.indexOf("GET /generate_204") >= 0 ||
                            header.indexOf("GET /gen_204") >= 0 ||
                            header.indexOf("connectivitycheck") >= 0) {
                            isCaptivePortalCheck = true;
                        }
                       
                        else if (header.indexOf("GET /hotspot-detect.html") >= 0 ||
                                 header.indexOf("captive.apple.com") >= 0) {
                            isCaptivePortalCheck = true;
                        }
                
                        else if (header.indexOf("GET /ncsi.txt") >= 0 ||
                                 header.indexOf("GET /connecttest.txt") >= 0) {
                            isCaptivePortalCheck = true;
                        }
                        
                        if (isCaptivePortalCheck) {
                            Serial.println("Captive portal detection request - redirecting");
                            
                            client.println("HTTP/1.1 302 Found");
                            client.println("Location: http://192.168.4.1/");
                            client.println("Cache-Control: no-cache");
                            client.println("Connection: close");
                            client.println();
                            break;
                        }
                        
                        // Для всіх інших запитів - показати сторінку налаштування
                        Serial.println("Sending config page...");
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html; charset=utf-8");
                        client.println("Cache-Control: no-cache, no-store, must-revalidate");
                        client.println("Pragma: no-cache");
                        client.println("Expires: 0");
                        client.println("Connection: close");
                        client.println();

                        // Замінюємо %OPTIONS% на список мереж
                        String page = apConfig;
                        page.replace("%OPTIONS%", options);

                        // Відправляємо порціями для великих файлів
                        const char* ptr = page.c_str();
                        size_t len = page.length();
                        const size_t chunkSize = 512;
                        while (len > 0 && client.connected()) {
                            size_t toSend = (len > chunkSize) ? chunkSize : len;
                            client.write((const uint8_t*)ptr, toSend);
                            ptr += toSend;
                            len -= toSend;
                            delay(1);
                        }
                        
                        Serial.println("Page sent.");
                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }

        delay(10);
        client.stop();
        Serial.println("Client Disconnected.");
    }
}