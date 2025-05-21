// src/config.h

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "display.h"

// ===== TFT display pin definitions =====
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8
#define TFT_BACKLIGHT 7

// ===== Magic Numbers =====
#define F_CPU 16000000.0        // 16MHz
#define TGUARD_COMP_VAL 124999  // Compare value for 0.5s at 16MHz with prescaler 64 Timer 1
#define ADC_TIMER_COMP_VAL 2499 // Compare value for 10ms at 16MHz with prescaler 64 Timer 2
#define DP_UPDATE_INTERVAL 500  // 500ms update interval for display
#define PROFILE_0_ref 0x152     // 0V reference for profile switching
#define PROFILE_1_ref 0x351     // 3.3V reference for profile switching

// ===== Buffer definitions =====
#define BUFFER_SIZE 8
#define BUFFER_MASK (BUFFER_SIZE - 1)
volatile uint8_t buffer[BUFFER_SIZE];
volatile uint8_t head = 0;

// ===== ADC configuration structures =====
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

// ===== ADC configuration set =====
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