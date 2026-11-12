idf_component_register(
        SRCS "segnalatore_a_tempo.c" "led.c" "pulsante.c" "segnalatore.c"
        PRIV_REQUIRES esp_driver_gpio
        INCLUDE_DIRS "."
)