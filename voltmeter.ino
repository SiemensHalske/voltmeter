#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

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
    ADCProfile profiles[2]; // expand here if needed
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

void apply_adc_profile(const ADCConfigSet &config_set, uint8_t profile_id)
{
    if (profile_id >= sizeof(config_set.profiles) / sizeof(ADCProfile))
        return; // sanity check

    const ADCProfile &p = config_set.profiles[profile_id];

    ADMUX = p.admux;
    ADCSRA = p.adcsra;
    ADCSRB = p.adcsrb;
    ADCSRA |= (1 << ADSC);
}

void setup()
{
    pinMode(13, OUTPUT);

    // Timer1 CTC, 10ms
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A = 2499;
    TIMSK1 = 0;

    uint8_t selected_profile = 0; // Change to 1 for the second profile
    apply_adc_profile(adc_configs, selected_profile);

    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    sei();
}

ISR(ADC_vect)
{
    buffer[head] = ADCH;
    head = (head + 1) & BUFFER_MASK;

    PINB |= (1 << PINB5);
}

void loop()
{
    sleep_cpu();
}
