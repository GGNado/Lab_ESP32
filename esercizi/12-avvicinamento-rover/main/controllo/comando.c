//
// Created by Luigi Massa on 20/11/2026.
//

#include "comando.h"

const char *comando_to_string(COMANDO_TYPE tipo)
{
    switch (tipo) {
        case Comando_STOP:                  return "STOP";
        case Comando_FORWARD:               return "FORWARD";
        case Comando_BACKWARD:              return "BACKWARD";
        case Comando_ROTATE_LEFT_FORWARD:   return "ROTATE_LEFT_FORWARD";
        case Comando_ROTATE_RIGHT_FORWARD:  return "ROTATE_RIGHT_FORWARD";
        case Comando_ROTATE_LEFT_BACKWARD:  return "ROTATE_LEFT_BACKWARD";
        case Comando_ROTATE_RIGHT_BACKWARD: return "ROTATE_RIGHT_BACKWARD";
        default:                           return "SCONOSCIUTO";
    }
}