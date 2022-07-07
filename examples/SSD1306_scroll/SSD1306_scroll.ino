#define SSD 0x1106    //0x1106, 0x1306, 0x1306B, 0x1315, 0x1309

#define G_BUFFER display.getBuffer()

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include "solomon_mono.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define up2dir(updn, rtlft) updn, rtlft
#define scrollrect s_scrollrect

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#if SSD == 0x1106
#include <Adafruit_SH1106_kbv.h>
//Adafruit_SH1106_kbv display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_SH1106_kbv display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, 9, 8, 10);
#else
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, 9, 8, 10);
#endif

#include <stdarg.h>
size_t Pprintf(Print &outdev, const char *format, ...)
{
    char buf[66];  //
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);
    return (outdev.write(buf));
}

// forward declarations avoid the IDE doing them for you.
void setup();
void loop();
bool silent = false;
uint8_t *g_buffer;

// scroll a rectangle up, down, rt, left, diagonally, ...
void s_scrollrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int8_t updn, int8_t rtlft)
{
    //uint8_t *p = display.getBuffer(); //avoid pre calculation
    uint8_t sb = ((y) & 7);           // posn  2
    uint8_t nsm = ((uint8_t)0xFF << sb);       // mask  0x03
    uint8_t eb = ((y + h - 1) & 7);   // posn  4
    uint8_t nem = ~(0xFF << (eb + 1));    // mask  0xE0
    int16_t sp = (y / 8) * 128;       // page  0
    int16_t ep = ((y + h - 1) / 8) * 128; //   2
    uint8_t sb_bm = (1 << sb);
    uint8_t eb_bm = (1 << eb);

    if (rtlft) { //rtlft 1488 + 1500 + 668 + 712 = 4536us
        for (int page = sp; page <= ep; page += 128) {
            uint8_t *p = G_BUFFER; //only 8 times
            uint8_t temp;
            uint8_t col;
            uint8_t mask = 0xFF;
            if (page == sp) mask &= nsm;
            if (page == ep) mask &= nem;
            if (rtlft > 0) { //rt
                uint8_t *q = &p[page + x + w - 1];
                temp = q[0];
                if (mask == 0xFF) {
                    memmove(p + page + x + 1, p + page + x, w - 1);
                } else {
                    for (col = x + w - 1; col > x; col--) {
                        q[0] &= ~mask;
                        uint8_t c = *--q & mask;
                        q[1] |= c;
                    }
                }
                p[page + col] = (p[page + col] & ~mask) | (temp & mask);
            }
            else { //lft
                uint8_t *q = &p[page + x];
                temp = q[0];
                col = x;
                if (mask == 0xFF) {
                    memmove(p + page + x, p + page + x + 1, w - 1);
                } else {
                    do {
                        q[0] = (q[0] & ~mask) | (q[1] & mask);
                        q++;
                    } while (++col < x + w - 1);
                }
                //q[0] = (q[0] & ~mask) | (temp & mask); //upsets optimZ
                col = x + w - 1;
                p[page + col] = (p[page + col] & ~mask) | (temp & mask);
            }
        }
    } //rtlft
    if (updn) {
        for (uint8_t col = x; col < x + w; col++) {
            int pp = ep - sp;
            uint8_t *q = G_BUFFER + col + sp; //best use of R26:27
            uint8_t lo = q[0], hi = q[pp];
            q[0] &= nsm;
            q[pp] &= nem;
            int8_t cnt = (pp) / 128 + 1;
            if (updn > 0) { //up
                uint8_t cy = (lo & (sb_bm)) ? (eb_bm) : 0;
                q += pp;
                do {
                    uint8_t c = q[0];
                    q[0] = (c >> 1) | cy;
                    q -= 128;
                    if (c & 0x01) cy = (1 << 7);
                    else cy = 0;
                } while (--cnt != 0);
            }
            else { // dn
                uint8_t cy = (hi & (eb_bm)) ? (sb_bm) : 0;
                do {
                    uint8_t c = q[0];
                    q[0] = (c << 1) | cy;
                    q += 128;
                    if (c & 0x80) cy = (1 << 0);
                    else cy = 0;
                } while (--cnt != 0);
            }
            q = G_BUFFER + col + sp; //not too expensive
            q[0] = (q[0] & nsm) | (lo & ~nsm);
            q[pp] = (q[pp] & nem) | (hi & ~nem);
        }
    } //updn
    if (!silent) {
        display.display(); //I2C@400kHz ~2.5ms.
        delay(10);  // a bit faster
    }
}

