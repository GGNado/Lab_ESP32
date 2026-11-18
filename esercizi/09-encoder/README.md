# 09 · Encoder della ruota

Il KY-040 di Wokwi genera il segnale di prova. Il canale A (CLK) arriva al GPIO 4 e al canale D0 del Logic Analyzer. Il canale B (DT) servirà nella prossima prova.

1. Scrivi il firmware in `main/main.c`: configura GPIO 4 come ingresso, conta i fronti di salita e stampa il totale fuori dall’ISR.
2. Avvia la simulazione e aspetta che il firmware sia pronto. Fai cinque scatti sulla freccia della manopola, uno alla volta.
3. Ferma Wokwi e apri il VCD in PulseView. Confronta il numero di fronti di salita con il conteggio in console.

Per compilare dalla cartella del progetto:

```sh
source "$HOME/.espressif/tools/activate_idf_v6.1.sh"
idf.py -B cmake-build-debug-esp-idf build merge-bin
```

[Encoder KY-040 in Wokwi](https://docs.wokwi.com/parts/wokwi-ky-040) · [Manuale e PulseView](../../docs/Manuale_API_ESP32_FreeRTOS.html#pulseview)
