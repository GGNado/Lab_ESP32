//
// Created by Luigi Massa on 11/11/2026.
//

#include "pulsante.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "FreeRTOSConfig.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include "driver/gpio.h"


#define PIN_PULSANTE_USCITA GPIO_NUM_46
#define PIN_PULSANTE_ENTRATA GPIO_NUM_4

static bool rileva_pressione(gpio_num_t pin, bool *stato_precedente, Pulsante pulsante, TaskHandle_t handle_gestore);
static bool is_debounced(gpio_num_t pin, int lettura);
static void pulsante_isr(void *arg);
static const char *pulsante_to_string(Pulsante pulsante);

typedef struct {
    TaskHandle_t task;
    uint32_t bit;
}PulsanteContesto;


esp_err_t pulsante_init(void) {
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << PIN_PULSANTE_ENTRATA) | (1ULL << PIN_PULSANTE_USCITA),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    return gpio_config(&config);
}

bool pulsante_is_pressed(Pulsante pulsante, TaskHandle_t handle_gestore) {
    static bool last_state[2] = {false, false};
   switch (pulsante) {
       case PULSANTE_ENTRATA:
            return rileva_pressione(PIN_PULSANTE_ENTRATA, &last_state[0], PULSANTE_ENTRATA, handle_gestore);
       case PULSANTE_USCITA:
           return rileva_pressione(PIN_PULSANTE_USCITA, &last_state[1], PULSANTE_USCITA, handle_gestore);
       default:
           return false;
   }
}


esp_err_t pulsante_abilita_interrupt(TaskHandle_t task_handle) {
    esp_err_t esito = gpio_install_isr_service(0);
    if (esito != ESP_OK) {
        return esito;
    }

    esito = gpio_set_intr_type(PIN_PULSANTE_ENTRATA, GPIO_INTR_ANYEDGE);
    if (esito != ESP_OK) {
        return esito;
    }

    static PulsanteContesto contesto_entrata = {
        nullptr, 0

    };
    contesto_entrata.task = task_handle;
    contesto_entrata.bit = (1u << 0);

    esito = gpio_isr_handler_add(PIN_PULSANTE_ENTRATA, pulsante_isr, &contesto_entrata);
    if (esito != ESP_OK) {
        return esito;
    }

    esito = gpio_set_intr_type(PIN_PULSANTE_USCITA, GPIO_INTR_ANYEDGE);
    if (esito != ESP_OK) {
        return esito;
    }

    static PulsanteContesto contesto_uscita = {
        nullptr, 0
    };

    contesto_uscita.bit = (1u << 1);
    contesto_uscita.task = task_handle;

    esito = gpio_isr_handler_add(PIN_PULSANTE_USCITA, pulsante_isr, &contesto_uscita);
    if (esito != ESP_OK) {
        return esito;
    }


    return ESP_OK;
}

// FUNZIONI STATIC HELPER

static bool rileva_pressione(gpio_num_t pin, bool *stato_precedente, Pulsante pulsante, TaskHandle_t handle_gestore) {
    int lettura = gpio_get_level(pin);

    if (*stato_precedente == false && lettura == 1) return false;
    if (lettura == 0 && *stato_precedente == true) return false;
    if (lettura == 0 && *stato_precedente == false){
        if (is_debounced(pin, lettura)) return false;

        *stato_precedente = true;
        printf("Hai premuto %s\n", pulsante_to_string(pulsante));
        return true;
    }

    if (lettura == 1 && *stato_precedente == true) {
        if (is_debounced(pin, lettura)) return false;

        *stato_precedente = false;
        printf("Hai rilasciato %s\n", pulsante_to_string(pulsante));
        return false;
    }

    return false;
}


static const char *pulsante_to_string(Pulsante pulsante) {
    switch (pulsante) {
        case PULSANTE_ENTRATA:
            return "ENTRATA";
        case PULSANTE_USCITA:
            return "USCITA";
        default:
            return "SCONOSCIUTO";
    }
}

static bool is_debounced(gpio_num_t pin, int lettura) {
    vTaskDelay(pdMS_TO_TICKS(10));
    return lettura != gpio_get_level(pin);

}

static void pulsante_isr(void *arg) {
const PulsanteContesto *contesto = (const PulsanteContesto *)arg;
    BaseType_t task_svegliato = pdFALSE;

    xTaskNotifyFromISR(contesto->task, contesto->bit, eSetBits, &task_svegliato);

    if (task_svegliato == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

