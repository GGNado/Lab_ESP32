idf_component_register(
        SRCS
        "main.c"
        "config/rover_config.c"
        "controllo/comando.c"
        "hardware/HCSR04.c"
        "hardware/driver_motori.c"
        "hardware/led.c"
        PRIV_INCLUDE_DIRS "config" "controllo" "hardware"
        PRIV_REQUIRES esp_driver_gpio esp_driver_ledc esp_timer
)