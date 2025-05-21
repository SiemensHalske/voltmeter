#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// Define SPI pins for the display
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

// Create display object
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

void apply_adc_profile(const ADCConfigSet &config_set, uint8_t profile_id)
{
    if (profile_id >= sizeof(config_set.profiles) / sizeof(ADCProfile))
        return; // sanity check

    const ADCProfile &p = config_set.profiles[profile_id];

    ADMUX = p.admux;
    ADCSRA = p.adcsra;
    ADCSRB = p.adcsrb;
    ADCSRA |= (1 << ADSC); // Start conversion
}

uint8_t read_adc_channel(uint8_t channel)
{
    // Configure the ADC channel
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel
    ADCSRA |= (1 << ADSC);                     // Start conversion

    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC))
        ;

    return ADCH; // Return the 8-bit high result
}

void setup()
{
    pinMode(13, OUTPUT); // Status LED

    // Timer1 CTC, 10ms
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A = 2499;
    TIMSK1 = 0;

    // Read ADC channel 1 for profile switch
    uint16_t adc_raw = read_adc_channel(1); // Using ADC channel 1 for profile switching

    if (adc_raw < 338)
    {
        apply_adc_profile(adc_configs, 0); // Apply profile 0 when Voltage is 0V
    }
    else if (adc_raw < 849)
    {
        apply_adc_profile(adc_configs, 1); // Apply profile 1 when Voltage is 3V3
    }
    else
    {
        apply_adc_profile(adc_configs, 1); // Apply profile 1 when voltage is 5V
    }

    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    sei();

    // Initialize the display
    tft.initR(INITR_BLACKTAB); // Use the correct initialization for your display
    tft.setRotation(1);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);

    // Display initial message
    tft.setCursor(0, 0);
    tft.print("Initializing...");
}

ISR(ADC_vect)
{
    buffer[head] = ADCH;             // Read the ADC value
    head = (head + 1) & BUFFER_MASK; // Increment head with wrap-around

    PINB |= (1 << PINB5); // Toggle the LED
}

void loop()
{
    static unsigned long lastUpdate = 0;      // Tracks the last time the display was updated
    const unsigned long updateInterval = 500; // Update every 500ms

    sleep_cpu(); // Enter sleep mode

    // Check if it's time to update the display
    if (millis() - lastUpdate >= updateInterval)
    {
        lastUpdate = millis(); // Update the timestamp

        // Calculate timer frequency
        float frequency = 16000000.0 / (2 * (OCR1A + 1) * 64); // Assuming prescaler of 64

        // Update display with the frequency
        tft.fillScreen(ST7735_BLACK);
        tft.setCursor(0, 0);
        tft.print("Frequency:");
        tft.setCursor(0, 20);
        tft.print(frequency);
        tft.print(" Hz");
    }
}
