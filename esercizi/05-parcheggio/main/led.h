//
// Created by Luigi Massa on 11/11/2026.
//

#ifndef PARCHEGGIO_LED_H
#define PARCHEGGIO_LED_H
#include "esp_err.h"

typedef enum {
    LED_RED,
    LED_GREEN,
    LED_YELLOW,
} Led;

esp_err_t led_init(void);
esp_err_t led_on(Led led);
esp_err_t led_off(Led led);

#endif //PARCHEGGIO_LED_H
