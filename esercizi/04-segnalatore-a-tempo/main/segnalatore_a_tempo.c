#include <iso646.h>
#include <stdio.h>

#include "led.h"
#include "pulsante.h"
#include "segnalatore.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

void pulsante_task(void *arg);

void app_main(void)
{
    ESP_ERROR_CHECK(led_init());
    ESP_ERROR_CHECK(pulsante_init());

    QueueHandle_t coda_pressioni = xQueueCreate(1, sizeof(EventoPressione));
    if (coda_pressioni == NULL) {
        printf("Creazione della coda fallita\n");
        return;
    }

    TaskHandle_t handle_pulsante = NULL;
    BaseType_t esito = xTaskCreate(pulsante_task, "pulsante_task",
                                  2048, coda_pressioni, 5, &handle_pulsante);

    if (esito != pdPASS) {
        printf("Creazione di pulsante_task fallita\n");
        return;
    }

    esito = xTaskCreate(segnalatore_task, "segnalatore_task", 2048, coda_pressioni, 5, NULL);

    if (esito != pdPASS) {
        printf("Creazione di segnalatore_task fallita\n");
        return;
    }

    ESP_ERROR_CHECK(pulsante_abilita_interrupt(handle_pulsante));
}

void pulsante_task(void *arg) {
    QueueHandle_t coda_pressioni = (QueueHandle_t)arg;
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(30));
        if (pulsante_is_pressed()) {
            EventoPressione evento_pressione = {
                .istante = xTaskGetTickCount()
            };
            xQueueOverwrite(coda_pressioni, &evento_pressione);
            printf("Bottone Premuto\n");
            bool flag = true;
            int counter = 0;

            while (flag) {
                vTaskDelay(pdMS_TO_TICKS(20));
                if (!pulsante_is_pressed()) {
                    counter++;
                    if (counter == 3) {
                        flag = false;
                    }
                } else {
                    counter = 0;
                }
            }
        }
    }
}