void drawsolomon(void)
{
    display.clearDisplay();
    display.drawRect(0, 0, 128, 64, WHITE);
    //display.drawBitmap(2, 0, solomon_125x64, 125, 64, WHITE);
    display.drawBitmap(41, 1, solomon_46x32, 46, 32, WHITE);
    display.setFont(NULL);
    display.setCursor(43, 40); display.print("SOLOMON");
    display.setCursor(43, 50); display.print("SYSTECH");
    //display.drawRect(0, 0, 128, 64, WHITE);
    //display.drawPixel(124, 1, WHITE);
    display.display();
}

void screen_message(String msg)
{
    display.clearDisplay();
    display.setFont(msg.c_str()[0] == '\n' ? NULL : &FreeSans9pt7b);
    display.setCursor(0, 2);
    Pprintf(display, "\n %s%04lX\n ", SSD == 0x1106 ? "SH" : "SSD", SSD);
    Pprintf(display, "%s", msg.c_str());
    display.drawRect(0, 0, 128, 64, WHITE);
    display.display();
    delay(msg.c_str()[0] == '\n' ? 5000 : 1000);
}

#if SSD != 0x1106
// SSD1309, SSD1315 can set an exact rectangle.  Diag, Vert, Horiz
// SSD1306 always uses whole width 0-127.  No Vert.  Always Diag
void startscroll_rect(int x, int y, int w, int h, int up, int horiz, int C = 7)
{
    uint8_t cmd = 0x29;
    if (horiz < 0) cmd = 0x2A, horiz = 1; //diagonal left
    if (up < 0) up = (h - 1) & 0x3F;
    //if (up == 59) up = 29;
    //up &= 0x3F;
    display.ssd1306_command(0xA3);
    display.ssd1306_command(y);  // vert scroll area
    display.ssd1306_command(h); // top+lines <= 64

    display.ssd1306_command(cmd); //vert_right_scroll
    display.ssd1306_command(horiz); //A enable.  1306 does not care
    display.ssd1306_command(y / 8); //B start page
    display.ssd1306_command(C); //C
    display.ssd1306_command((y + h - 1) / 8); //D end page
    display.ssd1306_command(up); //E
#if SSD == 0x1309 || SSD == 0x1315 //SSD1309 has extra arg
    display.ssd1306_command(x); //F
    display.ssd1306_command(x + w - 1);  //G
#endif

    display.ssd1306_command(0x2F);  //start

}

void startscroll_horiz(int x, int y, int w, int h, int horiz, int C = 7)
{
    uint8_t cmd = (horiz <= 0) ? 0x27 : 0x26; //left
    display.ssd1306_command(0xA3);
    display.ssd1306_command(y);  // vert scroll area
    display.ssd1306_command(h); // top+lines <= 64

    display.ssd1306_command(cmd); //hriz_scroll
    display.ssd1306_command(0); //A
    display.ssd1306_command(y / 8); //B start page
    display.ssd1306_command(C); //C
    display.ssd1306_command((y + h - 1) / 8); //D end page
#if SSD == 0x1309  //SSD1309 has extra arg
    display.ssd1306_command(0); //E
#endif
    display.ssd1306_command(x); //E
    display.ssd1306_command(x + w - 1);  //F

    display.ssd1306_command(0x2F);  //start

}

