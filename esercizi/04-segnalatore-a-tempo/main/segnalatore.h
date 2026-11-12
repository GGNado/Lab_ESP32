//
// Created by Luigi Massa on 10/11/2026.
//

#ifndef SEGNALATORE_A_TEMPO_SEGNALATORE_H
#define SEGNALATORE_A_TEMPO_SEGNALATORE_H

#include "freertos/FreeRTOS.h"

typedef struct {
    TickType_t istante;
} EventoPressione;

void segnalatore_task(void *arg);

#endif //SEGNALATORE_A_TEMPO_SEGNALATORE_H
