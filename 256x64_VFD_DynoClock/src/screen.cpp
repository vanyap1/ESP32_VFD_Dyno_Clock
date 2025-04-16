//screen.c
//
#include <Arduino.h>
#include "customBoard.h"
#include <ArduinoJson.h>


#include "screen.h"
#include "FastLED.h"
#include <U8g2lib.h>

#include <SPI.h>


CRGB leds[LED_STRIP_NUM_LEDS];
SystemSetup sysConfig;



TaskHandle_t secondHwLoop = NULL;
extern tm timeinfo;
uint8_t lockConfigScreen = 0;
char extMessage[8][32];
int configScreenTime = 200;




U8G2_GP1247AI_253X63_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/VFD_CS, /* dc=*/U8X8_PIN_NONE, /* reset=*/VFD_RESET);
//U8G2_GP1294AI_256X48_F_4W_SW_SPI u8g2(U8G2_R0, VFD_SCK, VFD_MOSI, VFD_CS, U8X8_PIN_NONE, VFD_RESET);
//U8G2_GP1247AI_253X63_F_4W_SW_SPI u8g2(
//    U8G2_R0,   // Обертання дисплея
//    VFD_SCK,   // SCK (клок)
//    VFD_MOSI,  // MOSI (дані)
//    VFD_CS,    // CS (chip select)
//    VFD_RESET  // RESET
//);

