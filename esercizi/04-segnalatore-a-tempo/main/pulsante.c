#include "pulsante.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const gpio_num_t PIN_PULSANTE = GPIO_NUM_4;

static void pulsante_isr(void *arg);

esp_err_t pulsante_init(void) {
    gpio_config_t pulsante_config = {
        .pin_bit_mask = (1ULL << PIN_PULSANTE),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t esito = gpio_config(&pulsante_config);
    if (esito != ESP_OK) {
        return esito;
    }

    return ESP_OK;

}

bool pulsante_is_pressed(void) {
    int stato = gpio_get_level(PIN_PULSANTE);
    return (stato == 0);
}

bool pulsante_just_pressed(void) {
    static bool stato_confermato = false;
    static int contatore = 0;
    bool stato_corrente = pulsante_is_pressed();

    if (stato_corrente == stato_confermato) {
        contatore = 0;
        return false;
    }

    contatore++;
    if (contatore < 3) {
        return false;
    }

    stato_confermato = stato_corrente;
    contatore = 0;
    return stato_confermato;
}

esp_err_t pulsante_abilita_interrupt(TaskHandle_t task_handle) {
    esp_err_t esito = gpio_install_isr_service(0);
    if (esito != ESP_OK) {
        return esito;
    }

    esito = gpio_set_intr_type(PIN_PULSANTE, GPIO_INTR_NEGEDGE);
    if (esito != ESP_OK) {
        return esito;
    }

    return gpio_isr_handler_add(PIN_PULSANTE, pulsante_isr, task_handle);
}

static void pulsante_isr(void *arg) {
    TaskHandle_t task_handle = (TaskHandle_t)(arg);
    BaseType_t task_svegliato = pdFALSE;

    vTaskNotifyGiveFromISR(task_handle, &task_svegliato);

    if (task_svegliato == pdTRUE) {
        portYIELD_FROM_ISR();
    }

}
