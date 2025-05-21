#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

extern Adafruit_ST7735 tft;

void init_display();
void init_message();
void sys_display(float freq);

#endif // DISPLAY_H
