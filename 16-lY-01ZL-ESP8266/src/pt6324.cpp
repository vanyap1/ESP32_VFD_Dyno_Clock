#include "pt6324.h"
#include <stdint.h>
#include <Arduino.h>
#include <SPI.h> // Додано для апаратного SPI

#define VFD_CLK_PIN     D5  // GPIO14 (SCK)
#define VFD_DATA_PIN    D7  // GPIO13 (MOSI)
#define VFD_STDBY_PIN   D8  // GPIO15 (CS)

#define PT6324

#ifndef PT6324
#ifndef PT6315
#error "Error: You must define either PT6324 or PT6315!"
#endif
#endif

#ifdef PT6324
#ifdef PT6315
#error "Error: Only one of PT6324 or PT6315 should be defined!"
#endif
#endif

uint8_t vfd_buff[64];

#ifdef PT6324
uint8_t charTable[][3] = {
    {0,0,0}, // Spase
    {0x66, 0x0, 0x2}, // !
    {0x81, 0x0, 0x0}, // "
    {0x1, 0x9, 0x2}, // #
    {0xf9, 0x70, 0x2}, // $
    {0xbd, 0xd0, 0x2}, // %
    {0x18, 0x0, 0x0}, // &
    {0x0, 0x0, 0x4}, // '
    {0x41, 0x40, 0x2}, // (
    {0x21, 0x20, 0x2}, // )
    {0x6, 0x80, 0x1}, // *
    {0x19, 0x0, 0x2}, // +
    {0x0, 0x6, 0x0}, // ,
    {0x18, 0x0, 0x0}, // -
    {0x0, 0x4, 0x0}, // .
    {0x4, 0x80, 0x0}, // /
    {0xe0, 0x79, 0x0}, // 0
    {0x4, 0x11, 0x0}, // 1
    {0x78, 0x69, 0x0}, // 2
    {0x70, 0x71, 0x0}, // 3
    {0x98, 0x11, 0x0}, // 4
    {0xe8, 0x60, 0x1}, // 5
    {0xf8, 0x78, 0x0}, // 6
    {0x60, 0x11, 0x0}, // 7
    {0xf8, 0x79, 0x0}, // 8
    {0xf8, 0x71, 0x0}, // 9
    {0x1, 0x0, 0x2}, // :
    {0x1, 0x80, 0x0}, // ;
    {0xc, 0x0, 0x1}, // <
    {0x18, 0x60, 0x0}, // =
    {0x12, 0x80, 0x0}, // >
    {0x70, 0x5, 0x2}, // ?
    {0xf1, 0x69, 0x0}, // @
    {0xf8, 0x19, 0x0}, // A
    {0x79, 0x71, 0x2}, // B
    {0xe0, 0x68, 0x0}, // C
    {0x61, 0x71, 0x2}, // D
    {0xe8, 0x68, 0x0}, // E
    {0xe8, 0x8, 0x0},  // F
    {0xf0, 0x78, 0x0}, // G
    {0x98, 0x19, 0x0}, // H
    {0x61, 0x60, 0x2}, // I
    {0x0, 0x79, 0x0}, // J
    {0x8c, 0x8, 0x1}, // K
    {0x80, 0x68, 0x0}, // L
    {0x86, 0x19, 0x0}, // M
    {0x82, 0x19, 0x1}, // N
    {0xe0, 0x79, 0x0}, // O
    {0xf8, 0x9, 0x0}, // P
    {0xe0, 0x79, 0x1}, // Q
    {0xf8, 0x9, 0x1}, // R
    {0xf8, 0x70, 0x0}, // S
    {0x61, 0x0, 0x2}, // T
    {0x80, 0x79, 0x0}, // U
    {0x84, 0x88, 0x0}, // V
    {0x80, 0x99, 0x1}, // W
    {0x6, 0x80, 0x1}, // X
    {0x98, 0x71, 0x0}, // Y
    {0x64, 0xe0, 0x0}, // Z
    {0x41, 0x40, 0x2}, // [
    {0x2, 0x0, 0x1}, // "\"
    {0x21, 0x20, 0x2}, // ]
    {0x0, 0x80, 0x1}, // ^
    {0x0, 0x60, 0x0}, // _
    {0x2, 0x0, 0x0}, // `
    {0x8, 0x68, 0x2}, // a
    {0x88, 0x28, 0x2}, // b
    {0x8, 0x28, 0x0}, // c
    {0x10, 0x51, 0x2}, // d
    {0x8, 0xa8, 0x0}, // e
    {0x59, 0x0, 0x2}, // f
    {0xa9, 0x20, 0x2}, // g
    {0x88, 0x8, 0x2}, // h
    {0x0, 0x0, 0x2}, // i
    {0x1, 0x28, 0x2}, // j
    {0x5, 0x0, 0x3},   // k
    {0x80, 0x8, 0x0},   // l
    {0x18, 0x18, 0x2},   // m
    {0x8, 0x8, 0x2},   // n
    {0x8, 0x28, 0x2},   // o
    {0xa9, 0x8, 0x0},   // p
    {0xa9, 0x0, 0x2},   // q
    {0x8, 0x8, 0x0},   // r
    {0xa8, 0x20, 0x2},   // s
    {0x88, 0x28, 0x0},   // t
    {0x0, 0x28, 0x2},   // u
    {0x0, 0x88, 0x0},   // v
    {0x0, 0x98, 0x1},   // w
    {0x6, 0x80, 0x1},   // x
    {0x11, 0x51, 0x0},   // y
    {0x8, 0xa0, 0x0},   // z
    {0x49, 0x40, 0x2},   // {
    {0x1, 0x0, 0x2},   // |
    {0x31, 0x20, 0x2},   // }
    {0x1c, 0x80, 0x0}     // ~

};
#endif

