idf_component_register(SRCS "main.c" "led_pwm.c"
                       PRIV_REQUIRES esp_driver_ledc)
