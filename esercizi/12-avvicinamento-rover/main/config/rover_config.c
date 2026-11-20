#include "rover_config.h"
#include "driver/gpio.h"

HCSR04 hcsr04_sensori[] = {
    { .trig_pin = GPIO_NUM_4, .echo_pin = GPIO_NUM_5 }, // FRONTALE
    { .trig_pin = GPIO_NUM_6, .echo_pin = GPIO_NUM_7 }, // POSTERIORE
};

LED leds[] = {
    { .pin = GPIO_NUM_3,  .color = LED_GREEN },
    { .pin = GPIO_NUM_18, .color = LED_RED },
    { .pin = GPIO_NUM_8,  .color = LED_YELLOW },
};

const size_t numero_sensori = sizeof(hcsr04_sensori) / sizeof(hcsr04_sensori[0]);
const size_t numero_led = sizeof(leds) / sizeof(leds[0]);