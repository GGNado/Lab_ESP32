# 01 - GPIO, task e pulsante

## Obiettivo

Osservare due attività FreeRTOS: lampeggio di un LED e lettura di un pulsante con conferma dello stato mediante campionamento periodico.

## Codice archiviato

Il contenuto di `main/hello_world_main.c` è una copia byte per byte di `ricorda.c`, salvato nel progetto originale. È cambiata soltanto la collocazione del file, così può essere costruito come progetto indipendente con la configurazione CMake già presente.

## Circuito

- ESP32-S3.
- LED su GPIO 4, con resistenza da 330 ohm in serie e catodo a GND.
- Pulsante su GPIO 5, verso GND, con pull-up **esterna da 10 kohm**.
- Il circuito completo è in `diagram.json`.

La pull-up esterna è importante: nel sorgente archiviato l'abilitazione di quella interna è soltanto una riga commentata.

## Comportamento del sorgente

- Task LED: 500 ms acceso e 500 ms spento, priorità 5.
- Task pulsante: lettura ogni 10 ms e conferma del cambio dopo campioni coerenti per la soglia didattica di 30 ms, priorità 3.
- Stampa del conteggio delle pressioni e del rilascio.

Il conteggio temporale del debounce è basato sul periodo nominale di campionamento, non su una misura rigorosa di tutto il tempo reale trascorso.

## Avvio e osservazioni

Segui le istruzioni della repository per compilazione e Wokwi. Verifica il lampeggio mentre premi il pulsante e osserva i messaggi nella console. I task non hanno un'affinità di core imposta nel codice: non usare questo esercizio come dimostrazione di scheduling su un solo core.

La versione recuperata è conservata come materiale didattico; non è stata eseguita una nuova prova funzionale durante l'organizzazione della repository.
