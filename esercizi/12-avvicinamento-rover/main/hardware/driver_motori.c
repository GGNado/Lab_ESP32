//
// Created by Luigi Massa on 13/11/2026.
//

#include "driver_motori.h"

#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

#define MOTOR_PWM_GPIO  GPIO_NUM_19
#define MOTOR_IN1_GPIO  GPIO_NUM_20
#define MOTOR_IN2_GPIO  GPIO_NUM_21

esp_err_t driver_motori_init_config() {
    const gpio_config_t config = {
        .pin_bit_mask = (1ULL << MOTOR_IN1_GPIO) | (1ULL << MOTOR_IN2_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t esito = gpio_config(&config);

    if (esito != ESP_OK) {
        return esito;
    }

    const ledc_channel_config_t channel = {
        .gpio_num = 19,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };

    return ledc_channel_config(&channel);

}
void driver_motori_init_value(uint32_t dutyStart) {
    gpio_set_level(MOTOR_IN1_GPIO, 1);
    gpio_set_level(MOTOR_IN2_GPIO, 0);

    ESP_ERROR_CHECK(ledc_set_duty(
        LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, dutyStart));
    ESP_ERROR_CHECK(ledc_update_duty(
        LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}
void driver_motori_stop() {
    gpio_set_level(MOTOR_IN1_GPIO, 0);
    gpio_set_level(MOTOR_IN2_GPIO, 0);
    ESP_ERROR_CHECK(ledc_set_duty(
        LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
    ESP_ERROR_CHECK(ledc_update_duty(
        LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}
void driver_motori_forward(uint32_t duty) {
    gpio_set_level(MOTOR_IN1_GPIO, 1);
    gpio_set_level(MOTOR_IN2_GPIO, 0);
    ESP_ERROR_CHECK(ledc_set_duty(
        LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
    ESP_ERROR_CHECK(ledc_update_duty(
        LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}
void driver_motori_backward(uint32_t duty) {
    gpio_set_level(MOTOR_IN1_GPIO, 0);
    gpio_set_level(MOTOR_IN2_GPIO, 1);
    ESP_ERROR_CHECK(ledc_set_duty(
        LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
    ESP_ERROR_CHECK(ledc_update_duty(
        LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}