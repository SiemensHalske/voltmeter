// src/config.h

#ifndef CONFIG_H
#define CONFIG_H

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

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// ===== Buffer definitions =====

// Buffer config
#define BUFFER_SIZE 8
#define BUFFER_MASK (BUFFER_SIZE - 1)

// Extern declarations
extern volatile uint8_t buffer[BUFFER_SIZE];
extern volatile uint8_t head;

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
        // Profile 0 – Timer2 Match A, AVcc, left-adjusted, Channel ADC0
        {
            .admux = (1 << REFS0) | (1 << ADLAR) | (0 << MUX0), // MUX[3:0]=0000 for ADC0
            .adcsra = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0),
            .adcsrb = (1 << ADTS1) | (1 << ADTS0) // ADTS[2:0]=011 for Timer2 Compare Match A
        },
        // Profile 1 – Timer1 Match A, AVcc, left-adjusted, Channel ADC5
        {
            .admux = (1 << REFS0) | (1 << ADLAR) | (1 << MUX2) | (1 << MUX0), // MUX[3:0]=0101 for ADC5
            .adcsra = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0),
            .adcsrb = (1 << ADTS2) // ADTS[2:0]=100 for Timer1 Compare Match A
        },
    }};

#endif // CONFIG_H