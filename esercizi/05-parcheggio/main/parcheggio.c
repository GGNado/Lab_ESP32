#include <stdio.h>
#include <sys/stat.h>

#include "led.h"
#include "pulsante.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

void gestore_parcheggio(void *arg);
void gestore_led_giallo(void *arg);

typedef struct {
    TaskHandle_t task_1;
    TaskHandle_t task_2;
} TaskContesto;

void app_main(void) {
    ESP_ERROR_CHECK(led_init());
    ESP_ERROR_CHECK(pulsante_init());

    static TaskContesto contesto_gestore_parcheggio = {
        nullptr, nullptr
    };

    BaseType_t esito = xTaskCreate(
        gestore_parcheggio, "gestore_parcheggio_task", 2048, &contesto_gestore_parcheggio, 5, &contesto_gestore_parcheggio.task_1
    );

    if (esito != pdPASS) {
        printf("Errore nell'avvio del task\n");
        return;
    }

    ESP_ERROR_CHECK(pulsante_abilita_interrupt(contesto_gestore_parcheggio.task_1));


    esito = xTaskCreate(
        gestore_led_giallo, "gestore_led_giallo_task", 2048, nullptr, 5, &contesto_gestore_parcheggio.task_2
        );

    if (esito != pdPASS) {
        printf("Errore nell'avvio del task\n");
        return;
    }
}

void gestore_parcheggio(void *arg) {
    TaskContesto *contesto = (TaskContesto *)arg;
    static int conteggio = 0;
    static const int MAX_PARCHEGGI = 5;
    led_on(LED_GREEN);

    while (1) {
        uint32_t eventi = 0;
        xTaskNotifyWait(0, UINT32_MAX, &eventi, portMAX_DELAY);

        if (eventi & (1 << 0) && pulsante_is_pressed(PULSANTE_ENTRATA, contesto->task_1)) {
            if (conteggio == MAX_PARCHEGGI) {
                xTaskNotifyGive(contesto->task_2);
            } else {
                conteggio++;
                if (conteggio == MAX_PARCHEGGI) {
                    led_on(LED_RED);
                    led_off(LED_GREEN);
                }
            }
        }

        if (eventi & (1 << 1) && pulsante_is_pressed(PULSANTE_USCITA, contesto->task_1)) {

            if (conteggio == 0) {
                xTaskNotifyGive(contesto->task_2);
            } else {
                conteggio--;
                led_on(LED_GREEN);
                led_off(LED_RED);
            }
        }
    }
}


void gestore_led_giallo(void *arg) {
    while (1) {
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
            led_on(LED_YELLOW);
            while (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(2000)) != 0) {
            }
            led_off(LED_YELLOW);
        }
    }
}