//
// Created by Luigi Massa on 20/11/2026.
//

#ifndef AVVICINAMENTO_ROVER_LED_H
#define AVVICINAMENTO_ROVER_LED_H
#include <stdint.h>

#include "esp_err.h"
#include "soc/gpio_num.h"

typedef enum {
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_WHITE,
    LED_YELLOW,
    LED_ORANGE,
    LED_PURPLE,
    LED_PINK,
    LED_BLACK,
} LED_COLOR;

typedef struct {
    gpio_num_t pin;
    LED_COLOR color;

} LED;

esp_err_t led_init(const LED *led);
void led_set(const LED *led, bool on);

#endif //AVVICINAMENTO_ROVER_LED_H
