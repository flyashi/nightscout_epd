#include "display.h"
/*
mosi: 23
miso: 17
clk: 18
cs: 5
rst: 16
busy: 4
*/

  GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=D8*/ 5, /*DC=D3*/ 17, /* rst */ 16, /*BUSY=D2*/ 4));
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

void display_init() {
    display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
}
void display_hibernate() {
  display.hibernate();
}
void fullscreen_message(char *message) {
  fullscreen_message_subtitle(message, "");
}

void fullscreen_message_subtitle(char* message, char* subtitle) {
  fullscreen_message_subtitle_header(message, subtitle, "");
}

void fullscreen_message_subtitle_header(char* message, char* subtitle, char* header) {
  display_init();
  display.setRotation(1);
  //display.setFont(&FreeMonoBold9pt7b);
  display.setFont(&FreeMonoBold24pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();

    display.setFont(&FreeMonoBold9pt7b);

  int16_t tbx2, tby2; uint16_t tbw2, tbh2;
  display.getTextBounds(subtitle, 0, 0, &tbx2, &tby2, &tbw2, &tbh2);
  uint16_t x2 = ((display.width() - tbw2) / 2) - tbx2;
  uint16_t y2 = (display.height() - tbh2) - tby2;

    int16_t tbx3, tby3; uint16_t tbw3, tbh3;
    display.getTextBounds(header, 0, 0, &tbx3, &tby3, &tbw3, &tbh3);
Serial.printf("DEBUG: tbx3=%d tby3=%d tbw3=%d tbh3=%d\n", tbx3, tby3, tbw3, tbh3);
  do
  {
      display.setFont(&FreeMonoBold24pt7b);

    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(message);
      display.setFont(&FreeMonoBold9pt7b);

    display.setCursor(x2, y2);
    display.print(subtitle);

    display.setCursor(0, -tby3);
    display.print(header);
  }
  while (display.nextPage());
  display_hibernate();
}
