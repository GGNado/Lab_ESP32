//
// Created by Luigi Massa on 10/11/2026.
//

#include "segnalatore.h"
#include <stdio.h>

#include "led.h"
#include "freertos/queue.h"

void segnalatore_task(void *arg) {
    QueueHandle_t coda_pressioni = (QueueHandle_t)arg;
    EventoPressione evento_pressione = { 0 };
    bool ricevuto = false;
    TickType_t riferimento = xTaskGetTickCount();
    const TickType_t periodo = pdMS_TO_TICKS(20);
    const TickType_t durata = pdMS_TO_TICKS(3000);

    while (true) {
        if (xQueueReceive(coda_pressioni, &evento_pressione, 0) == pdPASS) {
            ricevuto = true;
            printf("Pressione ricevuta al tick %lu\n",
                   (unsigned long)evento_pressione.istante);
        }

        if (ricevuto) {
            TickType_t eta = xTaskGetTickCount() - evento_pressione.istante;
            if (eta < durata) {
                led_set(LED_VERDE, true);
                led_set(LED_ROSSO, false);
            } else {
                led_set(LED_VERDE, false);
                led_set(LED_ROSSO, true);
            }
        }

        xTaskDelayUntil(&riferimento, periodo);
    }
}
