// Le righe #ifndef, #define e #endif
// formano l’include guard.
// Se lo stesso header viene incluso più volte durante
// la compilazione di un .c, il suo contenuto viene
// letto una volta sola.

#ifndef SEGNALATORE_LED_H
#define SEGNALATORE_LED_H

#include <stdbool.h>
#include "esp_err.h"

typedef enum {
    LED_VERDE,
    LED_ROSSO
} Led;

esp_err_t led_init(void);
esp_err_t led_set(Led led, bool stato);

#endif