const char* daysOfWeekShort[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char* monthNamesShort[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char* daysOfWeekFull[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* monthNamesFull[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};


// Бітмапи для Google Dino
const unsigned char dyno1[] = {
    0x00, 0xfc, 0x03, 0x00, 0xf6, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00,
    0x3e, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x1f, 0x00, 0x81, 0x1f, 0x00, 0xc3, 0x7f, 0x00, 0xe7, 0x5f,
    0x00, 0xff, 0x1f, 0x00, 0xff, 0x1f, 0x00, 0xff, 0x0f, 0x00, 0xfe, 0x0f, 0x00, 0xfc, 0x07, 0x00,
    0xf8, 0x03, 0x00, 0x70, 0x03, 0x00, 0x30, 0x02, 0x00, 0x10, 0x02, 0x00, 0x30, 0x06, 0x00
};

const unsigned char dyno3[] = {
    0x00, 0xfc, 0x03, 0x00, 0xf6, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00,
    0x3e, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x1f, 0x00, 0x81, 0x1f, 0x00, 0xc3, 0x7f, 0x00, 0xe7, 0x5f,
    0x00, 0xff, 0x1f, 0x00, 0xff, 0x1f, 0x00, 0xff, 0x0f, 0x00, 0xfe, 0x0f, 0x00, 0xfc, 0x07, 0x00,
    0xf8, 0x03, 0x00, 0x70, 0x03, 0x00, 0x30, 0x0e, 0x00, 0x10, 0x00, 0x00, 0x30, 0x00, 0x00
};

const unsigned char dyno4[] = {
    0x00, 0xfc, 0x03, 0x00, 0xf6, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00,
    0x3e, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x1f, 0x00, 0x81, 0x1f, 0x00, 0xc3, 0x7f, 0x00, 0xe7, 0x5f,
    0x00, 0xff, 0x1f, 0x00, 0xff, 0x1f, 0x00, 0xff, 0x0f, 0x00, 0xfe, 0x0f, 0x00, 0xfc, 0x07, 0x00,
    0xf8, 0x03, 0x00, 0x30, 0x03, 0x00, 0x60, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x06, 0x00
};

// Масив для анімації діно
const unsigned char* dynoallArray[3] = {
    dyno1,
    dyno3,
    dyno4
};

void pullMsg(uint8_t lockSrc, String msg, uint8_t line){
    lockConfigScreen = lockSrc;
    if (line < 8) {
        strncpy(extMessage[line], msg.c_str(), sizeof(extMessage[line]) - 1);
        extMessage[line][sizeof(extMessage[line]) - 1] = '\0'; 
    }
}
void clearScr(void){
    
}



void sysSetupUpdate(SystemSetup data){
    sysConfig = data;
}


void ScreenTaskCreate(void){
    Serial.println("ScreenTaskCreate");
    //esp_task_wdt_reset();
    pinMode(VFD_CS, OUTPUT);
    pinMode(VFD_FILAMENT, OUTPUT);
    pinMode(VFD_RESET, OUTPUT);

    pinMode(IO0, OUTPUT);
    
    
    SPI.begin(VFD_SCK, VFD_MISO, VFD_MOSI, -1);
    SPI.setFrequency(100000);
    digitalWrite(VFD_FILAMENT, HIGH);
    digitalWrite(VFD_RESET, HIGH);
    u8g2.begin();
    u8g2.initDisplay();
    u8g2.setFlipMode(3);
    u8g2.setPowerSave(0);
    u8g2.setContrast(180);
    
    u8g2.clearBuffer();                
    //u8g2.setFont(u8g2_font_ncenB08_tr); 
    //u8g2.drawStr(10, 30, "Hello, VFD!");
    u8g2.sendBuffer();

    xTaskCreatePinnedToCore(ScreenHandler, "ScreenHandler", 10000, NULL, 10, &secondHwLoop, 1);
}

void ScreenHandler(void *arg){
   
    
    #if defined(GRB_LED_TYPE)
        FastLED.addLeds<WS2811, LED_STRIP_PIN, RGB>(leds, LED_STRIP_NUM_LEDS).setCorrection( TypicalLEDStrip );
    #else
        FastLED.addLeds<WS2811, LED_STRIP_PIN, GRB>(leds, LED_STRIP_NUM_LEDS).setCorrection( TypicalLEDStrip );
    #endif
    
    
    FastLED.setBrightness(50);
    pinMode(LED_STRIP_PIN, OUTPUT);
    //leds[0] = CHSV(0, 0, 0);
    //leds[1] = CHSV(0, 0, 0);
    //leds[0] = CRGB(0, 0, 0);
    //leds[1] = CRGB(0, 0, 0);
    FastLED.show();
    
    
    
while(1){
    u8g2.clearBuffer();
    
    if(configScreenTime != 0){
        if(lockConfigScreen==0){    //Lock config screen in config mode
            configScreenTime--;
        }
        showConfigScreen();
    }else{
        switch (sysConfig.screenDemoMode)
        {
        case 1:
            drawSpaceTrasher();
            break;
        case 2:
            spaceRain();
            break;
        case 3:
            drawDinoChase();
            break;

        
        default:
            break;
        }

        dravUI();
    }

    u8g2.sendBuffer();
   
    if(sysConfig.ambLightEnable){
        for(int i = 0; i < LED_STRIP_NUM_LEDS; i++){
            leds[i] = CRGB(sysConfig.ambLightColr[0],sysConfig.ambLightColr[1],sysConfig.ambLightColr[2]);
        }   
    }else{
        for(int i = 0; i < LED_STRIP_NUM_LEDS; i++){
            leds[i] = CRGB(0,0,0);
        }   
    }
    FastLED.show();
    //digitalWrite(Server_LED, !digitalRead(Server_LED));
    delay(20);

}
}

void showConfigScreen(){
    const char* gitLink = "github.com/vanyap1/ESP32_VFD_Dyno_Clock";
    const char* deviceInfo = "VFD Dyno Clock v1.0";

    // Очищуємо екран
    u8g2.clearBuffer();

    // Малюємо рамку для вікна
    //u8g2.drawFrame(5, 5, 243, 53); // Рамка розміром 243x53 пікселі

    // Малюємо заголовок вікна
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(10, 10, "Device Info");

    // Малюємо горизонтальну лінію під заголовком
   // u8g2.drawHLine(6, 18, 241);

    // Виводимо IP-адресу
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(10, 25, "Status:");
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(55, 25, extMessage[0]);

    // Виводимо IP-адресу
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(10, 35, "IPAddr:");
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(55, 35, extMessage[1]);

    // Виводимо посилання на GitHub
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(10, 45, "GitHub:");
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.drawStr(55, 45, gitLink);

    // Виводимо інформацію про пристрій
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(10, 55, "Device:");
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(55, 55, deviceInfo);
    
}

void dravUI() {
    getLocalTime(&timeinfo);

    // Буфери для тексту
    char timeBuff[16];
    char dateBuff[32];
    char dayBuff[16];
    char footerBuff[64];

    // Масив текстових рядків
    const char* messages[] = {
        "Hello, Vanya!",
        "How are you?",
        "Today you have 5 commits on your GitHub.",
        "Tomorrow will be sunny and +19°C",
        "Don't forget to rest.",
        "Every day is a new opportunity.",
        "You can achieve anything you want!",
        "Code and be happy!",
        "Dream and achieve!",
        "Life is an adventure!"
    };

    static int currentMessageIndex = 0; // Індекс поточного рядка
    static int charIndex = 0;           // Індекс поточної букви
    static int frameCounter = 0;        // Лічильник кадрів
    static bool printing = true;        // Чи триває друк рядка

    // Форматуємо час
    sprintf(timeBuff, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    u8g2.setFont(u8g2_font_logisoso32_tf);
    u8g2.drawStr(10, 52, timeBuff);

    // Форматуємо день тижня
    sprintf(dayBuff, "%s", daysOfWeekFull[timeinfo.tm_wday]);
    u8g2.setFont(u8g2_font_helvB10_tr);
    u8g2.drawStr(10, 14, dayBuff);

    // Форматуємо дату
    sprintf(dateBuff, "%02d %s %04d", timeinfo.tm_mday, monthNamesFull[timeinfo.tm_mon], 1900 + timeinfo.tm_year);
    u8g2.setFont(u8g2_font_helvB12_tf);
    u8g2.drawStr(140, 14, dateBuff);

    // Друк тексту для нижнього рядка
    if (printing) {
        // Друкуємо текст по одній букві
        strncpy(footerBuff, messages[currentMessageIndex], charIndex);
        footerBuff[charIndex] = '\0'; // Завершуємо рядок
        strcat(footerBuff, "_");     // Додаємо підкреслення під час друку
        charIndex++;

        // Перевіряємо, чи завершено друк рядка
        if (charIndex > strlen(messages[currentMessageIndex])) {
            printing = false; // Завершено друк
            charIndex = strlen(messages[currentMessageIndex]); // Фіксуємо довжину
        }
    } else {
        // Тримаємо рядок на екрані кілька викликів
        strncpy(footerBuff, messages[currentMessageIndex], charIndex);
        footerBuff[charIndex] = '\0'; // Завершуємо рядок

        frameCounter++;
        if (frameCounter > 50) { // Тримаємо рядок 50 викликів
            frameCounter = 0;
            charIndex = 0;
            printing = true; // Починаємо друк наступного рядка
            currentMessageIndex++;
            if (currentMessageIndex >= 10) {
                currentMessageIndex = 0; // Повертаємося до першого рядка
            }
        }
    }

    // Малюємо рамку
    u8g2.drawFrame(0, 0, 253, 63);

    // Малюємо горизонтальну лінію для розділення
    u8g2.drawHLine(0, 18, 253);

    // Встановлюємо шрифт для нижнього рядка
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(10, 60, footerBuff); // Виводимо текст у нижньому рядку



}


void spaceRain() {
    static int trashX[5] = {20, 50, 80, 120, 200}; // Початкові X-координати об'єктів
    static int trashY[5] = {0, -10, -20, -30, -40}; // Початкові Y-координати об'єктів
    static int trashSpeed[5] = {2, 3, 1, 4, 2}; // Швидкість кожного об'єкта (вертикальна)
    static int trashType[5] = {0, 1, 2, 0, 1}; // Типи об'єктів (0 - квадрат, 1 - зірка, 2 - астероїд)
    static int trashXSpeed[5] = {1, -1, 0, 1, -1}; // Горизонтальна швидкість (вліво або вправо)

    // Малюємо кожен об'єкт
    for (int i = 0; i < 5; i++) {
        // Вибираємо тип об'єкта
        switch (trashType[i]) {
            case 0: // Квадрат
                u8g2.drawBox(trashX[i], trashY[i], 5, 5);
                break;
            case 1: // Зірка
                u8g2.drawPixel(trashX[i], trashY[i]);
                u8g2.drawPixel(trashX[i] - 1, trashY[i]);
                u8g2.drawPixel(trashX[i] + 1, trashY[i]);
                u8g2.drawPixel(trashX[i], trashY[i] - 1);
                u8g2.drawPixel(trashX[i], trashY[i] + 1);
                break;
            case 2: // Астероїд (умовна форма)
                u8g2.drawCircle(trashX[i], trashY[i], 3, U8G2_DRAW_ALL);
                break;
        }

        // Оновлюємо координати
        trashY[i] += trashSpeed[i]; // Рух вниз
        trashX[i] += trashXSpeed[i]; // Рух вліво або вправо

        // Якщо об'єкт виходить за межі екрана, повертаємо його наверх
        if (trashY[i] > 64 || trashX[i] < 0 || trashX[i] > 256) {
            trashY[i] = -5; // Повертаємо об'єкт наверх
            trashX[i] = random(0, 256); // Випадкова нова X-координата
            trashSpeed[i] = random(1, 5); // Випадкова нова вертикальна швидкість
            trashXSpeed[i] = random(-1, 2); // Випадкова нова горизонтальна швидкість (-1, 0, 1)
            trashType[i] = random(0, 3); // Випадковий новий тип об'єкта
        }
    }
}

void drawSpaceTrasher() {
    static int ship_x = 10, ship_y = 30; // Позиція корабля
    static int direction = 1; // Напрямок руху корабля

    // Астероїди
    #define AST_COUNT 6
    static int ast_x[AST_COUNT], ast_y[AST_COUNT], ast_speed[AST_COUNT];
    static bool ast_initialized = false;

    // Кулі
    #define BULLET_COUNT 10
    static int bullet_x[BULLET_COUNT], bullet_y[BULLET_COUNT];
    static bool bullet_active[BULLET_COUNT];

    // Частинки вибуху
    #define PARTICLE_COUNT 10
    static int part_x[PARTICLE_COUNT], part_y[PARTICLE_COUNT], part_dx[PARTICLE_COUNT], part_dy[PARTICLE_COUNT];
    static bool part_active[PARTICLE_COUNT];

    // Ініціалізація астероїдів
    if (!ast_initialized) {
        for (int i = 0; i < AST_COUNT; i++) {
            ast_x[i] = 256 + (rand() % 50);
            ast_y[i] = rand() % 64;
            ast_speed[i] = (rand() % 3) + 1;
        }
        ast_initialized = true;
    }

    // Рух корабля
    ship_y += direction;
    if (ship_y >= 55 || ship_y <= 5) direction = -direction;

    // Рух астероїдів
    for (int i = 0; i < AST_COUNT; i++) {
        ast_x[i] -= ast_speed[i];
        if (ast_x[i] < 0) {
            ast_x[i] = 256 + (rand() % 50);
            ast_y[i] = rand() % 64;
            ast_speed[i] = (rand() % 3) + 1;
        }
    }

    // Рух куль
    for (int i = 0; i < BULLET_COUNT; i++) {
        if (bullet_active[i]) {
            bullet_x[i] += 3;
            if (bullet_x[i] > 256) bullet_active[i] = false;
        }
    }

    // Рух частинок вибуху
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        if (part_active[i]) {
            part_x[i] += part_dx[i];
            part_y[i] += part_dy[i];
            if (part_x[i] < 0 || part_x[i] > 256 || part_y[i] < 0 || part_y[i] > 64) {
                part_active[i] = false;
            }
        }
    }

    // Перевірка зіткнень між кулями та астероїдами
    for (int i = 0; i < BULLET_COUNT; i++) {
        if (bullet_active[i]) {
            for (int j = 0; j < AST_COUNT; j++) {
                if (bullet_x[i] >= ast_x[j] - 3 && bullet_x[i] <= ast_x[j] + 3 &&
                    bullet_y[i] >= ast_y[j] - 3 && bullet_y[i] <= ast_y[j] + 3) {
                    bullet_active[i] = false; // Деактивація кулі
                    // Вибух астероїда
                    for (int k = 0; k < PARTICLE_COUNT; k++) {
                        part_x[k] = ast_x[j];
                        part_y[k] = ast_y[j];
                        part_dx[k] = (rand() % 5) - 2;
                        part_dy[k] = (rand() % 5) - 2;
                        part_active[k] = true;
                    }
                    // Перезапуск астероїда
                    ast_x[j] = 256 + (rand() % 50);
                    ast_y[j] = rand() % 64;
                    ast_speed[j] = (rand() % 3) + 1;
                }
            }
        }
    }

    // Малюємо корабель
    u8g2.drawTriangle(ship_x, ship_y, ship_x - 5, ship_y + 5, ship_x - 5, ship_y - 5);

    // Малюємо астероїди
    for (int i = 0; i < AST_COUNT; i++) {
        u8g2.drawCircle(ast_x[i], ast_y[i], 3, U8G2_DRAW_ALL);
    }

    // Малюємо кулі
    for (int i = 0; i < BULLET_COUNT; i++) {
        if (bullet_active[i]) {
            u8g2.drawBox(bullet_x[i], bullet_y[i], 2, 2);
        }
    }

    // Малюємо частинки вибуху
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        if (part_active[i]) {
            u8g2.drawPixel(part_x[i], part_y[i]);
        }
    }

    // Стрільба (автоматична для тестування)
    static int shootDelay = 0;
    shootDelay++;
    if (shootDelay > 20) { // Стріляємо кожні 20 кадрів
        for (int i = 0; i < BULLET_COUNT; i++) {
            if (!bullet_active[i]) {
                bullet_x[i] = ship_x + 5;
                bullet_y[i] = ship_y;
                bullet_active[i] = true;
                break;
            }
        }
        shootDelay = 0;
    }


}

void drawDinoChase() {
    // Позиція діно
    static int dinoX = 10, dinoY = 30;
    static int dinoFrame = 0; // Кадр для анімації
    static int dinoDirectionX = 1, dinoDirectionY = 1;

    // Кількість кругів
    #define CIRCLE_COUNT 5
    static int circleX[CIRCLE_COUNT], circleY[CIRCLE_COUNT];
    static bool circleActive[CIRCLE_COUNT];
    static bool initialized = false;

    // Ініціалізація кругів
    if (!initialized) {
        for (int i = 0; i < CIRCLE_COUNT; i++) {
            circleX[i] = random(20, 240); // Випадкова позиція по X
            circleY[i] = random(10, 54); // Випадкова позиція по Y
            circleActive[i] = true;      // Круг активний
        }
        initialized = true;
    }

    // Оновлення позиції діно
    dinoX += dinoDirectionX;
    dinoY += dinoDirectionY;

    // Зміна напрямку діно при досягненні меж екрану
    if (dinoX <= 0 || dinoX >= 240) dinoDirectionX = -dinoDirectionX;
    if (dinoY <= 0 || dinoY >= 54) dinoDirectionY = -dinoDirectionY;

    // Перевірка колізій між діно та кругами
    for (int i = 0; i < CIRCLE_COUNT; i++) {
        if (circleActive[i] &&
            dinoX >= circleX[i] - 5 && dinoX <= circleX[i] + 5 &&
            dinoY >= circleY[i] - 5 && dinoY <= circleY[i] + 5) {
            // Круг зникає при колізії
            circleActive[i] = false;

            // З'являється новий круг у випадковій позиції
            circleX[i] = random(20, 240);
            circleY[i] = random(10, 54);
            circleActive[i] = true;
        }
    }
    // Малюємо діно
    u8g2.drawXBMP(dinoX, dinoY, 18, 21, dynoallArray[dinoFrame]);

    // Оновлення кадру анімації діно
    dinoFrame++;
    if (dinoFrame >= 3) dinoFrame = 0;

    // Малюємо круги
    for (int i = 0; i < CIRCLE_COUNT; i++) {
        if (circleActive[i]) {
            u8g2.drawCircle(circleX[i], circleY[i], 5, U8G2_DRAW_ALL);
        }
    }
}