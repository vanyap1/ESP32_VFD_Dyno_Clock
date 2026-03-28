#include "pt63xx.h"
#include "SPI.h"



uint8_t vfd_buff[36];
uint8_t specialCharData = 0;
uint32_t blinkCharMask = 0;
uint8_t  blinkCharPosition = -1;
uint8_t blinkCharState = 0;
uint8_t blinkCharEnabled = 0;
uint32_t customCharData[96] = {0};

uint8_t IVL2_7_characters[] = {
  0x00, // Space
  0xC0, // !
  0xC0, // "
  0x00, // #
  0x00, // $
  0x00, // %
  0x00, // &
  0xC0, // '
  0x00, // (
  0x00, // )
  0x00, // *
  0x00, // +
  0x80, // ,
  0x40, // -
  0x80, // .
  0x00, // Slash
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F, // 9
  0xC0, // :
  0xC0, // ;
  0x00, // <
  0x00, // =
  0x00, // >  
  0x00, // ?
  0x77, // A
  0x7C, // B
  0x39, // C
  0x5E, // D
  0x79, // E
  0x71, // F
  0x00, // G
  0x00, // H
  0x24, // I
  0x0F, // J
  0x00, // K
  0x2B, // L
  0x00, // M
  0x00, // N
  0xAF, // O
  0x97, // P
  0x00, // Q
  0x00, // R
  0xBA, // S
  0x00, // T
  0x2F, // U
  0x00, // V
  0x00, // W
  0x00, // X
  0x00, // Y
  0x97  // Z
  
};

uint8_t IV18_characters[] = {
  0x00, // Space
  0x04, // !
  0x22, // "
  0x00, // #
  0x00, // $
  0x00, // %
  0x00, // &
  0x02, // '
  0x00, // (
  0x00, // )
  0x00, // *
  0x00, // +
  0x40, // ,
  0x10, // -
  0x40, // .
  0x00, // Slash
  0xaf, // 0
  0x24, // 1
  0x97, // 2
  0xB6, // 3
  0x3C, // 4
  0xBA, // 5
  0xBB, // 6
  0xA4, // 7
  0xBF, // 8
  0xBE, // 9
  0x00, // :
  0x00, // ;
  0x00, // <
  0x00, // =
  0x00, // >  
  0x00, // ?
  0xBD, // A
  0x3B, // B
  0x8B, // C
  0x37, // D
  0x9B, // E
  0x99, // F
  0x00, // G
  0x00, // H
  0x24, // I
  0x0F, // J
  0x00, // K
  0x2B, // L
  0x00, // M
  0x00, // N
  0xAF, // O
  0x97, // P
  0x00, // Q
  0x00, // R
  0xBA, // S
  0x00, // T
  0x2F, // U
  0x00, // V
  0x00, // W
  0x00, // X
  0x00, // Y
  0x97  // Z
  
};



PT63XX::PT63XX(int latchPin, SCREEN_TYPE type)
  : _latchPin(latchPin), _screenType(type), _screenDriver(DIG12_SEG16) {
    digitalWrite(_latchPin, HIGH);
    pinMode(_latchPin, OUTPUT);
  }



//Usage example
void screen_init() {
  PT63XX vfd(27); // CS pin must be defined
  vfd.begin();
  vfd.clearDisplay();
}



void PT63XX::begin() {
    digitalWrite(_latchPin, HIGH);
    pinMode(_latchPin, OUTPUT);
    
    #ifdef ESP32
        SPI.begin(5, 18, 19, -1);  // Custom SPI pins for ESP32
    #else
        SPI.begin();  // ESP8266 uses default hardware SPI pins
    #endif
    
    memset(vfd_buff, 0, sizeof(vfd_buff));
    delay(200);
    sendCommand(COMMAND_4 | DISPLAY_OFF); // Display OFF
    delay(10);

    sendCommand(COMMAND_2 | NORMAL_OPERATION_MODE | INCREMENT_ADDRESS | WRITE_DATA_TO_DISPLAY_MODE); // Command 2 (Normal operation, Increment address, Write data to display)
    delay(10);
    clearDisplay();
    
    sendCommand(COMMAND_1 | _screenDriver); //Display mode set
     
    delay(10);

    sendCommand(COMMAND_4 | DISPLAY_ON | INIT_BRIGHTNESS); // Command 4 (Display ON)
    setBrightness(MAX_BRIGHTNESS);  
}

void PT63XX::begin(uint8_t screenDriver) {
    _screenDriver = screenDriver;
    begin();
}

void PT63XX::setScreenDriver(uint8_t driver) {
    _screenDriver = driver;
    // Send command to update display mode
    sendCommand(COMMAND_1 | _screenDriver);
}

void PT63XX::setBlinkCharData(uint32_t charMask, uint8_t position){
  blinkCharMask = charMask;
  blinkCharPosition = position;
}

void PT63XX::blinkState(uint8_t state, uint8_t enabled){
  blinkCharState = state;
  blinkCharEnabled = enabled;
}

void PT63XX::clearDisplay() {
  sendCommand(0x40);
  memset(vfd_buff, 0, sizeof(vfd_buff));
    vfd_buff[0] = COMMAND_3; // Start address
    sendData(vfd_buff, sizeof(vfd_buff)); // Clear display
}

