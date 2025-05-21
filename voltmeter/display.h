#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

extern Adafruit_ST7735 tft;

void init_display();
void init_message();
void sys_display(float freq);

#endif // DISPLAY_H