void HW_scroll_T(char *msg, int up, int rt, int ms, int C = 7)
{
    screen_message(msg);
#if 1
    if (up == 0 && rt == 1)
        startscroll_horiz(2, 2, 124, 60, rt, C);
    else
#endif
        startscroll_rect(2, 2, 124, 60, up, rt, C);
    delay(ms);            //show for a bit
    display.stopscroll(); //0x2E then stop
    delay(2000);
}

void solomon_T(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int8_t updn, int8_t rtlft, int ms)
{
    drawsolomon();
    startscroll_rect(x, y, w, h, updn, rtlft, 0); //5 frames
    delay(ms * 50);            //show for a bit
    display.stopscroll(); //0x2E then stop
}

#endif   // !0x1106

uint32_t time_trial(int8_t updn, int8_t rtlft, char *msg)
{
    silent = true;
    uint32_t t;
    t = micros();
    scrollrect(1, 1, 126, 62, up2dir(updn, rtlft)); //vert up for BREAK
    t = micros() - t;
    Pprintf(Serial, "%luus %s\n", t, msg);
    silent = false;
    return t;
}

void setup()
{
    Serial.begin(9600);
    while (!Serial) ;
    //Wire.setSDA(4); Wire.setSCL(5);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }
    g_buffer = display.getBuffer();
    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(2000); // Pause for 2 seconds
    display.setTextColor(WHITE);
    //time_trial(1, 0, "vert up for BREAK");
    uint32_t t = 0;
    t += time_trial(1, 0, "vert up"); //2048
    t += time_trial(-1, 0, "vert dn"); //2228
    t += time_trial(0, 1, "horiz rt"); //672
    t += time_trial(0, -1, "horiz lft"); //672
    Serial.println(t);
}

void solomon_N(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int8_t updn, int8_t rtlft, int N)
{
    drawsolomon();
    for (int i = 0; i < N; i++) {
        scrollrect(x, y, w, h, up2dir(updn, rtlft)); //rotate logo
    }
}

void loop()
{
#if 0 || SSD == 0x1106
    screen_message(F("\n manipulate buffer\n to move rectangles\n in software"));
    solomon_N(41, 2, 46, 30, 0, 1, 46); //rotate logo
    solomon_N(41, 2, 46, 30, 1, 0, 30); //up logo
    solomon_N(41, 32, 46, 30, 1, 0, 30); //up text
    solomon_N(41, 32, 46, 30, -1, 0, 30); //down text
    solomon_N(2, 2, 124, 60, 1, 0, 60); //up window
    solomon_N(41, 2, 46, 30, -1, 0, 30); //down logo
    solomon_N(2, 2, 124, 60, -1, 0, 60); //down window
    solomon_N(2, 2, 124, 36, 0, 1, 124); //rt logo
    solomon_N(2, 2, 124, 36, 0, -1, 124); //lft logo
    solomon_N(2, 38, 124, 24, 0, 1, 124); //rt text
    solomon_N(2, 2, 124, 60, 0, 1, 60); //rt window
    solomon_N(0, 0, 128, 64, 1, 0, 128); // up screen
    solomon_N(0, 0, 128, 64, 0, -1, 128); // left screen
    solomon_N(0, 0, 128, 64, -1, -1, 128); // left, dn screen
#endif
#if SSD == 0x1106
    screen_message(F("\n Syno Wealth\n 132x64 controller\n can not scroll"));
#else
    screen_message(F("\n Solomon Systech\n hardware scroll\n functions\n frame period ~11ms"));
    screen_message(F("\n move rectangles\n in hardware"));
    solomon_T(41, 2, 46, 30, 0, 1, 46); //rotate logo
    solomon_T(41, 2, 46, 30, 1, 0, 30); //up logo
    solomon_T(41, 32, 46, 30, 1, 0, 30); //up text
    solomon_T(41, 32, 46, 30, -1, 0, 30); //down text
    solomon_T(2, 2, 124, 60, 1, 0, 60); //up window
    solomon_T(41, 2, 46, 30, -1, 0, 30); //down logo
    solomon_T(2, 2, 124, 60, -1, 0, 60); //down window
    solomon_T(2, 2, 124, 36, 0, 1, 124); //rt logo
    solomon_T(2, 2, 124, 36, 0, -1, 124); //lft logo
    solomon_T(2, 38, 124, 24, 0, 1, 124); //rt text
    solomon_T(2, 2, 124, 60, 0, 1, 60); //rt window
    solomon_T(0, 0, 128, 64, 1, 0, 128); // up screen
    solomon_T(0, 0, 128, 64, 0, -1, 128); // left screen
    solomon_T(0, 0, 128, 64, -1, -1, 128); // left, dn screen
#if 1
    screen_message(F("\n SSD1306 whole width\n SSD1309 window\n SSD1315 window"));
    float frm = (SSD == 0x1315) ? 14 : 11;
    HW_scroll_T("Diagonal Scroll", 1, 1, 5 * frm * 60, 7);
    //HW_scroll_T("Horiz 1X", 0, 1, frm * 5 * 124, 6); //5 frames
    HW_scroll_T("Horiz 2X", 0, 1, frm * 8 * 124, 5); //4 frames
    //HW_scroll_T("Horiz 3X", 0, 1, frm * 9 * 124, 4); //3 frames
    HW_scroll_T("Horiz 4X", 0, 1, frm * 8 * 124, 7); //2 frames
    HW_scroll_T("Scroll UP", 1, 0, 5 * frm * 60, 7);
    HW_scroll_T("Scroll DOWN", -1, 0, 5 * frm * 60, 7);
#endif
#endif
}

