#include "driver_motori.h"
#include "esp_err.h"
#include "driver/ledc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void init_timer();

void app_main(void) {
    init_timer();
    ESP_ERROR_CHECK(driver_motori_init_config());

    int percentuale_start = 25;
    uint32_t duty = (1u << 10) * (uint32_t)percentuale_start / 100u;
    driver_motori_init_value(duty);

    vTaskDelay(pdMS_TO_TICKS(2000));

    driver_motori_stop();

    vTaskDelay(pdMS_TO_TICKS(1000));

    driver_motori_backward(duty);

    vTaskDelay(pdMS_TO_TICKS(2000));

    driver_motori_stop();
}

static void init_timer() {
    const ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));
}
