//
// Created by Luigi Massa on 13/11/2026.
//

#ifndef PWM_DUE_LED_LED_PWM_H
#define PWM_DUE_LED_LED_PWM_H
#include "esp_err.h"

typedef enum {
    LED_VERDE,
    LED_ROSSO,
} Led;

esp_err_t led_pwm_init();
void led_pwm_set_duty(Led led, uint32_t duty);

#endif //PWM_DUE_LED_LED_PWM_H
