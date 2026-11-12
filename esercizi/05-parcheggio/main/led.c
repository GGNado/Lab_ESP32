//
// Created by Luigi Massa on 11/11/2026.
//

#include "led.h"

#include "driver/gpio.h"
#include "soc/gpio_num.h"

static const gpio_num_t PIN_LED_GREEN = GPIO_NUM_16;
static const gpio_num_t PIN_LED_RED = GPIO_NUM_15;
static const gpio_num_t PIN_LED_YELLOW = GPIO_NUM_17;

esp_err_t led_init(void) {
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << PIN_LED_GREEN) | (1ULL << PIN_LED_RED) | (1ULL << PIN_LED_YELLOW),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t esito = gpio_config(&config);

    if (esito != ESP_OK) {
        return esito;
    }

    esito = gpio_set_level(PIN_LED_GREEN, 0);
    if (esito != ESP_OK) {
        return esito;
    }

    esito = gpio_set_level(PIN_LED_RED, 0);
    if (esito != ESP_OK) {
        return esito;
    }

    return gpio_set_level(PIN_LED_YELLOW, 0);
}

esp_err_t led_on(Led led) {
    switch (led) {
        case LED_RED:
            return gpio_set_level(PIN_LED_RED, 1);
            break;
        case LED_GREEN:
            return gpio_set_level(PIN_LED_GREEN, 1);
            break;
        case LED_YELLOW:
            return gpio_set_level(PIN_LED_YELLOW, 1);
            break;
        default:
            return ESP_FAIL;
    }
    return ESP_FAIL;
}

esp_err_t led_off(Led led) {
    switch (led) {
        case LED_RED:
            return gpio_set_level(PIN_LED_RED, 0);
        case LED_GREEN:
            return gpio_set_level(PIN_LED_GREEN, 0);
        case LED_YELLOW:
            return gpio_set_level(PIN_LED_YELLOW, 0);
        default:
            return ESP_FAIL;
    }
    return ESP_FAIL;
}
