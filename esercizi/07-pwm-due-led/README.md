# 07 · PWM su due LED

In Wokwi il LED verde è sul GPIO 16 e il LED rosso sul GPIO 15. Entrambi hanno una resistenza in serie. Usa la stessa frequenza PWM per entrambi.

| Fase | Verde | Rosso |
| --- | ---: | ---: |
| Primi 2 secondi | 25% | 75% |
| Altri 2 secondi | 75% | 25% |
| Fine | Spento | Spento |

Esegui la sequenza una volta sola. Scrivi il tuo tentativo in `main/main.c`. Puoi consultare l'[esempio 06](../06-pwm-led/) e il [manuale](../../docs/Manuale_API_ESP32_FreeRTOS.html).

Dalla cartella dell'esercizio, con ESP-IDF attivo, genera il firmware prima di avviare Wokwi.

```sh
idf.py -B cmake-build-debug-esp-idf build merge-bin
```
