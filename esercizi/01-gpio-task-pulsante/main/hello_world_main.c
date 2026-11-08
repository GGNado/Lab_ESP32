/*
* SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_4
#define BTN_PIN GPIO_NUM_5

#define BLINK_MS     500
#define POLL_MS      10
#define DEBOUNCE_MS  30

// Task 1: lampeggio LED, indipendente dal resto
static void blink_task(void *arg) {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (true) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(BLINK_MS));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(BLINK_MS));
    }
}

// Task 2: lettura pulsante con debounce e conteggio pressioni
static void button_task(void *arg) {
    gpio_reset_pin(BTN_PIN);
    gpio_set_direction(BTN_PIN, GPIO_MODE_INPUT);
    // Pull-up esterno (r2) presente nel circuito.
    // Senza r2, decommenta:
    // gpio_set_pull_mode(BTN_PIN, GPIO_PULLUP_ONLY);

    int stable_level = 1;        // stato stabile (1 = rilasciato)
    int last_raw = 1;
    int stable_ms = 0;
    int press_count = 0;

    while (true) {
        int raw = gpio_get_level(BTN_PIN);

        if (raw == last_raw) {
            if (stable_ms < DEBOUNCE_MS) stable_ms += POLL_MS;
        } else {
            last_raw = raw;
            stable_ms = 0;
        }

        // Cambio di stato confermato dopo il debounce
        if (stable_ms >= DEBOUNCE_MS && raw != stable_level) {
            stable_level = raw;
            if (stable_level == 0) {
                press_count++;
                printf("Pulsante PREMUTO (pressione #%d)\n", press_count);
            } else {
                printf("Pulsante RILASCIATO\n");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(POLL_MS));
    }
}

void app_main(void) {
    xTaskCreate(blink_task,  "blink",  2048, NULL, 5, NULL);
    xTaskCreate(button_task, "button", 2048, NULL, 3, NULL);
}