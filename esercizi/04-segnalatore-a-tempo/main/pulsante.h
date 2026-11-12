#ifndef SEGNALATORE_PULSANTE_H
#define SEGNALATORE_PULSANTE_H

#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t pulsante_init(void);
bool pulsante_is_pressed(void);
bool pulsante_just_pressed(void);
esp_err_t pulsante_abilita_interrupt(TaskHandle_t task);
#endif
