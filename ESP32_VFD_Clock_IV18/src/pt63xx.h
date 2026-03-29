

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

//parameters COMMAND_1 (PT6315)
#define PT6315_DIG4_SEG24 0b0000
#define PT6315_DIG5_SEG23 0b0001
#define PT6315_DIG6_SEG22 0b0010
#define PT6315_DIG7_SEG21 0b0011
#define PT6315_DIG8_SEG20 0b0100
#define PT6315_DIG9_SEG19 0b0101
#define PT6315_DIG10_SEG18 0b0110
#define PT6315_DIG11_SEG17 0b0111
#define PT6315_DIG12_SEG16 0b1000  // Any value 1XXX (8-15)

//parameters COMMAND_1 (PT6311)
#define PT6311_DIG8_SEG20 0b0000   // Any value 0XXX (0-7)
#define PT6311_DIG9_SEG19 0b1000
#define PT6311_DIG10_SEG18 0b1001
#define PT6311_DIG11_SEG17 0b1010
#define PT6311_DIG12_SEG16 0b1011
#define PT6311_DIG13_SEG15 0b1100
#define PT6311_DIG14_SEG14 0b1101
#define PT6311_DIG15_SEG13 0b1110
#define PT6311_DIG16_SEG12 0b1111

//parameters COMMAND_1 (PT6324)
#define PT6324_DIG8_SEG24 0b0000
#define PT6324_DIG9_SEG24 0b1000
#define PT6324_DIG10_SEG24 0b1001
#define PT6324_DIG11_SEG24 0b1010
#define PT6324_DIG12_SEG24 0b1011
#define PT6324_DIG13_SEG24 0b1100
#define PT6324_DIG14_SEG24 0b1101
#define PT6324_DIG15_SEG24 0b1110
#define PT6324_DIG16_SEG24 0b1111

// Legacy compatibility
#define DIG12_SEG16 PT6315_DIG12_SEG16
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
    void begin(uint8_t screenDriver);
    void screen_init();
    void sendCommand(uint8_t command);
    void sendData(uint8_t *data, size_t length);
    void setBrightness(uint8_t brightness);
    void displayOff();
    void displayOn();
    void setBlinkCharData(uint32_t charMask, uint8_t position);
    void blinkState(uint8_t state, uint8_t enabled);
    void clearDisplay();
    void writeChar(uint8_t position, uint8_t character);
    void writeRawData(uint8_t position, uint32_t data);
    void writeStringUniverslaChrTab(const char* str, uint8_t position);
    void setCharToCustomTable(uint8_t index, uint32_t segments);
    void writeString(const char* str, uint8_t position);
    void writeSpecialCharPlase(IV18_SPECIAL_CHARS charName, bool state);
    void setScreenDriver(uint8_t driver);
    void setScreenDirection(bool reversed);
  private:
    int _latchPin;
    SCREEN_TYPE _screenType;
    uint8_t _screenDriver;
    uint8_t _maxDigits;
    uint8_t _positionMap[16];
    //SPI ob
};

#endif
