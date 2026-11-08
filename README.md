# Lab_ESP32

Quaderno pratico di esercizi su **ESP32-S3, ESP-IDF e FreeRTOS**, con circuiti virtuali Wokwi. Ogni esercizio è un progetto indipendente, da riaprire e compilare senza sovrascrivere gli altri.

## Esercizi disponibili

| Cartella | Argomenti | Stato dell'archivio |
|---|---|---|
| [01 - GPIO, task e pulsante](esercizi/01-gpio-task-pulsante/) | LED, polling del pulsante, debounce a campioni, due task. | Versione recuperata dal file salvato `ricorda.c`. |
| [02 - Comandi rover](esercizi/02-comandi-rover/) | Coda da un elemento, ultimo comando, timeout, periodicità e riferimenti delle ruote. | Fotografia del codice presente l'8 novembre 2026. |

Sono esercizi didattici, non un firmware di produzione completo. Il codice è stato archiviato senza modifiche; la descrizione dell'esercizio distingue ciò che fa da eventuali miglioramenti successivi.

## Struttura

```text
Lab_ESP32/
├── esercizi/
│   ├── 01-gpio-task-pulsante/
│   │   ├── main/
│   │   │   ├── hello_world_main.c
│   │   │   └── CMakeLists.txt
│   │   ├── CMakeLists.txt
│   │   ├── diagram.json
│   │   ├── wokwi.toml
│   │   ├── sdkconfig
│   │   └── README.md
│   └── 02-comandi-rover/
│       └── ... stessa struttura di progetto
├── docs/
│   ├── Riferimento_API_ESP32_FreeRTOS.md
│   ├── Riferimento_API_ESP32_FreeRTOS.pdf
│   └── registro_prova_template.md
└── README.md
```

Il nome interno del progetto e del sorgente rimane `hello_world` / `hello_world_main.c`, ereditato dal progetto originale, per mantenere CMake, firmware e configurazione Wokwi coerenti senza modificare il codice.

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

- [Riferimento API in Markdown](docs/Riferimento_API_ESP32_FreeRTOS.md)
- [Riferimento API in PDF](docs/Riferimento_API_ESP32_FreeRTOS.pdf)
- [Modello per registrare una prova](docs/registro_prova_template.md)

## Come aggiungere il prossimo esercizio

Crea una nuova cartella numerata dentro `esercizi/`, con un progetto ESP-IDF autonomo e un breve README: obiettivo, circuito, procedura, risultati e limiti. Conserva la versione degli esercizi precedenti; non riutilizzare ogni volta lo stesso file cancellando il lavoro precedente.

Prima di fare commit controlla `git status`: non aggiungere password, token, chiavi o parametri Wi-Fi privati nel codice o in `sdkconfig`. Il file `sdkconfig` è qui versionato per conservare la configurazione degli esercizi.

## Provenienza

Archivio iniziale creato l'8 novembre 2026 a partire dai file locali di `p00-hello`. Sono state conservate le configurazioni CMake e Wokwi e i sorgenti effettivamente disponibili. Gli esperimenti precedenti senza una versione salvata non sono stati ricostruiti artificialmente.
