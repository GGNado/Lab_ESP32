//
// Created by Luigi Massa on 13/11/2026.
//

#ifndef COMANDO_MOTORE_DRIVER_MOTORI_H
#define COMANDO_MOTORE_DRIVER_MOTORI_H
#include "esp_err.h"

esp_err_t driver_motori_init_config();
void driver_motori_init_value(uint32_t dutyStart);
void driver_motori_stop();
void driver_motori_forward(uint32_t duty);
void driver_motori_backward(uint32_t duty);

#endif //COMANDO_MOTORE_DRIVER_MOTORI_H
