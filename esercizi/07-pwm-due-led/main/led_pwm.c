//
// Created by Luigi Massa on 13/11/2026.
//

#include "led_pwm.h"

#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

esp_err_t led_pwm_init() {
    const ledc_channel_config_t channel_green = {
        .gpio_num = 16,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };

    esp_err_t esito = ledc_channel_config(&channel_green);

    if (esito != ESP_OK) {
        return esito;
    }

     const ledc_channel_config_t channel_red = {
        .gpio_num = 15,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };

    return ledc_channel_config(&channel_red);

}
void led_pwm_set_duty(Led led, uint32_t duty) {
    ledc_channel_t channel;
    switch (led) {
        case LED_ROSSO:
            channel = LEDC_CHANNEL_1;
            break;
        case LED_VERDE:
            channel = LEDC_CHANNEL_0;
            break;
        default:
            return;
    }

    ESP_ERROR_CHECK(ledc_set_duty(
            LEDC_LOW_SPEED_MODE, channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(
        LEDC_LOW_SPEED_MODE, channel));
}

