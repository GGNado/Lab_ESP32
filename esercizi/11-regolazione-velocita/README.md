# 11 · Regolazione della velocità di una ruota

Il firmware di partenza legge l’encoder e calcola gli RPM come nel 10. In Wokwi ruoti tu il KY-040 per simulare la velocità misurata. L’analizzatore osserva il comando che riceverebbe il driver. La manopola non reagisce automaticamente al PWM.

| Segnale | GPIO | Logic Analyzer |
| --- | ---: | --- |
| Encoder A (CLK) | 4 | D0 |
| Encoder B (DT) | 5 | D1 |
| PWM | 16 | D2 |
| IN1 | 1 | D3 |
| IN2 | 2 | D4 |

1. Riprendi la configurazione LEDC dell’08 e verifica su D2 un PWM fisso a 1 kHz. Per la prova usa solo il verso avanti.
2. Fissa un riferimento di 15 RPM e un duty iniziale intermedio. Fuori dalla ISR, confronta gli RPM misurati con il riferimento. Progetta una piccola correzione a ogni misura e limita il duty all’intervallo scelto. Stampa riferimento, misura e duty.
3. Con una finestra di circa 2 secondi e 20 conteggi per giro, prova 5, 10 e 15 scatti in avanti dentro singole finestre. Corrispondono a circa 7,5, 15 e 22,5 RPM. Verifica che il duty rispettivamente salga, resti stabile vicino al riferimento e scenda. Confronta la console con D2 in PulseView.

Apri la cartella in CLion e usa `cmake-build-debug-esp-idf` come directory di build.

```sh
source "$HOME/.espressif/tools/activate_idf_v6.1.sh"
idf.py -B cmake-build-debug-esp-idf build merge-bin
```
