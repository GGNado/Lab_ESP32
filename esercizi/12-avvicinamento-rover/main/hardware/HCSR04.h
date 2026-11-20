//
// Created by Luigi Massa on 20/11/2026.
//

#ifndef AVVICINAMENTO_ROVER_HCSR04_H
#define AVVICINAMENTO_ROVER_HCSR04_H

#include "esp_err.h"
#include "driver/gpio.h"

typedef struct {
    gpio_num_t trig_pin;
    gpio_num_t echo_pin;
} HCSR04;

esp_err_t hcsr04_init(HCSR04 *hcsr04);
int hcsr04_distance(const HCSR04 *hcsr04);

#endif //AVVICINAMENTO_ROVER_HCSR04_H
