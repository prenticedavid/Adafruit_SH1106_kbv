// Demo of KS0108B graphics LCD screen connected to DIYMORE 16-bit Adapter
// uses regular GFX methods
//
// defaults to DIYMORE Adapter wiring. i.e. plug KS0108 display into DIYMORE 20x1 header.
// #define USE_GAMMON_WIRING in diymore_min.cpp if you hand-wire the KS0108
//
// Gammon's library expects MCP23017 BANK0 i.e. POR state.  Toggle power before using Gasmmon. 
// Adafruit_KS0108_kbv uses BANK1.  start from BANK0 or BANK1.   POR is not necessary.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "bitmap_mono.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSerif12pt7b.h"

#define USE_LCD 0x1106

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
Adafruit_ST7920_kbv display(10);
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

#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{   B00000000, B11000000,
    B00000001, B11000000,
    B00000001, B11000000,
    B00000011, B11100000,
    B11110011, B11100000,
    B11111110, B11111000,
    B01111110, B11111111,
    B00110011, B10011111,
    B00011111, B11111100,
    B00001101, B01110000,
    B00011011, B10100000,
    B00111111, B11100000,
    B00111111, B11110000,
    B01111100, B11110000,
    B01110000, B01110000,
    B00000000, B00110000
};

void setup() {
    Serial.begin(9600);
    LCD_BEGIN();

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(2000); // Pause for 2 seconds
}

void runtests() {
    // Clear the buffer
    display.clearDisplay();

    // Draw a single pixel in white
    display.drawPixel(10, 10, KS0108_WHITE);

    // Show the display buffer on the screen. You MUST call display() after
    // drawing commands to make them visible on screen!
    display.display();
    delay(2000);
    // display.display() is NOT necessary after every single drawing command,
    // unless that's what you want...rather, you can batch up a bunch of
    // drawing operations and then update the screen all at once by calling
    // display.display(). These examples demonstrate both approaches...

    testdrawline();      // Draw many lines

    testdrawrect();      // Draw rectangles (outlines)

    testfillrect();      // Draw rectangles (filled)

    testdrawcircle();    // Draw circles (outlines)

    testfillcircle();    // Draw circles (filled)

    testdrawroundrect(); // Draw rounded rectangles (outlines)

    testfillroundrect(); // Draw rounded rectangles (filled)

    testdrawtriangle();  // Draw triangles (outlines)

    testfilltriangle();  // Draw triangles (filled)

    testdrawchar();      // Draw characters of the default font

    testdrawstyles();    // Draw 'stylized' characters

    testscrolltext();    // Draw scrolling text

    testdrawbitmap();    // Draw a small bitmap image

    display.clearDisplay();
    display.setFont(&FreeSerif12pt7b);
    display.setCursor(0, 20);
    display.setTextSize(1);
    display.println("Free  Fonts");
    display.setFont(&FreeSans9pt7b);
    display.println("Adafruit");
    display.display();
    display.setFont(NULL);
    delay(1000);

    // Invert and restore display, pausing in-between
#if USE_LCD == 0xE7920
    display.invertDisplay(); //.kbv non-GFX method
    delay(1000);
    display.invertDisplay();
#else
    display.invertDisplay(true);
    delay(1000);
    display.invertDisplay(false);
#endif
    delay(1000);

    //testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}

void loop() {
    static uint8_t rotation;
    if (++rotation > 3) rotation = 0;
    display.setRotation(rotation);
    runtests();
}

void testdrawline() {
    int16_t i;

    display.clearDisplay(); // Clear display buffer

    for (i = 0; i < display.width(); i += 4) {
        display.drawLine(0, 0, i, display.height() - 1, KS0108_WHITE);
        display.display(); // Update screen with each newly-drawn line
        delay(1);
    }
    for (i = 0; i < display.height(); i += 4) {
        display.drawLine(0, 0, display.width() - 1, i, KS0108_WHITE);
        display.display();
        delay(1);
    }
    delay(250);

    display.clearDisplay();

    for (i = 0; i < display.width(); i += 4) {
        display.drawLine(0, display.height() - 1, i, 0, KS0108_WHITE);
        display.display();
        delay(1);
    }
    for (i = display.height() - 1; i >= 0; i -= 4) {
        display.drawLine(0, display.height() - 1, display.width() - 1, i, KS0108_WHITE);
        display.display();
        delay(1);
    }
    delay(250);

    display.clearDisplay();

    for (i = display.width() - 1; i >= 0; i -= 4) {
        display.drawLine(display.width() - 1, display.height() - 1, i, 0, KS0108_WHITE);
        display.display();
        delay(1);
    }
    for (i = display.height() - 1; i >= 0; i -= 4) {
        display.drawLine(display.width() - 1, display.height() - 1, 0, i, KS0108_WHITE);
        display.display();
        delay(1);
    }
    delay(250);

    display.clearDisplay();

    for (i = 0; i < display.height(); i += 4) {
        display.drawLine(display.width() - 1, 0, 0, i, KS0108_WHITE);
        display.display();
        delay(1);
    }
    for (i = 0; i < display.width(); i += 4) {
        display.drawLine(display.width() - 1, 0, i, display.height() - 1, KS0108_WHITE);
        display.display();
        delay(1);
    }

    delay(2000); // Pause for 2 seconds
}

void testdrawrect(void) {
    display.clearDisplay();
    int w = display.width(), h = display.height();
    int limit = (h < w) ? h : w;;

    for (int16_t i = 0; i < limit / 2; i += 2) {
        display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, KS0108_WHITE);
        display.display(); // Update screen with each newly-drawn rectangle
        delay(1);
    }

    delay(2000);
}

