#include <sys/stat.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#define ENCODER_A_GPIO GPIO_NUM_4
#define ENCODER_B_GPIO GPIO_NUM_5

typedef enum {
    AVANTI,
    STALLO,
    INDIETRO
} STATE;

typedef struct {
    int conteggio;
    STATE state;

} Encoder;

static void interrupt(void *args);
static const char *stato_to_string(STATE stato);

void app_main(void) {
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << ENCODER_A_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };

    gpio_config(&config);

    gpio_config_t configB = {
        .pin_bit_mask = (1ULL << ENCODER_B_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&configB);

    esp_err_t esito = gpio_install_isr_service(0);
    if (esito != ESP_OK) {
        return;
    }

    Encoder encoder = {
        .conteggio = 0,
        .state = STALLO,
    };
    esito = gpio_isr_handler_add(ENCODER_A_GPIO, interrupt, &encoder);

    if (esito != ESP_OK) {
        return;
    }

    int lastCount = 0;
    double rpm = 0.0;

    TickType_t time = xTaskGetTickCount();

    // Aggiungi qui la configurazione del comando motore.
    while (1) {
        // Dopo la misura RPM, aggiorna il duty nel task.
        TickType_t ora = xTaskGetTickCount();
        TickType_t tick_trascorsi = ora - time;
        double secondi_trascorsi = tick_trascorsi / (double)configTICK_RATE_HZ;

        if (secondi_trascorsi > 0.0) {
            rpm = ((encoder.conteggio - lastCount) / 20.0)
                  / secondi_trascorsi * 60.0;
        }

        printf("Conteggio: %d | Verso: %s | Last: %d\n",
        encoder.conteggio, stato_to_string(encoder.state), lastCount);
        printf("Gradi: %d\n", encoder.conteggio * 360 / 20);
        printf("RPM: %.1f\n", rpm);

        lastCount = encoder.conteggio;
        time = ora;
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void interrupt(void *args) {
    Encoder *en = (Encoder *)args;

    if (gpio_get_level(ENCODER_B_GPIO) == 0) {
        en->conteggio++;
        en->state = AVANTI;
    } else {
        en->conteggio--;
        en->state = INDIETRO;
    }

    if (en->conteggio == 0) {
        en->state = STALLO;
    }

}

static const char *stato_to_string(STATE stato)
{
    switch (stato) {
        case AVANTI:   return "Avanti";
        case INDIETRO: return "Indietro";
        case STALLO:    return "Stallo";
        default:       return "Sconosciuto";
    }
}
