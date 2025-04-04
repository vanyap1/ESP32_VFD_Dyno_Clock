//#include <Arduino.h>
#ifndef CUSTOM_BOARD_H
#define CUSTOM_BOARD_H

#define ESP32_DEV_SOCKET 
//#define SMALL_ESP32_MODULE_v1 
//#define ESP32_RELAY_MODULE_V2

#define IO0 21

#if defined(ESP32_DEV_SOCKET)
    // Transceiver side
    //CAN IO26 - RX 
    //CAN IO25 - TX
    #define SDA_PIN 23  // Пін SDA
    #define SCL_PIN 22  // Пін SCL
    
    #define VFD_RESET       33
    #define VFD_CS          32
    #define VFD_FILAMENT    27

    #define VFD_SCK         5
    #define VFD_MOSI        19
    #define VFD_MISO        18
    
    #define userBtn   4

    #define Wifi_LED 14
    #define Server_LED 12
    #define LED_STRIP_PIN 13
    #define LED_STRIP_NUM_LEDS 5
    #define GRB_LED_TYPE
#endif

#if defined(SMALL_ESP32_MODULE_v1)
    #define SDA_PIN 23  // Пін SDA
    #define SCL_PIN 22  // Пін SCL
    
    #define LCD_RS     27
    #define LCD_EN     14
    #define LCD_D4     26
    #define LCD_D5     32
    #define LCD_D6     33
    #define LCD_D7     25

    #define Relay     18
    #define Dio       19
    #define in1       16
    #define in2       17
    #define userBtn   4

    #define Wifi_LED 14
    #define Server_LED 12
    #define LED_STRIP_PIN 13
    #define LED_STRIP_NUM_LEDS 5
    #define GRB_LED_TYPE
#endif

#if defined(ESP32_RELAY_MODULE_V2)
//LiquidCrystal My_LCD(15, 27, 26, 32, 33, 25);
    #define SDA_PIN 23  // Пін SDA
    #define SCL_PIN 22  // Пін SCL

    #define LCD_RS     15
    #define LCD_EN     27
    #define LCD_D4     26
    #define LCD_D5     32
    #define LCD_D6     33
    #define LCD_D7     25

    #define Relay     18
    #define Dio       19
    #define in1       16
    #define in2       17
    #define userBtn   4

    #define Wifi_LED 14
    #define Server_LED 12
    #define LED_STRIP_PIN 13
    #define LED_STRIP_NUM_LEDS 2
#endif

#if defined(SMALL_ESP32_MODULE_v1) && defined(ESP32_RELAY_MODULE_V2)
    #error "Cannot define both SMALL_ESP32_MODULE and ESP32_RELAY_MODULE_V2 at the same time. Define only one."
#endif

#endif