# 06 · Esempio PWM su un LED

Il programma in `main/main.c` genera un PWM a **1 kHz**, con **10 bit** di risoluzione. Il LED verde è collegato al **GPIO 16** con una resistenza da **1 kΩ**. Il duty cycle sale da 0% a 100% e poi scende, con passi del 5% ogni 50 ms.

Consulta la configurazione del timer per frequenza e risoluzione, quella del canale per il collegamento al pin e le chiamate `ledc_set_duty` e `ledc_update_duty` per modificare il duty cycle.

Dalla cartella dell'esercizio, con ESP-IDF attivo, genera il firmware prima di avviare Wokwi.

```sh
idf.py -B cmake-build-debug-esp-idf build merge-bin
```

[Manuale ESP-IDF e FreeRTOS](../../docs/Manuale_API_ESP32_FreeRTOS.html)