#ifdef PT6315
uint8_t charTable[][3] = {
    {0b11000100, 0b10001100, 0b00000000}, // 0
    {0b01000000, 0b00000100, 0b00000000}, // 1
    {0b01000100, 0b10001011, 0b00000000}, // 2
    {0b01000100, 0b10000111, 0b00000000}, // 3
    {0b11000000, 0b00000111, 0b00000000}, // 4
    {0b10000100, 0b10000111, 0b00000000}, // 5
    {0b10000100, 0b10001111, 0b00000000}, // 6
    {0b01000100, 0b00000100, 0b00000000}, // 7
    {0b11000100, 0b10001111, 0b00000000}, // 8
    {0b11000100, 0b10000111, 0b00000000}, // 9
    {0b00000010, 0b00000000, 0b00000000}, // :
    {0b00000000, 0b00000000, 0b00000000}, // ;
    {0b00000000, 0b00000000, 0b00000000}, // <
    {0b00000000, 0b00000000, 0b00000000}, // =
    {0b00000000, 0b00000000, 0b00000000}, // >
    {0b00000000, 0b00000000, 0b00000000}, // ?
    {0b00000000, 0b00000000, 0b00000000}, // @
    {0b11000100, 0b00001111, 0b00000000}, // A
    {0b10000000, 0b10001111, 0b00000000}, // B
    {0b10000100, 0b10001000, 0b00000000}, // C
    {0b01000000, 0b10001111, 0b00000000}, // D
    {0b10000100, 0b10001011, 0b00000000}, // E
    {0b10000100, 0b00001011, 0b00000000}, // F
};
#endif

// Якщо ви використовуєте стандартний SPI на ESP8266 (MOSI:GPIO13, MISO:GPIO12, SCK:GPIO14)
// вам не потрібно явно вказувати піни для SPI.begin()
// VFD_STDBY_PIN буде використовуватися як Chip Select (CS)

