# 02 - Comandi del rover, timeout e riferimenti ruote

## Obiettivo

Separare la produzione dei comandi dal controllo periodico: conservare l'ultimo aggiornamento, verificarne la validità e calcolare le velocità periferiche delle due ruote.

## Codice archiviato

`main/hello_world_main.c` è una copia byte per byte del sorgente attivo nel progetto originale al momento dell'archiviazione. Non sono state effettuate correzioni, refactoring o modifiche al comportamento.

## Circuito

Il LED su GPIO 4 è un indicatore software: non ci sono driver motori o motori collegati. Il circuito Wokwi conserva anche il pulsante su GPIO 5 e la sua pull-up, che questo esercizio non usa.

## Sequenza prodotta

| Tempo dall'inizio del produttore | v (m/s) | w (rad/s) |
|---|---:|---:|
| 0-2 s | 0,20 | 0 |
| 2-4 s | 0 | +1 |
| 4-6 s | 0,10 | -0,5 |
| Dopo 6 s | Nessun nuovo invio | |

Gli invii avvengono ogni circa 100 ms. Il produttore termina con `vTaskDelete(NULL)`.

## Controllo

- Coda da un elemento che contiene una struttura `ComandoRover` completa.
- Lettura non bloccante degli aggiornamenti.
- Ciclo periodico di 50 ms con `xTaskDelayUntil()`.
- Timeout di 500 ms misurato dal timestamp dell'ultimo comando ricevuto.
- Distanza tra le ruote utilizzata nel calcolo: 0,20 m.

| Fase | Sinistra (m/s) | Destra (m/s) |
|---|---:|---:|
| Avanti | 0,20 | 0,20 |
| Rotazione a sinistra | -0,10 | +0,10 |
| Curva a destra | 0,15 | 0,05 |
| Comando assente o scaduto | 0 | 0 |

## Risultato e limiti della fotografia archiviata

La sequenza e i riferimenti sono stati confermati durante le prove in Wokwi della sessione di studio. Non dimostrano uno stop fisico o una regolazione motori reale.

Nel codice salvato il LED si accende quando il comando è **valido** e si spegne quando è assente o scaduto. Non verifica separatamente che il comando valido richieda un movimento non nullo. Il log stampa ancora a ogni ciclo: la riduzione ai soli cambi di stato resta un possibile esercizio successivo.

Le creazioni della coda e dei task non hanno ancora controlli espliciti dell'esito. Queste osservazioni descrivono il sorgente esistente: non sono state applicate modifiche.

## Avvio

Apri questa cartella come progetto ESP-IDF e segui le istruzioni di build e simulazione nella radice della repository.
