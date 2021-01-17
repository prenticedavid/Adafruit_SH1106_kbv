// (C)2019 Pawel A. Hernik

/*
 128x64 ST7920 connections in SPI mode (only 6 wires between LCD and MCU):

 #01 GND  -> GND
 #02 VCC  -> VCC (5V)
 #04 RS   -> D10/CS or any pin
 #05 R/W  -> D11/MOSI
 #06 E    -> D13/SCK
 #15 PSB  -> GND (for SPI mode)
 #19 BLA  -> D9, VCC or any pin via 300ohm resistor
 #20 BLK  -> GND
*/

#define USE_LCD 0x1106   //select header and constructor

#define LCD_BACKLIGHT  9
#define LCD_CS         10 //10

#include <SPI.h>
#include <Wire.h>

#if USE_LCD == 0x0108
#include <Adafruit_KS0108_kbv.h>
Adafruit_KS0108_kbv display;
#define LCD_BEGIN()    display.begin()
#elif USE_LCD == 0x1106
#include <Adafruit_SH1106_kbv.h>
Adafruit_SH1106_kbv display(128, 64, &Wire);
#define LCD_BEGIN()    display.begin(SSD1306_SWITCHCAPVCC, 0x3C)
#define KS0108_WHITE   WHITE
#define KS0108_BLACK   BLACK
#define KS0108_INVERSE INVERSE
#elif USE_LCD == 0x1306
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire);
#define LCD_BEGIN()    display.begin(SSD1306_SWITCHCAPVCC, 0x3C)
#define KS0108_WHITE   WHITE
#define KS0108_BLACK   BLACK
#define KS0108_INVERSE INVERSE
#elif USE_LCD == 0x7920
#include "Adafruit_ST7920_kbv.h" //local
Adafruit_ST7920_kbv display(LCD_CS);
#define LCD_BEGIN()    display.begin()
#define KS0108_WHITE   WHITE
#define KS0108_BLACK   BLACK
#define KS0108_INVERSE INVERSE
#elif USE_LCD == 0xE7920
#include <ST7920_GFX_Library.h>
ST7920 display(10);
#define LCD_BEGIN()    display.begin()
#define KS0108_WHITE   WHITE
#define KS0108_BLACK   BLACK
#define KS0108_INVERSE 2
#endif


// from PropFonts library
//#include "c64enh_font.h"

char buf[20];

void setup()
{
    Serial.begin(9600);
    pinMode(LCD_BACKLIGHT, OUTPUT);
    //analogWrite(LCD_BACKLIGHT,30);
    digitalWrite(LCD_BACKLIGHT, HIGH);
    SPI.begin();
    //display.begin();
    LCD_BEGIN();
    //display.invertDisplay(0);
    display.setTextColor(WHITE);
    some_timings();
}

float t = 0;

void animSinLines()
{
    cls();
    float x;
    for (int i = 0; i < 64; i++) {
        x = 20.0 * (2.0 + sin(t / 10.0 + i / 18.0) * sin(i / 9.0));
        drawLineHfastD(64 - x, i, x * 2, WHITE);
        x = 10.0 * (2.0 + sin(t / 8.0 + i / 15.0) * sin(i / 5.0));
        display.drawFastHLine(64 - x, i, x * 2 + 1, WHITE);
    }
    t += 0.7;
    display.display();
}

char *banner = "GLCD Library";

void animTextSin()
{
    cls();
    //display.setFont(c64enh);
    char *txt = banner;
    int x = 4, i = 0;
    float y;
    while (*txt) {
        y = 20 + 10.0 * (1.0 + sin(t / 10.0 + i / 6.0) * sin(t / 18.0 + i / 9.0));
        display.setCursor(x, (int)y);
        display.print(*txt++);
        x += display.getCursorX() - x;
        i++;
    }
    t += 0.7;
    display.display();
}

void animTextCircle()
{
    cls();
    //display.setFont(c64enh);
    char *txt = banner;
    int x = 4;
    float y, y2, xr;
    float r = 10.0 + 6.0 * (1.0 + sin(t / 28.0));
    float xo = 38.0 * (0.0 + sin(t / 20.0));
    float yt = 24;
    while (*txt) {
        xr = 2 + x - 64 - xo;
        y2 = (r + 3) * (r + 3) - (xr) * (xr);
        y = yt;
        if (y2 > 0) y = 32 - sqrt(y2) - 5;
        if (y < 0) y = 0;
        if (y > yt) y = yt;
        display.setCursor(x, (int)y);
        display.print(*txt++);
        x += display.getCursorX() - x;
    }

    fillCircleD(64 + xo, 32, r, WHITE);
    //display.drawCircle(64+xo,32,r+1,WHITE);
    t += 0.4;
    display.display();
}

void rects()
{
    int x, y;
    x = random(128);
    y = random(40);
    setDither(random(12));
    display.fillRect(x, y, 30, 30, BLACK);
    fillRectD(x, y, 30, 30, 1);
    display.drawRect(x, y, 30, 30, WHITE);
    display.display();
    //delay(100);
}

