// comando.h
#pragma once

#include <stdint.h>

typedef enum {
    Comando_STOP,
    Comando_FORWARD,
    Comando_BACKWARD,
    Comando_ROTATE_LEFT_FORWARD,
    Comando_ROTATE_RIGHT_FORWARD,
    Comando_ROTATE_LEFT_BACKWARD,
    Comando_ROTATE_RIGHT_BACKWARD,
} COMANDO_TYPE;

typedef struct {
    COMANDO_TYPE comando;
    uint32_t duty;
} Comando;

const char *comando_to_string(COMANDO_TYPE tipo);