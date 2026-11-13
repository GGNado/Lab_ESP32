#include <stdio.h>

#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    const ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));

    const ledc_channel_config_t channel = {
        .gpio_num = 16,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel));

    int percentuale = 0;
    int passo = 5;

    while (1) {
        uint32_t duty = (1u << 10) * (uint32_t)percentuale / 100u;
        printf("%lu\n", duty);

        ESP_ERROR_CHECK(ledc_set_duty(
            LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
        ESP_ERROR_CHECK(ledc_update_duty(
            LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

        vTaskDelay(pdMS_TO_TICKS(50));

        if (percentuale == 100) {
            passo = -5;
        } else if (percentuale == 0) {
            passo = 5;
        }

        percentuale += passo;
    }
}