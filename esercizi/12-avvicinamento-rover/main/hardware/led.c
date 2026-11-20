//
// Created by Luigi Massa on 20/11/2026.
//

#include "led.h"

#include "driver/gpio.h"

esp_err_t led_init(const LED *led) {
    const gpio_config_t config = {
        .pin_bit_mask = (1ULL << led->pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    return gpio_config(&config);
}

void led_set(const LED *led, bool on) {
    gpio_set_level(led->pin, on);
}



