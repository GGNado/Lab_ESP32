#include "esp_err.h"
#include "config/rover_config.h"
#include "hardware/led.h"
#include "hardware/HCSR04.h"
#include "controllo/comando.h"
#include "hardware/driver_motori.h"
#include "esp_random.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void comand_recived(void *arg);
void comand_validator(void *arg);

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

static void init(HCSR04 sens[], size_t n_sensori,
                 LED luci[], size_t n_led) {
    for (size_t i = 0; i < n_sensori; ++i) {
        ESP_ERROR_CHECK(hcsr04_init(&sens[i]));
    }

    for (size_t i = 0; i < n_led; ++i) {
        ESP_ERROR_CHECK(led_init(&luci[i]));
    }

    for (size_t i = 0; i < n_led; ++i) {
        led_set(&luci[i], false);
    }

    init_timer();
    driver_motori_init_config();
    driver_motori_init_value(0);
}

void app_main(void) {
    init(hcsr04_sensori, numero_sensori, leds, numero_led);
    QueueHandle_t coda = xQueueCreate(1, sizeof(Comando));

    BaseType_t esito = xTaskCreate(comand_recived, "comand_recived", 4096, coda, 10, NULL);
    if (esito != pdPASS) {
        return;
    }

    esito = xTaskCreate(comand_validator, "comand_validator", 4096, coda, 10, nullptr);
    if (esito != pdPASS) {
        return;
    }




}

void comand_recived(void *arg)
{
    QueueHandle_t coda = arg;

    static const COMANDO_TYPE comandi[] = {
        Comando_STOP,
        Comando_FORWARD,
        Comando_BACKWARD,
        Comando_ROTATE_LEFT_FORWARD,
        Comando_ROTATE_RIGHT_FORWARD,
        Comando_ROTATE_LEFT_BACKWARD,
        Comando_ROTATE_RIGHT_BACKWARD,
    };

    const size_t numero_comandi = sizeof(comandi) / sizeof(comandi[0]);

    // Circa 25–75% per un timer PWM a 8 bit.
    const uint32_t duty_min = 64U;
    const uint32_t duty_max = 192U;

    while (true) {
        const COMANDO_TYPE tipo = comandi[esp_random() % numero_comandi];

        const Comando comando = {
            .comando = tipo,
            .duty = (tipo == Comando_STOP)
                ? 0U
                : duty_min + esp_random() % (duty_max - duty_min + 1U),
        };

        xQueueOverwrite(coda, &comando);

        const uint32_t attesa_ms = 500U + esp_random() % 2501U;
        vTaskDelay(pdMS_TO_TICKS(attesa_ms));
    }
}

void comand_validator(void *arg) {
    QueueHandle_t coda = (QueueHandle_t) arg;
    Comando comando;

    while (true) {
        if (xQueueReceive(coda, &comando, pdMS_TO_TICKS(5000)) == pdPASS) {
            printf("Comando ricevuto: %s\n", comando_to_string(comando.comando));
            switch (comando.comando) {
                case Comando_STOP:
                    led_set(&leds[0], false);
                    led_set(&leds[1], false);
                    led_set(&leds[2], true);
                    driver_motori_stop();
                    break;

                case Comando_FORWARD:
                case Comando_ROTATE_LEFT_FORWARD:
                case Comando_ROTATE_RIGHT_FORWARD:
                    if (hcsr04_distance(&hcsr04_sensori[0]) < 20) {
                        driver_motori_stop();
                        led_set(&leds[0], false);
                        led_set(&leds[1], true);
                        led_set(&leds[2], false);
                    } else {
                        driver_motori_forward(comando.duty);
                        led_set(&leds[0], true);
                        led_set(&leds[1], false);
                        led_set(&leds[2], false);
                    }
                    break;

                case Comando_BACKWARD:
                case Comando_ROTATE_LEFT_BACKWARD:
                case Comando_ROTATE_RIGHT_BACKWARD:
                    if (hcsr04_distance(&hcsr04_sensori[1]) < 20) {
                        driver_motori_stop();
                        led_set(&leds[0], false);
                        led_set(&leds[1], true);
                        led_set(&leds[2], false);
                    } else {
                        driver_motori_backward(comando.duty);
                        led_set(&leds[0], true);
                        led_set(&leds[1], false);
                        led_set(&leds[2], false);
                    }
                    break;
                default:
                    led_set(&leds[0], true);
                    led_set(&leds[1], true);
                    led_set(&leds[2], true);
                    driver_motori_stop();
            }
        } else {
            driver_motori_stop();
        }
    }
}