void pt63Init(void) {
    pinMode(VFD_STDBY_PIN, OUTPUT);
    digitalWrite(VFD_STDBY_PIN, HIGH); // CS high (неактивний)

    SPI.begin();
    SPI.setBitOrder(LSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV8); // 40MHz / 8 = 5MHz

    memset(vfd_buff, 0, sizeof(vfd_buff));

    writeCmd(0b10000000); // Display OFF
    delay(100);

    writeCmd(0b10001111); // Command 1 (Display ON)

    memset(vfd_buff, 0, sizeof(vfd_buff));
    writeData(vfd_buff, 49);

    pt63SetBrightness(MAX_BRIGHTNESS);
}

void pt63SetBrightness(uint8_t brightness) {
    brightness = (brightness <= MAX_BRIGHTNESS) ? brightness : MAX_BRIGHTNESS;
    writeCmd(brightness | 0x88);
}

void writeCmd(uint8_t cmd) {
    SPI.beginTransaction(SPISettings(5000000, LSBFIRST, SPI_MODE0));
    digitalWrite(VFD_STDBY_PIN, LOW); // CS Low (активація)
    SPI.transfer(cmd);
    digitalWrite(VFD_STDBY_PIN, HIGH); // CS High (деактивація)
    SPI.endTransaction();
}

void writeData(uint8_t *data, uint8_t len) {
    SPI.beginTransaction(SPISettings(5000000, LSBFIRST, SPI_MODE0));
    digitalWrite(VFD_STDBY_PIN, LOW); // CS Low (активація)
    for (uint8_t n = 0; n < len; n++) {
        SPI.transfer(data[n]);
    }
    digitalWrite(VFD_STDBY_PIN, HIGH); // CS High (деактивація)
    SPI.endTransaction();
}

void writeChars(uint8_t *data, uint8_t len) {
    #ifdef PT6315
    uint8_t n, tmp, chrIndex;
    uint8_t dot;
    chrIndex = 0;
    for (n = 0; n < len; n++) {
        if (data[chrIndex] == ' ') {
            tmp = 10;
        } else if (data[chrIndex] >= '0' && data[chrIndex] <= '9') {
            tmp = data[chrIndex] - '0';
        } else if (data[chrIndex] >= 'A' && data[chrIndex] <= 'F') {
            tmp = data[chrIndex] - 'A' + 17;
        } else {
            tmp = 0;
        }
        dot = 0;

        writeCmd(0x40);
        writeCmd(0xC0 | (n * 3));
        vfd_buff[0] = charTable[tmp][0] + dot;
        vfd_buff[1] = charTable[tmp][1];
        vfd_buff[2] = charTable[tmp][2];
        writeData(vfd_buff, 3);
        chrIndex++;
    }
    #endif

    #ifdef PT6324
    uint8_t n, tmp, chrIndex;
    chrIndex = 0;
    writeCmd(0b00001111);
    writeCmd(0b10001111);
    for(n=16; n>0; n--){
        if(data[chrIndex] < 32){
            tmp = 0;
        }else{
            tmp = data[chrIndex] - 32;
        }
        
        // Перевіряємо чи наступний символ - крапка або кома
        uint8_t dotComma = 0;
        if(chrIndex + 1 < len) {
            if(data[chrIndex + 1] == '.') {
                dotComma = 0x4; // Біт крапки
                chrIndex++; // Пропускаємо крапку на наступній ітерації
            } else if(data[chrIndex + 1] == ',') {
                dotComma = 0x6; // Біт коми
                chrIndex++; // Пропускаємо кому на наступній ітерації
            }
        }
        
        writeCmd(0x40);
        vfd_buff[0] = 0xc0 | ((n-1)*3);
        vfd_buff[1] = charTable[tmp][0];
        vfd_buff[2] = charTable[tmp][1] | dotComma; // Додаємо крапку/кому до другого байта
        vfd_buff[3] = charTable[tmp][2];
        writeData((uint8_t *)&vfd_buff, 4);
        chrIndex++;
    }
    #endif
}