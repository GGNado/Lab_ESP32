# 10 · Rotazione e velocità della ruota

Il circuito e il firmware di partenza sono quelli del 09. CLK arriva al GPIO 4 e a D0, DT al GPIO 5 e a D1.

1. Per la prima prova, assumi 20 conteggi per giro della ruota simulata. Usa il conteggio con segno per ricavare la rotazione in gradi rispetto alla posizione iniziale. Verifica che cinque conteggi nello stesso verso corrispondano a un quarto di giro e che cinque nel verso opposto riportino il risultato a zero.
2. Poi stima la velocità in RPM confrontando il conteggio in due istanti. Usa il tempo effettivamente trascorso tra le due letture. Verifica che il segno cambi invertendo verso e che la velocità torni a zero quando smetti di ruotare la manopola.

Stampa i risultati fuori dalla ISR. Un conteggio totale pari a zero indica il ritorno alla posizione iniziale. Per rilevare che la ruota è ferma, osserva se il conteggio resta invariato nell’intervallo di misura.

Apri la cartella in CLion e usa `cmake-build-debug-esp-idf` come directory di build. Compila dalla cartella dell’esercizio prima di avviare Wokwi.

```sh
source "$HOME/.espressif/tools/activate_idf_v6.1.sh"
idf.py -B cmake-build-debug-esp-idf build merge-bin
```
