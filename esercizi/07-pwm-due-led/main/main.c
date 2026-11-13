#include "driver/ledc.h"
#include "esp_err.h"
#include "led_pwm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void init_timer();

void gestore_pwm(void *arg);


void app_main(void) {
    init_timer();
    ESP_ERROR_CHECK(led_pwm_init());

    BaseType_t esito = xTaskCreate(gestore_pwm, "led_pwm", 4096, NULL, 10, NULL);

    if (esito != pdPASS) {
        printf("Errore\n");
        return;
    }

    printf("ciao\n");

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

void gestore_pwm(void *arg) {
    int percentuale_green = 25;
    int percentuale_red = 75;
    bool flag = true;

    while (flag) {
        printf("ENTRO\n");
        uint32_t duty_green = (1u << 10) * (uint32_t)percentuale_green / 100u;
        uint32_t duty_red = (1u << 10) * (uint32_t)percentuale_red / 100u;

        led_pwm_set_duty(LED_VERDE, duty_green);
        led_pwm_set_duty(LED_ROSSO, duty_red);
        vTaskDelay(pdMS_TO_TICKS(2000));

        percentuale_green += 50;
        percentuale_red -= 50;

        if (percentuale_green > 100) {
            flag = false;
        }
    }
    printf("ESCO\n");
    led_pwm_set_duty(LED_VERDE, 0);
    led_pwm_set_duty(LED_ROSSO, 0);

    vTaskDelete(NULL);
}