void PT63XX::setBrightness(uint8_t brightness) {
  if (brightness > MAX_BRIGHTNESS) {
    brightness = MAX_BRIGHTNESS;
  }
  sendCommand(0x88 | brightness); // Set brightness command
}

void PT63XX::displayOff() {
  sendCommand(COMMAND_4 | DISPLAY_OFF); // Display OFF
}
void PT63XX::displayOn() {
  sendCommand(COMMAND_4 | DISPLAY_ON | INIT_BRIGHTNESS); // Display ON
}

//write char to VFD
void PT63XX::writeChar(uint8_t position, uint8_t character) {
  if (position >= 12) {
    return; // Invalid position
  }
  vfd_buff[0] = COMMAND_3 + (position*3); // Set address
  vfd_buff[1] = character; // Character data
  sendData(&vfd_buff[0], 2); // Send command and data
}
void PT63XX::writeRawData(uint8_t position, uint32_t data){
  if (position >= 12) {
    return; // Invalid position
  }
  if (blinkCharPosition == position && blinkCharEnabled) {
    if (blinkCharState) {
      data = data | blinkCharMask;
    } else {
      data = data & ~blinkCharMask;
    } 
  }

  vfd_buff[0] = COMMAND_3 + (position*3); // Set address
  vfd_buff[1] = data & 0xFF; // Lower byte
  vfd_buff[2] = (data >> 8) & 0xFF; // Middle byte
  vfd_buff[3] = (data >> 16) & 0xFF; // Upper byte
  sendData(&vfd_buff[0], 4); // Send command and data
}

void PT63XX::writeStringUniverslaChrTab(const char* str, uint8_t position){
 uint8_t charsToWrite = strlen(str);
 uint8_t currentPos = position;
 uint32_t lastCharData = 0;
 bool hasLastChar = false;
 
 for(uint8_t i = 0; i < charsToWrite; i++){
   if(currentPos >= 16) break; // Prevent overflow
   uint8_t charCode = str[i];
   
   if(charCode == '.' || charCode == ',' || charCode == ':') {
     if (hasLastChar) {
       
       uint32_t dotSegments = customCharData[charCode - 32];
       lastCharData = lastCharData | dotSegments;
       writeRawData(currentPos-1, lastCharData);
     } else {

       lastCharData = customCharData[charCode - 32];
       writeRawData(currentPos, lastCharData);
       hasLastChar = true;
       currentPos++;
     }
     continue;
   }
   
   lastCharData = customCharData[charCode - 32];
   writeRawData(currentPos, lastCharData);
   hasLastChar = true;
   currentPos++;
 }
}

void PT63XX::setCharToCustomTable(uint8_t index, uint32_t segments){
  if(index < 96+32){
    customCharData[index-32] = segments;
    
    
  }
}


void PT63XX::writeString(const char* str, uint8_t position) {
  uint8_t currentPos = position;
  uint8_t lastCharData = 0;
  bool hasLastChar = false;
  if(_screenType == IV18){
    currentPos++; // Shift for IV18
  }


  while (*str && currentPos < 32) {
    uint8_t charCode = *str;
    switch (_screenType)
    {
    case IV18:
     
      charCode = (charCode == ':') ? '-' : charCode;
      if(charCode == '.' || charCode == ',') {
        if (hasLastChar) {
          writeChar(currentPos - 1, lastCharData | 0x40);
          } else {
          lastCharData = 0x40;
          writeChar(currentPos, lastCharData);
          hasLastChar = true;
          currentPos++;
        }
        str++;
        continue;
      }
      lastCharData = IV18_characters[charCode - 32];
      break;
    case IVL2_7:
      
      lastCharData =IVL2_7_characters[charCode - 32];

      writeChar(currentPos, lastCharData); // Always turn on dots at position 4
      break;
    
    default:
      lastCharData = IV18_characters[charCode - 32];
      break;
    }
    writeChar(currentPos, lastCharData);
    hasLastChar = true;
    str++;
    currentPos++;
  }
}

void PT63XX::writeSpecialCharPlase(IV18_SPECIAL_CHARS charName, bool state){
  if(_screenType == IV18){

    if(state){
      specialCharData |= charName;
    } else {
      specialCharData &= ~charName;
    }
    writeChar(0, specialCharData);
  }
}
void PT63XX::sendCommand(uint8_t command) {
  digitalWrite(_latchPin, LOW);
  delayMicroseconds(IV18_VFD_LATCH_DELAY_US);
  vfd_buff[0] = command;
  SPI.beginTransaction(SPISettings(IV18_VFD_SPI_SPEED, LSBFIRST, SPI_MODE3));
  SPI.transferBytes(vfd_buff, nullptr, 1);
  SPI.endTransaction();
  delayMicroseconds(IV18_VFD_LATCH_DELAY_US);
  digitalWrite(_latchPin, HIGH);
}
void PT63XX::sendData(uint8_t *data, size_t length) {
  digitalWrite(_latchPin, LOW);
  delayMicroseconds(IV18_VFD_LATCH_DELAY_US);
  SPI.beginTransaction(SPISettings(IV18_VFD_SPI_SPEED, LSBFIRST, SPI_MODE3));
  SPI.transferBytes(data, nullptr, length);
  SPI.endTransaction();
  delayMicroseconds(IV18_VFD_LATCH_DELAY_US);
  digitalWrite(_latchPin, HIGH);
}


