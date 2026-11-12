idf_component_register(SRCS "parcheggio.c" "led.c" "pulsante.c"
                    PRIV_REQUIRES esp_driver_gpio
                    INCLUDE_DIRS ".")