void circles()
{
    int x, y;
    x = 15 + random(128 - 30);
    y = 15 + random(64 - 30);
    setDither(random(12));
    display.fillCircle(x, y, 15, BLACK);
    fillCircleD(x, y, 15, WHITE);
    display.drawCircle(x, y, 16, WHITE);
    display.display();
    //delay(100);
}

int x = 40, y = 0;
int dx = 6, dy = 5;
int x2 = 80, y2 = 40;
int dx2 = 9, dy2 = 8;

void animRect()
{
    x += dx;
    y += dy;
    if (x > 64 || x < 0) {
        dx = -dx;
        x += dx;
    }
    if (y > 32 || y < 0) {
        dy = -dy;
        y += dy;
    }
    display.fillRect(x, y, 64, 32, 2);  //INVERSE
    display.display();
    display.fillRect(x, y, 64, 32, 2);  //INVERSE
    delay(40);
}

#define MAX_LINES 10
byte lx0[MAX_LINES];
byte lx1[MAX_LINES];
byte ly0[MAX_LINES];
byte ly1[MAX_LINES];
byte curLine = 0;

void animLines()
{
    x += dx;
    y += dy;
    x2 += dx2;
    y2 += dy2;
    if (x > 127) {
        dx = -dx;
        x = 127;
    }
    if (x < 1) {
        dx = -dx;
        x = 0;
    }
    if (y > 63) {
        dy = -dy;
        y = 63;
    }
    if (y < 1) {
        dy = -dy;
        y = 0;
    }
    if (x2 > 127) {
        dx2 = -dx2;
        x2 = 127;
    }
    if (x2 < 1) {
        dx2 = -dx2;
        x2 = 0;
    }
    if (y2 > 63) {
        dy2 = -dy2;
        y2 = 63;
    }
    if (y2 < 1) {
        dy2 = -dy2;
        y2 = 0;
    }
    lx0[curLine] = x;
    lx1[curLine] = x2;
    ly0[curLine] = y;
    ly1[curLine] = y2;
    if (++curLine >= MAX_LINES) curLine = 0;
    cls();
    for (int i = 0; i < MAX_LINES; i++) {
        display.drawLine(lx0[i], ly0[i], lx1[i], ly1[i], WHITE);
        display.drawLine(127 - lx0[i], ly0[i], 127 - lx1[i], ly1[i], WHITE);
        display.drawLine(lx0[i], 63 - ly0[i], lx1[i], 63 - ly1[i], WHITE);
        display.drawLine(127 - lx0[i], 63 - ly0[i], 127 - lx1[i], 63 - ly1[i], WHITE);
    }
    display.display();
    delay(50);
}

unsigned long tm, demoTime = 14000;

void loop()
{
    tm = millis();
    cls();
    while (millis() - tm < demoTime) rects();

    tm = millis();
    cls();
    while (millis() - tm < demoTime) circles();

    setDither(8);
    tm = millis();
    while (millis() - tm < demoTime) animSinLines();

    tm = millis();
    while (millis() - tm < demoTime) animTextSin();

    tm = millis();
    while (millis() - tm < demoTime) animTextCircle();

    tm = millis();
    while (millis() - tm < demoTime) animLines();

    //display.setFont(c64enh);
    display.fillRect(0, 27, 128, 9, BLACK);
    display.setCursor((display.width() - strWidth(banner)) / 2, 28);
    display.print(banner);
    x = 40; y = 0;
    dx = 2; dy = 1;
    tm = millis();
    while (millis() - tm < demoTime) animRect();
}

//############################### Dither functions ###########################
static const byte ditherTab[4 * 17] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, // 0

    0x88, 0x00, 0x00, 0x00, // 1
    0x88, 0x00, 0x22, 0x00, // 2
    0x88, 0x00, 0xaa, 0x00, // 3
    0xaa, 0x00, 0xaa, 0x00, // 4
    0xaa, 0x44, 0xaa, 0x00, // 5
    0xaa, 0x44, 0xaa, 0x11, // 6
    0xaa, 0x44, 0xaa, 0x55, // 7

    0xaa, 0x55, 0xaa, 0x55, // 8

    0xaa, 0xdd, 0xaa, 0x55, // 9
    0xaa, 0xdd, 0xaa, 0x77, // 10
    0xaa, 0xdd, 0xaa, 0xff, // 11
    0xaa, 0xff, 0xaa, 0xff, // 12
    0xbb, 0xff, 0xaa, 0xff, // 13
    0xbb, 0xff, 0xee, 0xff, // 14
    0xbb, 0xff, 0xff, 0xff, // 15

    0xff, 0xff, 0xff, 0xff // 16
};

byte pattern[4];

void cls(void)
{
    display.fillRect(0, 0, display.width(), display.height(), BLACK);
}

int strWidth(char *txt) {
    int16_t pos, x1, y1;
    uint16_t len, w, h;
    display.getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);   //always
    len = x1 + w + 0;    // assumes the final right padding = 1.
    return len;
}