#if 0
// ################################## UNUSED ###########################################
extern void g_scrollrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t dir);
extern void g_scrollrect_up(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t dir);
extern void g_scrollrect_rt(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t dir);

#ifdef USE_EXT
uint8_t up2dir(int8_t updn, int8_t rtlft)
{
    uint8_t dir = 0;
    if (updn) {
        dir |= 1;
        if (updn > 0) dir |= 2;
    }
    if (rtlft) {
        dir |= 4;
        if (rtlft > 0) dir |= 8;
    }
    return dir;
}
#if 0
#define scrollrect g_scrollrect
#else
#define scrollrect(x, y, w, h, dir) {\
        g_scrollrect_up(x, y, w, h, dir); \
        g_scrollrect_rt(x, y, w, h, dir); \
    }
#endif
#else
#define up2dir(updn, rtlft) updn, rtlft
#define scrollrect s_scrollrect
#endif

...
#if USE_MEMMOVE
memmove(p + page + x + 1, p + page + x, w - 1);
#else
for (col = x + w - 1; col > x; col--) {
    uint8_t c = *--q;
    q[1] = c;
}
#endif
...
#if USE_MEMMOVE
memmove(p + page + x, p + page + x + 1, w - 1);
#else
do {
    q[0] = q[1];
    q++;
} while (++col < x + w - 1);
#endif
...

//always moves whole 8-row pages.  set left and rt limits by column
void startscrollhoriz(uint8_t pgtop, uint8_t pgbot, uint8_t left, uint8_t rt, uint8_t dir)
{
    display.ssd1306_command(dir ? 0x27 : 0x26);
    display.ssd1306_command(0);   //A
    display.ssd1306_command(pgtop); //B
    display.ssd1306_command(7);   //C 2 frames
    display.ssd1306_command(pgbot); //D
#if SSD == 0x1309 //SSD1309 has extra arg
    display.ssd1306_command(0); //E
    display.ssd1306_command(left); //F
    display.ssd1306_command(rt);  //G
#else //SSD1306 seems to accept SSD1315 args
    display.ssd1306_command(left); //E
    display.ssd1306_command(rt);  //F
#endif
    display.ssd1306_command(0x2F); //activate
}
#endif
