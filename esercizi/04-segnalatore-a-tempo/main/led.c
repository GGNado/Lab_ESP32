#include "led.h"
#include "driver/gpio.h"

static const gpio_num_t PIN_LED_VERDE = GPIO_NUM_16;
static const gpio_num_t PIN_LED_ROSSO = GPIO_NUM_15;

esp_err_t led_init(void) {
    gpio_config_t led_config = {
        .pin_bit_mask = (1ULL << PIN_LED_ROSSO) | (1ULL << PIN_LED_VERDE),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t esito = gpio_config(&led_config);
    if (esito != ESP_OK) {
        return esito;
    }

    esito = gpio_set_level(PIN_LED_VERDE, 0);
    if (esito != ESP_OK) {
        return esito;
    }

    return gpio_set_level(PIN_LED_ROSSO, 0);
}

esp_err_t led_set(Led led, bool stato)
{
    if (led != LED_VERDE && led != LED_ROSSO) {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_num_t pin = (led == LED_VERDE) ? PIN_LED_VERDE : PIN_LED_ROSSO;
    return gpio_set_level(pin, stato);
}

