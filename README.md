# Lab_ESP32

Quaderno pratico di esercizi su **ESP32-S3, ESP-IDF e FreeRTOS**, con circuiti virtuali Wokwi. Ogni esercizio è un progetto indipendente, da riaprire e compilare senza sovrascrivere gli altri.

## Esercizi disponibili

| Esercizio | Argomenti |
|---|---|
| [01 - GPIO, task e pulsante](esercizi/01-gpio-task-pulsante/) | LED, polling del pulsante, debounce a campioni e due task. |
| [02 - Comandi rover](esercizi/02-comandi-rover/) | Coda da un elemento, ultimo comando, timeout e riferimenti delle ruote. |
| [03 - Tempi di controllo del rover](esercizi/03-tempi-controllo-rover/) | Task periodico, misure in tick e microsecondi, effetto dei ritardi sul ciclo. |
| [04 - Segnalatore a tempo](esercizi/04-segnalatore-a-tempo/) | Interrupt GPIO, debounce nel task, coda delle pressioni e LED temporizzato. |
| [05 - Parcheggio](esercizi/05-parcheggio/) | Due pulsanti, notifiche con `eSetBits`, conteggio dei posti e LED giallo temporizzato in un task dedicato. |
| [06 - PWM su un LED](esercizi/06-pwm-led/) | Esempio LEDC con timer, canale e variazione del duty cycle. |
| [07 - PWM su due LED](esercizi/07-pwm-due-led/) | Timer condiviso e duty cycle indipendenti su due canali. |
| [08 - Comando motore](esercizi/08-comando-motore/) | PWM e segnali di direzione, con verifica di avanti, pausa, indietro e stop in PulseView. |
| [09 - Encoder](esercizi/09-encoder/) | Conteggio dei fronti di salita e rilevamento del verso con i canali A e B del KY-040. |

## Struttura

```text
Lab_ESP32/
├── esercizi/   # Progetti ESP-IDF indipendenti con configurazione Wokwi
├── docs/       # Manuale API e modello per registrare le prove
└── README.md
```

## Come aprire un esercizio

1. Installa ESP-IDF **6.1**, con target **ESP32-S3**.
2. In CLion apri la cartella del singolo esercizio, non la radice della repository come se fosse un unico progetto CMake.
3. Usa la toolchain ESP-IDF. Il file ambiente della nostra installazione EIM è `~/.espressif/tools/activate_idf_v6.1.sh`; il percorso può cambiare su altri computer.
4. Nel profilo CMake usa `-DIDF_TARGET=esp32s3` e la directory di build **`cmake-build-debug-esp-idf`**: i file Wokwi presenti puntano a quella cartella.
5. Dalla cartella dell'esercizio attiva l'ambiente e compila:

```bash
source ~/.espressif/tools/activate_idf_v6.1.sh
idf.py -B cmake-build-debug-esp-idf build merge-bin
```

6. Attiva la tua licenza del plugin Wokwi e avvia il simulatore usando `wokwi.toml` e `diagram.json` dell'esercizio.

Il firmware unificato deve essere rigenerato dopo le modifiche. Su GitHub non vengono caricati build, binari o impostazioni locali di CLion: si generano sul computer che apre il progetto.

## Materiale di consultazione

- [Manuale interattivo ESP-IDF e FreeRTOS](docs/Manuale_API_ESP32_FreeRTOS.html) · Scarica il file da GitHub e aprilo nel browser per usare la ricerca o stamparlo. Include schede API, una guida alle ISR e alle notifiche, e la tabella di `eSetBits` con l’esempio del parcheggio.
- [Misurare i segnali con PulseView](docs/Manuale_API_ESP32_FreeRTOS.html#pulseview) · Esportazione VCD da Wokwi e misure di frequenza, duty cycle e direzione nel laboratorio 08.
- [Riferimento API in Markdown](docs/Riferimento_API_ESP32_FreeRTOS.md)
- [Riferimento API in PDF](docs/Riferimento_API_ESP32_FreeRTOS.pdf)
- [Modello per registrare una prova](docs/registro_prova_template.md)

## Come aggiungere il prossimo esercizio

Crea una nuova cartella numerata dentro `esercizi/`, con un progetto ESP-IDF autonomo e un breve README: obiettivo, circuito, procedura, risultati e limiti. Conserva la versione degli esercizi precedenti; non riutilizzare ogni volta lo stesso file cancellando il lavoro precedente.

Prima di fare commit controlla `git status`: non aggiungere password, token, chiavi o parametri Wi-Fi privati nel codice o in `sdkconfig`. Il file `sdkconfig` è qui versionato per conservare la configurazione degli esercizi.

## Provenienza

Archivio iniziale creato l'8 novembre 2026 a partire dai file locali di `p00-hello`. Sono state conservate le configurazioni CMake e Wokwi e i sorgenti effettivamente disponibili. Gli esperimenti precedenti senza una versione salvata non sono stati ricostruiti artificialmente.
