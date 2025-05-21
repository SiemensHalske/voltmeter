#include "../display.h"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void init_display()
{
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);

    init_message();
}

void init_message()
{
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Initializing...");
}

void sys_display(float freq)
{
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 0);
    tft.print("Frequency:");
    tft.setCursor(0, 20);
    tft.print(freq);
    tft.print(" Hz");
}
