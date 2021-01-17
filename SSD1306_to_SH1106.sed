# Sino Wealth SH1106 is very similar to Solomon SSD1306 but without the Scroll functions
# this script will rename registers and class from the Adafruit_SSD1306 library
# it inherits from Adafruit_GFX and Print classes
#
# sed -f SSD1306_to_SH1106.sed ../Adafruit_SSD1306/Adafruit_SSD1306.h > Adafruit_SH1106_kbv.h
# sed -f SSD1306_to_SH1106.sed ../Adafruit_SSD1306/Adafruit_SSD1306.cpp > Adafruit_SH1106_kbv.cpp

# header guard
s/Adafruit_SSD1306_H/Adafruit_SH1106_KBV_H/
# remove SCROLL defines and method declarations from H 
/SCROLL/d
/scroll/d
# remove SCROLL stuff from CPP
/^\/\/ SCROLLING FUNCTIONS/,/^\/\/ OTHER HARDWARE/d
# else these cover most changes
s/Adafruit_SSD1306/Adafruit_SH1106_kbv/g
s/SSD1306_/SH1106_/g
# compatibility with most SSD1306 begin() statements
/define SH1106_SWITCHCAPVCC/a#define SSD1306_SWITCHCAPVCC SH1106_SWITCHCAPVCC
s/ssd1306_/sh1106_/
# read in replacement method. delete existing method. unconditional branch to the end
/::display(void)/r replace.txt
/::display(void)/,/^}/d
