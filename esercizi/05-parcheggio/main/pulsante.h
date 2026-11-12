//
// Created by Luigi Massa on 11/11/2026.
//

#ifndef PARCHEGGIO_PULSANTE_H
#define PARCHEGGIO_PULSANTE_H
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum {
    PULSANTE_ENTRATA,
    PULSANTE_USCITA,
} Pulsante;



esp_err_t pulsante_init(void);
bool pulsante_is_pressed(Pulsante pulsante, TaskHandle_t handle_gestore);
esp_err_t pulsante_abilita_interrupt(TaskHandle_t task);


#endif //PARCHEGGIO_PULSANTE_H
