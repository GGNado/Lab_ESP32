//
// Created by Luigi Massa on 20/11/2026.
//

#ifndef AVVICINAMENTO_ROVER_ROVER_CONFIG_H
#define AVVICINAMENTO_ROVER_ROVER_CONFIG_H

#pragma once

#include <stddef.h>
#include "../hardware/HCSR04.h"
#include "../hardware/led.h"

extern HCSR04 hcsr04_sensori[];
extern LED leds[];
extern const size_t numero_led;
extern const size_t numero_sensori;

#endif //AVVICINAMENTO_ROVER_ROVER_CONFIG_H
