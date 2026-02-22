

#ifndef PT63XX_H
#define PT63XX_H    
#include <Arduino.h>


  
#define IV18_VFD_LATCH_DELAY_US 250
#define IV18_VFD_SPI_SPEED 500000  // 500 kHz

#define MAX_BRIGHTNESS 7
#define INIT_BRIGHTNESS 1
#define COMMAND_1 0b00000000
#define COMMAND_2 0b01000000
#define COMMAND_3 0b11000000
#define COMMAND_4 0b10000000

//parameters COMMAND_1
#define DIG4_SEG24 0b0000
#define DIG5_SEG23 0b0001
#define DIG6_SEG22 0b0010
#define DIG7_SEG21 0b0011
#define DIG8_SEG20 0b0100
#define DIG9_SEG19 0b0101
#define DIG10_SEG18 0b0110
#define DIG11_SEG17 0b0111
#define DIG12_SEG16 0b1111
//parameters COMMAND_2
#define NORMAL_OPERATION_MODE 0b0000
#define TEST_MODE 0b1000
#define INCREMENT_ADDRESS 0b000
#define FIXED_ADDRESS 0b100
#define WRITE_DATA_TO_DISPLAY_MODE 0b00
#define WRITE_DATA_TO_LED_PORT 0b01
#define READ_KEY_DATA 0b10
#define READ_SW_DATA 0b11
//parameters COMMAND_4
#define DISPLAY_ON 0b1000
#define DISPLAY_OFF 0b0000

enum SCREEN_TYPE {
  IV18 = 0,
  IVL2_7
};

enum IV18_SPECIAL_CHARS {
  //Symbol bit mask:
  IV18_CHAR_DOT = 0x40,
  IV18_CHAR_MINUS = 0x10,
};

extern uint32_t customCharData[96];

class PT63XX {

  public:
    PT63XX(int latchPin, SCREEN_TYPE type = IV18);
    void begin();
    void screen_init();
    void sendCommand(uint8_t command);
    void sendData(uint8_t *data, size_t length);
    void setBrightness(uint8_t brightness);
    void displayOff();
    void displayOn();
    void setBlinkCharData(uint32_t charMask, uint8_t position);
    void blinkState(uint8_t state);
    void clearDisplay();
    void writeChar(uint8_t position, uint8_t character);
    void writeRawData(uint8_t position, uint32_t data);
    void writeStringUniverslaChrTab(const char* str, uint8_t position);
    void setCharToCustomTable(uint8_t index, uint32_t segments);
    void writeString(const char* str, uint8_t position);
    void writeSpecialCharPlase(IV18_SPECIAL_CHARS charName, bool state);
  private:
    int _latchPin;
    SCREEN_TYPE _screenType;
    //SPI ob
};

#endif