void fillRectD(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col)
{
    for (int y = y0; y < y0 + h; y++) { //y reads pat in MSB order
        uint8_t mask = 0x80 >> (y & 7); //(0,0) : 0xAA & 0x80. (0,1) : 0xAA & 0x40
        for (int x = x0; x < x0 + w; x++) {
            uint8_t pat = pattern[x & 3];
            if (pat & mask) display.drawPixel(x, y, col);
        }
    }
}

void drawLineHfastD(uint8_t x, uint8_t y, uint8_t w, uint8_t col)
{
    fillRectD(x, y, w, 1, col);
}

void fillCircleD(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col)
{
    drawLineHfastD(x0 - r, y0, 2 * r + 1, col);
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        drawLineHfastD(x0 - x, y0 + y, 2 * x + 1, col);
        drawLineHfastD(x0 - y, y0 + x, 2 * y + 1, col);
        drawLineHfastD(x0 - x, y0 - y, 2 * x + 1, col);
        drawLineHfastD(x0 - y, y0 - x, 2 * y + 1, col);
    }
}

void setDither(uint8_t s)
{
    if (s > 16) return;
    pattern[0] = pgm_read_byte(ditherTab + s * 4 + 0);
    pattern[1] = pgm_read_byte(ditherTab + s * 4 + 1);
    pattern[2] = pgm_read_byte(ditherTab + s * 4 + 2);
    pattern[3] = pgm_read_byte(ditherTab + s * 4 + 3);
}

// #######################  SOME TIMING ###########################

int32_t displaytime(void)
{
    uint32_t t = micros();
    display.display();
    return micros() - t;
}

int32_t clstime()
{
    uint32_t t = micros();
    cls();
    return micros() - t;
}

int32_t dithertime(void)
{
    uint32_t t = micros();
    setDither(8);
    fillRectD(0, 0, 128, 64, 1);
    for (int x = 0; x < display.width(); x += 16) {
        display.fillRect(x, 0, 8, 64, 2);
    }
    return micros() - t;
}

int32_t fastHtime(void)
{
    cls();
    display.display();  //resets the limits
    uint32_t t = micros();
    for (int y = 0; y < 64; y += 2) {
        display.drawFastHLine(0, y, 128, WHITE);
    }
    return micros() - t;
}

int32_t fastVtime(void)
{
    cls();
    display.display();
    uint32_t t = micros();
    for (int x = 0; x < 128; x += 2) {
        display.drawFastVLine(x, 0, 64, WHITE);
    }
    return micros() - t;
}

int32_t fillrecttime(uint8_t color)
{
    setDither(14);
    cls();
    fillRectD(0, 0, 128, 64, 1);
    display.display(); //resets display rectangle
    delay(1000);
    uint32_t t = micros();
    display.fillRect(0, 0, 128, 64, color);
    //lcd.clearDisplay(color);
    return micros() - t;
}

int32_t circletime(int x)
{
    uint32_t t = micros();
    display.drawCircle(x, 32, 16, 0); //no need to rub out filled
    fillCircleD(++x, 32, 16, 1);
    return micros() - t;
}

int32_t circlestime(bool flag)
{
    uint32_t t = 0;
    cls();
    setDither(8);
    for (int x = 0; x < 128; x++) {
        if (flag) cls();
        circletime(x);
        t += displaytime();
    }
    return t;
}

void show_timing(char *name, int32_t t1, int32_t t2)
{
    char buf[50];
    int frac = ((t1 + 500) / 100) % 10;
    int frac2 = ((t2 + 500) / 100) % 10;
    sprintf(buf, "%s = %ld.%dms, display(0) = %ld.%dms",
            name, t1 / 1000, frac, t2 / 1000, frac2);
    Serial.println(buf);
    delay(1000);
}

void some_timings(void)
{
    char buf[50];
    int32_t t1, t2;
    Serial.print("GFX_pattern: USE_LCD = 0x");
    Serial.println(USE_LCD, HEX);
    t1 = clstime(); t2 = displaytime();
    show_timing("cls()", t1, t2);
    t1 = dithertime(); t2 = displaytime();
    show_timing("dither(8)", t1, t2);
    t1 = fastHtime(); t2 = displaytime();
    show_timing("fastHtime()", t1, t2);
    t1 = fastVtime(); t2 = displaytime();
    show_timing("fastVtime()", t1, t2);
    t1 = circletime(64); t2 = circlestime(0);
    show_timing("circletime(64) [0]", t1, t2 / 128);
    t1 = circletime(64); t2 = circlestime(1);
    show_timing("circletime(64) [1]", t1, t2 / 128);
    t1 = fillrecttime(0); t2 = displaytime();
    show_timing("fillrecttime(0)", t1, t2);
    t1 = fillrecttime(1); t2 = displaytime();
    show_timing("fillrecttime(1)", t1, t2);
    t1 = fillrecttime(2); t2 = displaytime();
    show_timing("fillrecttime(2)", t1, t2);
    Serial.println("");
    delay(5000);
}