void testfillrect(void) {
    display.clearDisplay();
    int w = display.width(), h = display.height();
    int limit = (h < w) ? h : w;;

    for (int16_t i = 0; i < limit / 2; i += 3) {
        // The INVERSE color is used so rectangles alternate white/black
        display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, KS0108_INVERSE);
        display.display(); // Update screen with each newly-drawn rectangle
        delay(1);
    }

    delay(2000);
}

void testdrawcircle(void) {
    display.clearDisplay();

    for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 2) {
        display.drawCircle(display.width() / 2, display.height() / 2, i, KS0108_WHITE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testfillcircle(void) {
    display.clearDisplay();

    for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 3) {
        // The INVERSE color is used so circles alternate white/black
        display.fillCircle(display.width() / 2, display.height() / 2, i, KS0108_INVERSE);
        display.display(); // Update screen with each newly-drawn circle
        delay(1);
    }

    delay(2000);
}

void testdrawroundrect(void) {
    display.clearDisplay();
    int w = display.width(), h = display.height();
    int limit = (h < w) ? h : w;;
    for (int16_t i = 0; i < limit / 2 - 2; i += 2) {
        display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                              display.height() / 4, KS0108_WHITE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testfillroundrect(void) {
    display.clearDisplay();
    int w = display.width(), h = display.height();
    int limit = (h < w) ? h : w;;

    for (int16_t i = 0; i < limit / 2 - 2; i += 2) {
        // The INVERSE color is used so round-rects alternate white/black
        display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                              display.height() / 4, KS0108_INVERSE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testdrawtriangle(void) {
    display.clearDisplay();

    for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 5) {
        display.drawTriangle(
            display.width() / 2  , display.height() / 2 - i,
            display.width() / 2 - i, display.height() / 2 + i,
            display.width() / 2 + i, display.height() / 2 + i, KS0108_WHITE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testfilltriangle(void) {
    display.clearDisplay();

    for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 5) {
        // The INVERSE color is used so triangles alternate white/black
        display.fillTriangle(
            display.width() / 2  , display.height() / 2 - i,
            display.width() / 2 - i, display.height() / 2 + i,
            display.width() / 2 + i, display.height() / 2 + i, KS0108_INVERSE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testdrawchar(void) {
    display.clearDisplay();

    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(KS0108_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.cp437(true);         // Use full 256 char 'Code Page 437' font

    // Not all the characters will fit on the display. This is normal.
    // Library will draw what it can and the rest will be clipped.
    for (int16_t i = 0; i < 256; i++) {
        if (i == '\n') display.write(' ');
        else          display.write(i);
    }

    display.display();
    delay(2000);
}

void testdrawstyles(void) {
    display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(KS0108_WHITE);        // Draw white text
    display.setCursor(0, 0);            // Start at top-left corner
    display.println(F("Hello, world!"));

    display.setTextColor(KS0108_BLACK, KS0108_WHITE); // Draw 'inverse' text
    display.println(3.141592);

    display.setTextSize(2);             // Draw 2X-scale text
    display.setTextColor(KS0108_WHITE);
    display.print(F("0x")); display.println(0xDEADBEEF, HEX);

    display.display();
    delay(2000);
}

void testscrolltext(void) {
    display.clearDisplay();

    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(KS0108_WHITE);
    display.setCursor(10, 0);
    display.println(F("scroll"));
    display.display();      // Show initial text
    delay(100);

}

void testdrawbitmap(void) {
    display.clearDisplay();

    if (display.getRotation() & 1) {
        display.drawBitmap(0, 14, beetle_64x99, 64, 99, KS0108_WHITE);
        //display.drawBitmap(0, 0, beetle_64x128, 64, 128, KS0108_WHITE);
        //display.drawBitmap(2, 0, toilet_61x128, 61, 128, KS0108_WHITE);
        //display.drawBitmap(1, 0, rocket_62x128, 62, 128, KS0108_WHITE);
    }
    else display.drawBitmap(0, 0, tractor_128x64, 128, 64, KS0108_WHITE);

    display.display();
    delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
    int8_t f, icons[NUMFLAKES][3];

    // Initialize 'snowflake' positions
    for (f = 0; f < NUMFLAKES; f++) {
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
        Serial.print(F("x: "));
        Serial.print(icons[f][XPOS], DEC);
        Serial.print(F(" y: "));
        Serial.print(icons[f][YPOS], DEC);
        Serial.print(F(" dy: "));
        Serial.println(icons[f][DELTAY], DEC);
    }

    for (;;) { // Loop forever...
        display.clearDisplay(); // Clear the display buffer

        // Draw each snowflake:
        for (f = 0; f < NUMFLAKES; f++) {
            display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, KS0108_WHITE);
        }

        display.display(); // Show the display buffer on the screen
        delay(200);        // Pause for 1/10 second

        // Then update coordinates of each flake...
        for (f = 0; f < NUMFLAKES; f++) {
            icons[f][YPOS] += icons[f][DELTAY];
            // If snowflake is off the bottom of the screen...
            if (icons[f][YPOS] >= display.height()) {
                // Reinitialize to a random position, just off the top
                icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
                icons[f][YPOS]   = -LOGO_HEIGHT;
                icons[f][DELTAY] = random(1, 6);
            }
        }
    }
}
