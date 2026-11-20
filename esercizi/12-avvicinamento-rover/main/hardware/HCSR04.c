//
// Created by Luigi Massa on 20/11/2026.
//

#include "HCSR04.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

esp_err_t hcsr04_init(HCSR04 *hcsr04) {

    gpio_config_t config = {
        .pin_bit_mask = (1ULL << hcsr04->echo_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    if (gpio_config(&config)) {
        return ESP_FAIL;
    };

    gpio_config_t config2 = {
        .pin_bit_mask = (1ULL << hcsr04->trig_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    if (gpio_config(&config2)) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

int hcsr04_distance(const HCSR04 *hcsr04) {
    gpio_set_level(hcsr04->trig_pin, 1);
    esp_rom_delay_us(10);
    gpio_set_level(hcsr04->trig_pin, 0);

    const int64_t scadenza = esp_timer_get_time() + 30000; // 30 ms

    // Aspetto che ECHO salga da 0 a 1.
    while (gpio_get_level(hcsr04->echo_pin) == 0) {
        if (esp_timer_get_time() >= scadenza) {
            return -1;
        }
    }

    const int64_t inizio = esp_timer_get_time();

    // Aspetto che ECHO scenda da 1 a 0.
    while (gpio_get_level(hcsr04->echo_pin) == 1) {
        if (esp_timer_get_time() >= scadenza) {
            return -1;
        }
    }
    const int64_t fine = esp_timer_get_time();

    return (int)((fine - inizio) / 58);
}
