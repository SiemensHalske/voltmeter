// src/config.h

#ifndef CONFIG_H
#define CONFIG_H

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8
#define TFT_BACKLIGHT 7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define BUFFER_SIZE 8
#define BUFFER_MASK (BUFFER_SIZE - 1)

volatile uint8_t buffer[BUFFER_SIZE];
volatile uint8_t head = 0;

struct ADCProfile
{
    uint8_t admux;
    uint8_t adcsra;
    uint8_t adcsrb;
};

struct ADCConfigSet
{
    ADCProfile profiles[2];
};

const ADCConfigSet adc_configs = {
    .profiles = {
        // Profile 0 – Timer1 Match A, AVcc, left-adjusted
        {
            .admux = (1 << REFS0) | (1 << ADLAR),
            .adcsra = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0),
            .adcsrb = (1 << ADTS1) | (1 << ADTS0)},
        // Profile 1 – Free Running, 1.1V, right-adjusted
        {
            .admux = (1 << REFS1) | (1 << REFS0),                                             // 1.1V internal, right adjust
            .adcsra = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS0), // prescaler 32
            .adcsrb = 0                                                                       // Free running
        }}};

#endif // CONFIG_H