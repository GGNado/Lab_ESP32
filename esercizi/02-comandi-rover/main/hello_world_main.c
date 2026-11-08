#include <iso646.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_4

void gpio_init(gpio_num_t gpio_num, gpio_mode_t mode);
void blink_led(void *arg);
void send_command(void *arg);


typedef  struct {
    float velocita_lineare;
    float velocita_angolare;
    TickType_t istante_ricevuto;
} ComandoRover;

void app_main() {
    QueueHandle_t coda_comandi = xQueueCreate(1, sizeof(ComandoRover));
    gpio_init(LED_PIN, GPIO_MODE_OUTPUT);

    xTaskCreate(send_command, "send_comandi",
            2048, coda_comandi, 5, NULL);

    xTaskCreate(blink_led, "blink_led",
        2048, coda_comandi, 5, NULL);
}

void gpio_init(gpio_num_t gpio_num, gpio_mode_t mode) {
    gpio_reset_pin(gpio_num);
    gpio_set_direction(gpio_num, mode);
}

void blink_led(void *arg)
{
    QueueHandle_t coda = (QueueHandle_t)arg;
    ComandoRover comando_rover = {0};
    bool ricevuto = false;
    float velocita_sx = 0.0f;
    float velocita_dx = 0.0f;

    TickType_t riferimento = xTaskGetTickCount();
    const TickType_t periodo = pdMS_TO_TICKS(50);
    const TickType_t timeout = pdMS_TO_TICKS(500);

    while (true) {
        if (xQueueReceive(coda, &comando_rover, 0) == pdPASS) {
            printf("Comando ricevuto\n");
            ricevuto = true;
        }

        bool valido = false;

        if (ricevuto) {
            TickType_t ora = xTaskGetTickCount();
            TickType_t eta = ora - comando_rover.istante_ricevuto;

            valido = eta < timeout;
        }

        if (valido) {
            velocita_dx = comando_rover.velocita_lineare + (comando_rover.velocita_angolare * 0.20f / 2);
            velocita_sx = comando_rover.velocita_lineare - (comando_rover.velocita_angolare * 0.20f / 2);
            gpio_set_level(LED_PIN, 1);
            printf("Velocità lineare: %f\n",
                   comando_rover.velocita_lineare);

            printf("Velocità angolare: %f\n",
                   comando_rover.velocita_angolare);

            printf("Istante: %lu\n",
                   (unsigned long)comando_rover.istante_ricevuto);

            printf("Velcità DX: %f\n", velocita_dx);
            printf("Velcità SX: %f\n", velocita_sx);
        } else {
            gpio_set_level(LED_PIN, 0);
            velocita_dx = 0.0f;
            velocita_sx = 0.0f;
            printf("Comando assente o scaduto: richiedo STOP\n");
        }

        xTaskDelayUntil(&riferimento, periodo);
    }
}

void send_command(void *arg) {
    printf("CI SIAMO");
    QueueHandle_t coda = (QueueHandle_t)arg;
    ComandoRover comando;
    TickType_t inizio = xTaskGetTickCount();

    // PRIMI DUE SECONDI
    while ((TickType_t)(xTaskGetTickCount() - inizio) < pdMS_TO_TICKS(2000)) {
        comando.velocita_lineare = 0.20f;
        comando.velocita_angolare = 0.0f;
        comando.istante_ricevuto = xTaskGetTickCount();
        if (xQueueOverwrite(coda, &comando) != pdPASS)
            printf("ERRORE INVIO");
        else
            printf("INVIATO\n");

        vTaskDelay(pdMS_TO_TICKS(100));
    }


    while ((TickType_t)(xTaskGetTickCount() - inizio) < pdMS_TO_TICKS(4000)) {
        comando.velocita_lineare = 0.0f;
        comando.velocita_angolare = 1.0f;
        comando.istante_ricevuto = xTaskGetTickCount();
        if (xQueueOverwrite(coda, &comando) != pdPASS)
            printf("ERRORE INVIO");
        else
            printf("INVIATO\n");

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    while ((TickType_t)(xTaskGetTickCount() - inizio) < pdMS_TO_TICKS(6000)) {

        comando.velocita_lineare = 0.10f;
        comando.velocita_angolare = -0.5f;
        comando.istante_ricevuto = xTaskGetTickCount();
        if (xQueueOverwrite(coda, &comando) != pdPASS)
            printf("ERRORE INVIO");
        else
            printf("INVIATO\n");
        vTaskDelay(pdMS_TO_TICKS(100));
        }

    printf("FINE INVIO\n");
    vTaskDelete(NULL);

}
