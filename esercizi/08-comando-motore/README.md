# 08 · Comando di un motore DC

Il Logic Analyzer osserva i tre segnali che l'ESP32 invierebbe a un driver motore.

| Segnale | GPIO ESP32-S3 | Logic Analyzer |
| --- | ---: | --- |
| PWM | 4 | D0 |
| IN1 | 1 | D1 |
| IN2 | 2 | D2 |

Prima prova. Configura GPIO 1 e 2 come uscite digitali e imposta IN1 alto e IN2 basso. Genera sul GPIO 4 un PWM al 25% con LEDC. Usa 1 kHz come frequenza di prova. Nel tracciato D1 deve restare alto, D2 basso e D0 alto per circa 250 µs ogni millisecondo. In una seconda prova aggiungerai lo stop e l'inversione.

Dalla cartella dell'esercizio, con ESP-IDF attivo:

```sh
idf.py -B cmake-build-debug-esp-idf build merge-bin
```

Avvia Wokwi, lascia girare la simulazione e fermala per esaminare la cattura dell'analizzatore.
