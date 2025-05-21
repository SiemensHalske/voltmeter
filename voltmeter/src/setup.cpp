#include "../config.h"
#include "../setup.h"
#include "../display.h"

void setup_timer_1()
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A = TGUARD_COMP_VAL;
    TIMSK1 |= (1 << OCIE1A);
}

void setup_timer_2()
{
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22) | (1 << CS21);
    OCR2A = ADC_TIMER_COMP_VAL;
    TIMSK2 |= (1 << OCIE2A);
}

ISR(TIMER1_COMPA_vect)
{
    TIME_BASE++;
}

ISR(TIMER2_COMPA_vect)
{
    buffer[head] = ADCH;
    head = (head + 1) & BUFFER_MASK;

    PINB |= (1 << PINB5);
}

void pin_setup()
{
    pinMode(13, OUTPUT);
}

void apply_adc_profile(const ADCConfigSet &config_set, uint8_t profile_id)
{
    if (profile_id >= sizeof(config_set.profiles) / sizeof(ADCProfile))
        return;

    const ADCProfile &p = config_set.profiles[profile_id];

    ADMUX = p.admux;
    ADCSRA = p.adcsra;
    ADCSRB = p.adcsrb;
    ADCSRA |= (1 << ADSC);
}

void setup()
{
    init_display();

    pin_setup();

    setup_timer_1();
    setup_timer_2();

    apply_adc_profile(adc_configs, 0);

    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    sei();
}
