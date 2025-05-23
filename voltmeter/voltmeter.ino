#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

// —— Pin defs ——
#define OUT_PIN PD2
#define OUT_DDR DDRD
#define LED_PIN PB5
#define LED_DDR DDRB

// —— Config macros ——
#define PRESC_F (2UL * 64)
#define HEARTBEAT_INTERVAL 2000

// ——— STARTUP ———
void setup()
{
    OUT_DDR |= _BV(OUT_PIN); // Output pin for 15Hz signal
    LED_DDR |= _BV(LED_PIN); // LED for heartbeat

    setup_timer1(2500); // 15.00 Hz = 2500 centiHz

    sei(); // Global interrupts on

    // Power savings
    ACSR |= _BV(ACD);
    PRR = _BV(PRADC) | _BV(PRSPI) | _BV(PRTWI) | _BV(PRTIM2);
}

void heartbeat()
{
    static uint32_t lastBlink = 0;
    uint32_t now = millis();

    if (now - lastBlink >= HEARTBEAT_INTERVAL)
    {
        lastBlink = now;
        PINB = _BV(LED_PIN); // Toggle heartbeat LED
    }
}

// ——— LOOP ———
void loop()
{
    heartbeat();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_mode();
}

// ——— TIMER1 SETUP + ISR ———
void setup_timer1(uint16_t freqQ)
{
    uint32_t top = (F_CPU * 100UL) / (PRESC_F * freqQ) - 1UL;
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10); // CTC mode, prescaler 64
    OCR1A = top;
    TIMSK1 |= _BV(OCIE1A); // Enable compare interrupt
    interrupts();
}
ISR(TIMER1_COMPA_vect)
{
    PIND = _BV(OUT_PIN); // Toggle OUT_PIN
}
