#include "display.h"
#include "config.h"
#include "setup.h"

volatile unsigned long TIME_BASE = 0;
volatile uint8_t buffer[BUFFER_SIZE];
volatile uint8_t head = 0;

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
