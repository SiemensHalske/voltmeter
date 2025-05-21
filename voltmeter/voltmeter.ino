#include "config.h"

volatile unsigned long TIME_BASE = 0;

volatile uint8_t buffer[BUFFER_SIZE];
volatile uint8_t head = 0;

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

uint8_t read_adc_channel(uint8_t channel)
{

    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC))
        ;

    return ADCH;
}

void setup()
{
    init_display();

    pinMode(13, OUTPUT);

    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A = TGUARD_COMP_VAL;
    TIMSK1 |= (1 << OCIE1A);

    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22) | (1 << CS21);
    OCR2A = ADC_TIMER_COMP_VAL;
    TIMSK2 |= (1 << OCIE2A);

    uint16_t adc_raw = read_adc_channel(1);
    if (adc_raw < PROFILE_0_ref)
    {
        apply_adc_profile(adc_configs, 0);
    }
    else if (adc_raw < PROFILE_1_ref)
    {
        apply_adc_profile(adc_configs, 1);
    }
    else
    {
        apply_adc_profile(adc_configs, 1);
    }

    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    sei();
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

void loop()
{
    static unsigned long lastUpdate = 0;
    const unsigned long updateInterval = 1;

    sleep_cpu();

    if (TIME_BASE - lastUpdate >= updateInterval)
    {
        lastUpdate = TIME_BASE;

        float frequency = F_CPU / (2 * (OCR1A + 1) * 64);

        sys_display(frequency);
    }
}
