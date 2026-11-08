# ESP32 e FreeRTOS - Riferimento delle API studiate

Aggiornato all'8 novembre 2026. Ambiente di riferimento: ESP-IDF 6.1, ESP32-S3, C.

Questo è un promemoria delle API incontrate, non la soluzione dell'esercizio del rover. Gli esempi mostrano firme e singole operazioni: non costituiscono un programma completo.

## 1. Prima distinzione: dove viene eseguita un'istruzione

| Contesto | Significato | Operazioni tipiche |
|---|---|---|
| Inizializzazione | Preparazione prima dell'uso di una risorsa. Spesso dentro `app_main()` o funzioni chiamate da essa. | Configurare GPIO, creare code, installare handler, creare i task permanenti. |
| Task | Esecuzione ordinaria gestita dallo scheduler. Anche `app_main()` viene chiamata da un task. | Elaborare dati, aggiornare uscite, comunicare, attendere tempo o eventi. |
| ISR | Routine chiamata in risposta a un interrupt. Non è un task. | Acquisire informazioni essenziali, segnalare un evento con API adatte al contesto ISR. |
| Terminale del Mac | Strumenti di sviluppo, non codice del microcontrollore. | Attivare ESP-IDF, compilare, preparare il firmware, avviare il laboratorio. |

**“Inizializzazione” e “task” non sono contesti tecnici incompatibili:** spesso l'inizializzazione avviene proprio in un task. La distinzione serve a separare ciò che prepari una volta da ciò che ripeti.

In un firmware reale non occorre mettere tutto in un unico file: `app_main()` può coordinare moduli di configurazione, comunicazione e controllo. Le collocazioni qui indicate sono responsabilità logiche, non nomi di file obbligatori.

## 2. Header e dipendenze

| Header | Perché lo abbiamo usato |
|---|---|
| `<stdio.h>` | `printf()` e `fflush()`. |
| `<stdbool.h>` | `bool`, `true`, `false` nel codice C. |
| `<stdlib.h>` | `malloc()` e `free()`, nominate parlando di heap. Non sono necessarie se il modulo non le usa. |
| `<inttypes.h>` | Tipi interi e macro di formato come `PRIu32`. |
| `"sdkconfig.h"` | Configurazione generata dal progetto, per esempio `CONFIG_IDF_TARGET`. Non modificarlo come un normale file applicativo. |
| `"freertos/FreeRTOS.h"` | Tipi, costanti e configurazione FreeRTOS. Includilo prima di `task.h` e `queue.h`. |
| `"freertos/task.h"` | Task, attese e notifiche. |
| `"freertos/queue.h"` | Code. |
| `"driver/gpio.h"` | Configurazione e uso dei GPIO. |
| `"esp_err.h"` | `esp_err_t`, `ESP_OK`, `ESP_ERROR_CHECK`. |
| `"esp_chip_info.h"` | Informazioni sul chip. |
| `"esp_flash.h"` | Informazioni sulla flash. |
| `"esp_system.h"` | Riavvio e diagnostica di sistema incontrati nell'esempio iniziale. |

Per usare il driver GPIO, il componente applicativo deve dichiarare la dipendenza `esp_driver_gpio` nel proprio `CMakeLists.txt`, per esempio nella lista `PRIV_REQUIRES`. L'header da solo non sostituisce la dipendenza di build.

Non includere tutti gli header per abitudine: conserva quelli necessari al modulo. Non definire manualmente `configTICK_RATE_HZ` per risolvere errori di include.

## 3. Punto di ingresso, tipi e controlli degli errori

### `void app_main(void)`

- **Dove:** punto di ingresso dell'applicazione ESP-IDF.
- **Uso nel firmware:** inizializza risorse, crea i task o svolge direttamente un'attività.
- **Particolarità:** può ritornare; ESP-IDF conclude il task principale. I task creati continuano a esistere.
- **Non generalizzare:** una normale funzione task FreeRTOS non deve semplicemente ritornare.

### `esp_err_t`, `ESP_OK`, `ESP_ERROR_CHECK(...)`

```c
esp_err_t esito = funzione_esp(...);

if (esito != ESP_OK) {
    /* Gestione esplicita dell'errore. */
}

ESP_ERROR_CHECK(funzione_esp(...));
```

- `esp_err_t`: tipo dei codici di errore di molte API Espressif.
- `ESP_OK`: operazione riuscita per queste API.
- `ESP_ERROR_CHECK`: se il risultato non è `ESP_OK`, produce diagnostica e chiama `abort()`; ciò che segue dipende dalla gestione dei panic configurata.
- **Dove:** tipicamente inizializzazione e operazioni dei task. Non trattarlo come un generico gestore di errori per ISR.
- **Uso reale:** utile per prerequisiti indispensabili; errori recuperabili richiedono una politica esplicita, non necessariamente un arresto dell'applicazione.
- **Limite:** configurazione software riuscita non significa cablaggio corretto.

### `BaseType_t`, `pdPASS`, `pdTRUE`, `pdFALSE`

- `BaseType_t`: tipo FreeRTOS usato, tra l'altro, per vari esiti e valori logici.
- `pdPASS`: successo di operazioni FreeRTOS come creazione task, invio e ricezione in coda.
- `pdTRUE` / `pdFALSE`: costanti logiche FreeRTOS; il significato del parametro dipende dalla funzione.
- Non confrontare automaticamente tutte le API con `ESP_OK`: leggi il contratto della funzione.

### `configASSERT(condizione)`

- **Scopo:** verifica un'assunzione quando gli assert sono abilitati.
- **Dove:** nei nostri esempi, dopo la creazione di risorse o dopo operazioni attese come riuscite.
- **Uso reale:** aiuto diagnostico; non sostituisce la gestione degli errori che vuoi mantenere nel prodotto.
- Evita effetti indispensabili dentro la condizione: esegui la chiamata prima, salva l'esito e poi controllalo. La configurazione può disabilitare gli assert.

```c
BaseType_t esito = operazione_freertos(...);
configASSERT(esito == pdPASS);
```

## 4. GPIO: configurazione e uso

### Tabella delle funzioni

| API vista | Parametri / risultato | Collocazione nel firmware |
|---|---|---|
| `gpio_reset_pin(pin)` | Riporta il GPIO a uno stato predefinito. Restituisce `esp_err_t`. | Preparazione o riconfigurazione del pin. Non da ripetere per ogni cambio di livello. |
| `gpio_set_direction(pin, modo)` | Per esempio `GPIO_MODE_INPUT` o `GPIO_MODE_OUTPUT`. Restituisce `esp_err_t`. | Inizializzazione del modulo che possiede il pin. |
| `gpio_set_level(pin, livello)` | Livello `0` o `1`. Restituisce `esp_err_t`. | Task che gestisce l'uscita. Il livello va impostato dopo una configurazione adatta. |
| `gpio_get_level(pin)` | Restituisce il livello digitale `0` o `1`. | Task di acquisizione o controllo quando serve leggere il pin; il percorso di ingresso deve essere abilitato. |
| `gpio_pullup_en(pin)` | Abilita la pull-up interna; restituisce `esp_err_t`. | Configurazione, se richiesta dal circuito e disponibile sul pin. |
| `gpio_pullup_dis(pin)` | Disabilita la pull-up interna. | Configurazione o riconfigurazione. |
| `gpio_pulldown_en(pin)` | Abilita la pull-down interna. | Configurazione, se richiesta dal circuito e disponibile sul pin. |
| `gpio_pulldown_dis(pin)` | Disabilita la pull-down interna. | Configurazione o riconfigurazione. |
| `gpio_config(&configurazione)` | Applica una struttura `gpio_config_t`; restituisce `esp_err_t`. | Inizializzazione coordinata di uno o più pin con le stesse impostazioni. |

La reset/configurazione non va considerata una sequenza obbligatoria identica per ogni uso: scegli un'inizializzazione esplicita e coerente. `gpio_config()` sovrascrive le impostazioni dei GPIO selezionati; non applicarlo distrattamente a un pin già assegnato a un'altra periferica.

### Campi di `gpio_config_t`

| Campo | Significato | Valori incontrati |
|---|---|---|
| `pin_bit_mask` | Selezione dei pin da configurare. | `1ULL << N`; più pin uniti con OR `|`. |
| `mode` | Modalità del pin. | `GPIO_MODE_INPUT`, `GPIO_MODE_OUTPUT`. |
| `pull_up_en` | Impostazione pull-up. | `GPIO_PULLUP_ENABLE`, `GPIO_PULLUP_DISABLE`. |
| `pull_down_en` | Impostazione pull-down. | `GPIO_PULLDOWN_ENABLE`, `GPIO_PULLDOWN_DISABLE`. |
| `intr_type` | Condizione che genera interrupt. | `GPIO_INTR_DISABLE`, `GPIO_INTR_NEGEDGE`. |

### Espressioni C collegate

```c
(1ULL << 5)                 // Seleziona il bit 5.
(1ULL << 4) | (1ULL << 5)   // Seleziona i bit 4 e 5.
```

`ULL` rende la costante `unsigned long long`; nella toolchain usata ha 64 bit. La posizione del bit corrisponde al numero GPIO. Una maschera può selezionare più pin, ma a tutti verrà applicata la stessa configurazione. LED e pulsante richiedono impostazioni distinte.

**Limiti elettrici:** un GPIO non è una misura analogica, non è un'alimentazione per motori e non è intercambiabile con qualsiasi altro pin. Verifica scheda, tensioni, correnti e funzioni riservate prima dei collegamenti reali.

## 5. Interrupt GPIO e comunicazione ISR-task

| API / elemento | Significato | Dove va usato |
|---|---|---|
| `GPIO_INTR_NEGEDGE` | Richiesta di interrupt sul fronte di discesa. | Campo di configurazione GPIO. Nel nostro circuito indica l'inizio della pressione. |
| `gpio_install_isr_service(flags)` | Installa il servizio che gestisce gli handler GPIO. Nell'esempio `flags` valeva `0`. | Inizializzazione, una volta per il servizio condiviso; non dentro ogni iterazione o ISR. |
| `gpio_isr_handler_add(pin, handler, arg)` | Associa un callback al pin e gli passa `arg`. Restituisce `esp_err_t`. | Dopo l'installazione del servizio; destinatari e dati del callback devono già essere validi. |
| `static void handler(void *arg)` | Firma del callback che abbiamo usato. | La funzione verrà eseguita in contesto interrupt, non come task ordinario. |
| `vTaskNotifyGiveFromISR(handle, &risvegliato)` | Incrementa il contatore di notifica del task. Può impostare il flag se serve una decisione di scheduling al ritorno dall'ISR. | Dentro la ISR. La `v` iniziale fa parte del nome corretto. |
| `portYIELD_FROM_ISR()` | Richiede lo scheduling al ritorno dall'interrupt, quando necessario. | Nell'ISR secondo il flag restituito dall'operazione precedente. |

Schema d'uso dei due ultimi elementi:

```c
BaseType_t risvegliato = pdFALSE;
vTaskNotifyGiveFromISR(destinatario, &risvegliato);

if (risvegliato == pdTRUE) {
    portYIELD_FROM_ISR();
}
```

**In una ISR:** lavoro breve, niente attese bloccanti, niente invii di rete o stampe ordinarie. Non usare `vTaskDelay()`, `ulTaskNotifyTake()` o le normali operazioni bloccanti sulle code. Le API `FromISR` esistono proprio per un contesto diverso.

La notifica non è una conseguenza automatica dell'interrupt: è il nostro handler a inviarla. Un interrupt su un pulsante non elimina il rimbalzo dei contatti. Il debouncing resta una responsabilità distinta.

Se cambi i flag di allocazione degli interrupt, verifica i vincoli ulteriori di memoria e funzioni richiamabili: non basta aggiungere attributi alla cieca.

## 6. Creazione, identificazione e fine dei task

### Tipi

| Elemento | Significato |
|---|---|
| `TaskHandle_t` | Handle che identifica un task; non è il codice del task e non è la sua priorità. |
| `void funzione_task(void *arg)` | Firma della funzione che esegue il lavoro del task. |
| `void *arg` | Argomento consegnato alla funzione task; se punta a dati, questi devono restare validi. |

### `xTaskCreate()` e `xTaskCreatePinnedToCore()`

```c
xTaskCreate(funzione, nome, stack, argomento, priorita, &handle);

xTaskCreatePinnedToCore(
    funzione, nome, stack, argomento, priorita, &handle, core
);
```

| Parametro | Significato |
|---|---|
| `funzione` | Funzione task da eseguire. |
| `nome` | Nome diagnostico. |
| `stack` | Dimensione dello stack: **in byte in ESP-IDF**, diversamente da diverse API/documentazioni FreeRTOS generiche. |
| `argomento` | Puntatore passato al task; `NULL` se non necessario. |
| `priorita` | Numero maggiore = priorità maggiore; deve rientrare nei limiti della configurazione. |
| `&handle` | Dove memorizzare l'handle creato; `NULL` se non vuoi conservarlo. |
| `core` | Affinità al core per la versione pinned. Negli esperimenti `0` serviva a rendere visibile lo scheduling su un solo core. |

- **Dove:** inizializzazione, oppure dentro un altro task per lavoro creato a runtime. Non sono le operazioni da chiamare da una ISR.
- **Esito:** `pdPASS` se la creazione riesce; controllarlo.
- **In produzione:** prepara prima risorse, handle e dati necessari. Il nuovo task può iniziare subito, anche prima che il chiamante prosegua; su un altro core può eseguire contemporaneamente.
- Task permanenti possono restare bloccati quando non hanno lavoro. Non serve creare un task per ogni messaggio ricevuto.
- Le priorità degli esercizi e il pinning al core 0 non sono automaticamente la configurazione definitiva del rover.

### `xTaskGetCurrentTaskHandle()`

- **Restituisce:** l'handle del task chiamante.
- **Dove:** nel task che vuole identificarsi, per esempio per passare il proprio handle a chi dovrà notificarlo.
- Non restituisce “il task più importante” o un handle valido per qualsiasi destinatario.

### `vTaskDelete(handle)` / `vTaskDelete(NULL)`

- **Scopo:** elimina un task. `NULL` indica il task chiamante.
- **Dove:** dopo la conclusione definitiva di un'attività, oppure in una gestione di lifecycle progettata esplicitamente.
- **Non usarlo per aspettare:** se arriverà altro lavoro, blocca il task sul meccanismo adatto.
- Non libera automaticamente tutte le risorse applicative possedute dal task. Chiudi connessioni, rilascia memoria e risorse secondo la loro ownership.
- Per un task che si elimina da solo, il recupero di alcune risorse kernel richiede l'esecuzione del task idle.
- Una funzione task ordinaria non deve semplicemente finire con `return`; `app_main()` è l'eccezione gestita da ESP-IDF.

### `taskYIELD()`

- **Scopo:** richiede volontariamente una decisione di scheduling senza mettere il task in attesa di tempo o dati.
- **Dove:** task; non sostituisce `portYIELD_FROM_ISR()`.
- **Limite osservato:** non garantisce CPU ai task meno prioritari. Il chiamante resta pronto; se è il più prioritario può essere scelto ancora.

## 7. Tempo e periodicità

| Elemento / API | Parametri e risultato | Collocazione nel firmware |
|---|---|---|
| `TickType_t` | Tipo usato per contatori e intervalli di tick FreeRTOS. | Variabili temporali dei task. |
| `xTaskGetTickCount()` | Legge il contatore dei tick. | Task: riferimenti temporali, misura di intervalli, istante locale di ricezione. Non usare questa versione ordinaria come sostituto delle API dedicate alle ISR. |
| `pdMS_TO_TICKS(ms)` | Converte millisecondi in tick usando la configurazione. | Dove prepari un intervallo o un timeout espresso in tick. |
| `portTICK_PERIOD_MS` | Periodo del tick espresso in millisecondi tramite macro intera. | Incontrato nell'esempio iniziale; preferire `pdMS_TO_TICKS()` per esprimere le attese. Non presumere precisione arbitraria di questa macro. |
| `configTICK_RATE_HZ` | Frequenza configurata del tick. | Configurazione FreeRTOS, non una variabile da inventare nel codice applicativo. |
| `vTaskDelay(tick)` | Blocca il task per un intervallo relativo alla chiamata. | Task che deve attendere dopo il lavoro. Non ISR. |
| `xTaskDelayUntil(&riferimento, periodo)` | Aggiorna il riferimento alla scadenza successiva e attende se ancora futura. Restituisce se è stata effettuata l'attesa. | Task periodico. Il riferimento va inizializzato prima del ciclo e conservato tra le iterazioni. |
| `portMAX_DELAY` | Valore massimo rappresentabile per un'attesa. Con supporto all'attesa indefinita, nelle API che lo prevedono significa nessun timeout. | Ricezioni o invii bloccanti scelti consapevolmente, non un valore da inserire ovunque. |

Esempi di significato, non programmi completi:

```c
vTaskDelay(pdMS_TO_TICKS(100));
```

Aspetta dopo il lavoro. Il tempo tra inizi di due cicli include lavoro e attesa. La fase del tick e lo scheduling influenzano la durata osservata.

```c
TickType_t riferimento = xTaskGetTickCount();
/* Tra le iterazioni: */
xTaskDelayUntil(&riferimento, periodo);
```

Mantiene scadenze periodiche. Non reinizializzare `riferimento` a ogni giro. Se la scadenza è passata, la chiamata non crea tempo aggiuntivo per recuperare il lavoro in ritardo. Nessuna delle due API garantisce da sola una deadline.

La conversione in tick ha risoluzione e arrotondamento interi: una durata troppo piccola può diventare zero. Verifica che il periodo richiesto sia rappresentabile nella configurazione effettiva.

### Intervalli e timestamp incontrati nelle prove

```c
TickType_t trascorso = ora - istante_precedente;
```

Serve a confrontare intervalli sulla stessa base temporale. Nel caso di contatori unsigned, la sottrazione modulare gestisce un attraversamento del wrap nel campo di intervalli appropriato; non significa poter ignorare qualunque durata o numero di giri del contatore.

Timestamp di ricezione e istante di elaborazione sono concetti distinti. Un comando non deve diventare “nuovo” solo perché viene prelevato più tardi.

## 8. Notifiche task-task

### `xTaskNotifyGive(handle_destinatario)`

- **Dove:** task mittente.
- **Effetto:** incrementa il valore di notifica usato come contatore e può rendere pronto il destinatario in attesa.
- **Non trasporta:** una sequenza di messaggi con contenuti distinti.
- La modalità vista usa l'indice di notifica predefinito, 0. Non mescolare altri usi dello stesso indice senza un contratto chiaro.

### `ulTaskNotifyTake(azzera, attesa_tick)`

- **Dove:** task destinatario. Non riceve un handle perché opera sul task chiamante.
- **Attesa:** se il contatore è zero, si blocca fino a disponibilità o timeout. Se è già maggiore di zero, non aspetta.
- `pdTRUE`: azzera il conteggio disponibile.
- `pdFALSE`: sottrae uno.
- **Restituisce:** conteggio prima della modifica; zero se nessuna notifica è disponibile al termine dell'attesa.

Esempio già ragionato:

| Prima | Parametro | Restituisce | Dopo |
|---:|---|---:|---:|
| 4 | `pdFALSE` | 4 | 3 |
| 4 | `pdTRUE` | 4 | 0 |

**Collocazione reale:** un task può aspettare un evento invece di controllare ripetutamente una variabile. Il mittente può essere un task; se è una ISR usa `vTaskNotifyGiveFromISR()` e il controllo del flag mostrato nella sezione interrupt.

Una notifica già arrivata resta pendente, ma il conteggio è finito. Non è un archivio illimitato né una garanzia che i dati associati siano stati conservati.

## 9. Code: creazione, invio e ricezione

### `QueueHandle_t` e `xQueueCreate(capacita, dimensione_elemento)`

```c
QueueHandle_t coda = xQueueCreate(capacita, sizeof(TipoElemento));
```

- **Dove:** inizializzazione, prima di avviare task che useranno la coda; possibile anche in un task con gestione esplicita del lifecycle.
- **Capacità:** numero massimo di elementi contemporaneamente nella coda, non numero di byte e non numero di task.
- **Dimensione:** byte copiati per ogni elemento. Deve corrispondere al dato inviato e ricevuto.
- **Esito:** handle valido oppure `NULL` se la creazione fallisce.
- I task devono usare la stessa coda e concordare il tipo del messaggio.

**Errore reale incontrato:** una coda creata con `sizeof(int)` non trasferisce una struttura più grande. Il campo non copiato può restare non inizializzato. FreeRTOS non rileva automaticamente l'incompatibilità dei tipi.

### `xQueueSend(coda, &elemento, attesa_tick)`

- **Dove:** task produttore; questa è la versione ordinaria, non quella ISR.
- **Effetto:** copia l'elemento in fondo alla coda, normalmente FIFO.
- Attesa `0`: se piena, fallisce subito.
- Attesa finita: può bloccare il task aspettando spazio, fino al limite indicato.
- `portMAX_DELAY`: può aspettare indefinitamente quando la configurazione lo consente. Valuta se il task può davvero permetterselo.
- **Esito:** `pdPASS` se inserito; altrimenti gestire il mancato invio.
- Modificare successivamente la variabile originale non modifica la copia in coda.

**In produzione:** utile per elementi distinti che devono essere conservati e processati. Una coda piena può bloccare il task di rete o far fallire invii, secondo il timeout scelto. Non presume validità illimitata dei messaggi.

### `xQueueReceive(coda, &destinazione, attesa_tick)`

- **Dove:** task consumatore.
- **Effetto:** copia il primo elemento disponibile nella destinazione e lo rimuove dalla coda.
- **Esito:** `pdPASS` se ricevuto. In caso di insuccesso non usare la destinazione come un nuovo dato valido.
- Attesa `0`: non si blocca se la coda è vuota.
- Attesa positiva: può bloccare il task in attesa di dati.
- La rimozione libera uno spazio **prima** che l'elaborazione applicativa termini.

**Scelta della collocazione:**

| Tipo di task | Ricezione adatta da valutare |
|---|---|
| Lavora solo quando arriva un messaggio | Ricezione bloccante. |
| Deve comunque eseguire controlli periodici | Ricezione senza attesa, oppure altra progettazione che rispetti le scadenze. |

Una coda vuota non significa necessariamente assenza di uno stato valido già conservato dal destinatario.

### `xQueueOverwrite(coda, &elemento)`

- **Dove:** task produttore; non ISR con questa versione.
- **Vincolo:** usarla con una coda lunga **un solo elemento**.
- **Effetto:** aggiorna il dato pendente; se presente, lo sostituisce senza aspettare spazio.
- **Esito previsto con argomenti validi:** `pdPASS`.
- Non sostituisce il dato che un task ha già prelevato e sta elaborando.
- Non scade automaticamente il valore e non aggiunge timestamp.

**In produzione:** possibile casella dell'ultimo aggiornamento non ancora letto. Non è una struttura per conservare ogni evento o per distribuire automaticamente lo stesso messaggio a più destinatari.

### Copia dei dati e puntatori

- Coda di strutture: copia i byte della struttura.
- Struttura che contiene puntatori: vengono copiati i puntatori, non i dati puntati.
- Coda di puntatori: copia gli indirizzi. Validità, ownership e durata degli oggetti restano da progettare.
- Una variabile condivisa è un'alternativa, ma consistenza degli accessi e segnalazione degli aggiornamenti vanno gestite. `volatile` da solo non rende coerente l'aggiornamento di una struttura tra task.

### Confronto delle modalità provate

| Modalità | In caso di coda piena | Conseguenza |
|---|---|---|
| Invio senza attesa | Fallisce il nuovo invio. | I vecchi elementi restano. |
| Invio bloccante | Il produttore aspetta spazio. | Backpressure; il suo ciclo può rallentare. |
| Un elemento con overwrite | Sostituisce il pendente. | Alcuni aggiornamenti intermedi non verranno elaborati. |

## 10. Diagnostica incontrata nell'esempio iniziale

Queste API non sono necessarie tutte in ogni firmware. Le abbiamo viste nel progetto `hello_world` per capire avvio, chip e memoria.

| API / macro | Significato | Collocazione ragionevole |
|---|---|---|
| `printf(formato, ...)` | Stampa formattata; `\n` va a capo. | Diagnostica nei task, con frequenza controllata. Non dentro una ISR ordinaria o come prova di una deadline rispettata. |
| `fflush(stdout)` | Forza lo svuotamento del buffer di uscita. | Task, quando necessario prima di una transizione o per la diagnostica. Non garantisce da solo che un destinatario remoto abbia letto i dati. |
| `esp_chip_info(&informazioni)` | Riempie una struttura `esp_chip_info_t`. | Diagnostica di avvio. |
| `CONFIG_IDF_TARGET` | Nome del target configurato. | Messaggi diagnostici o configurazione del progetto. |
| `CHIP_FEATURE_*` | Flag delle capacità del chip, interrogati con AND bit a bit. | Interpretazione di `esp_chip_info_t.features`. |
| `esp_flash_get_size(NULL, &dimensione)` | Legge la dimensione del chip flash predefinito; controllare l'esito. | Diagnostica di avvio o controllo configurazione. |
| `esp_get_minimum_free_heap_size()` | Minimo heap libero osservato dall'avvio. | Monitoraggio della memoria nei task; non è semplicemente il valore libero attuale. |
| `PRIu32` | Frammento di formato per stampare correttamente un `uint32_t`. | `printf()` quando serve quel tipo. |
| `esp_restart()` | Riavvia il sistema. | Politica esplicita di riavvio in contesto ordinario; non cura automaticamente una causa di errore. |

## 11. Sintassi C utilizzata insieme alle API

### Memoria dinamica nominata nella spiegazione di stack e heap

| Funzione | Significato e collocazione |
|---|---|
| `malloc(byte)` | Richiede memoria dinamica; restituisce un puntatore oppure `NULL`. Usarla in contesto ordinario con ownership esplicita, non come operazione della ISR. Non è necessaria per ogni variabile o messaggio. |
| `free(puntatore)` | Restituisce una precedente allocazione compatibile. Non usare su variabili locali, non liberare due volte e non accedere ai dati dopo il rilascio. |

Le abbiamo soltanto nominate: la gestione dinamica della memoria richiede un approfondimento prima di introdurla nei percorsi critici del rover. Creare task e code con le API dinamiche studiate richiede già memoria, anche se non chiami direttamente `malloc()`.

| Espressione | Significato |
|---|---|
| `&variabile` | Indirizzo della variabile. Permette a una funzione di leggerla o scriverla secondo il proprio contratto. |
| `a & maschera` | AND bit a bit: significato diverso dall'operatore di indirizzo precedente. |
| `a | b` | OR bit a bit, usato per unire selezioni. |
| `1ULL << N` | Spostamento a sinistra di una costante unsigned long long; costruisce il bit selezionato nel campo appropriato. |
| `sizeof(Tipo)` | Dimensione del tipo in byte; include l'eventuale padding di una struttura. |
| `.campo = valore` | Inizializzazione designata di un campo di struttura in C. |
| `struttura.campo` | Accesso a un campo. |
| `condizione ? a : b` | Espressione condizionale: sceglie uno dei due valori. |
| `NULL` | Puntatore nullo; il significato come parametro dipende dall'API. |
| `static` davanti a una funzione | Visibilità limitata al file C. |
| `static` a livello file su un handle | Visibilità limitata al file, durata per tutta l'esecuzione; non è una protezione dagli accessi concorrenti. |
| `Tipo variabile = {0}` | Inizializza i campi ai valori zero appropriati al tipo. Non equivale a ricevere un messaggio valido. |

Una funzione può leggere un dato solo finché quel dato è valido. Passare l'indirizzo di una variabile locale a un task che la userà dopo la fine della funzione è un errore di durata della memoria.

## 12. Dove collocare le responsabilità nel firmware del rover

Questa tabella indica il ruolo delle API, non il codice da scrivere per l'esercizio corrente.

| Responsabilità | API già studiate pertinenti | Vincolo principale |
|---|---|---|
| Preparazione dell'applicazione | `gpio_config`, creazione code/task, installazione ISR. | Preparare le dipendenze prima che i task le usino. |
| Acquisizione periodica | `gpio_get_level`, API temporali, eventuale invio in coda. | Periodo e costo del lavoro devono essere misurati. |
| Ricezione/comunicazione | API di coda o notifica, tempo locale di ricezione. | Validare i dati; non prolungare implicitamente un comando con messaggi inutilizzabili. |
| Controllo periodico | API temporali, ricezione senza attesa, aggiornamento delle uscite. | Non aspettare indefinitamente un nuovo messaggio se devi continuare a controllare scadenze e stato. |
| Segnalazione hardware | Handler GPIO e notifica `FromISR`. | Lavoro minimo e dati destinatari validi. |
| Diagnostica | `printf`, informazioni chip e memoria. | Il logging ha un costo e può alterare i tempi osservati. |
| Terminazione di un lavoro temporaneo | Rilascio delle risorse applicative, poi `vTaskDelete`. | Non lasciare lock o risorse possedute dal task eliminato. |

**Non confondere:**

- Comando trasferito correttamente e comando ancora valido.
- Task pronto e task effettivamente in esecuzione.
- Richiesta software di stop e arresto fisico delle ruote.
- Test in Wokwi e verifica elettrica/meccanica sul rover reale.
- Pulsante di prova e arresto di emergenza hardware: non sono automaticamente equivalenti.

## 13. Comandi di terminale già incontrati

Questi comandi non vanno scritti nel file C e non vengono eseguiti sul microcontrollore.

### Installazione e ambiente

```bash
brew tap espressif/eim
brew trust --formula espressif/eim/eim
brew install eim
eim --version
eim wizard
```

Sono passaggi di installazione, non operazioni da ripetere a ogni compilazione. Il trust è stato limitato alla formula necessaria.

```bash
source ~/.espressif/tools/activate_idf_v6.1.sh
idf.py --version
```

Attivano e verificano l'ambiente nel terminale corrente. Un altro terminale può aver bisogno di una propria attivazione.

### Build del progetto ESP32

```bash
idf.py set-target esp32s3
idf.py build
idf.py merge-bin
```

- `set-target`: seleziona il target; può pulire/riconfigurare build e impostazioni. Non è da ripetere a ogni modifica.
- `build`: costruisce il progetto nella cartella di build scelta.
- `merge-bin`: crea un'immagine unificata con i componenti necessari all'avvio.

Nel progetto attuale usiamo la cartella di build di CLion:

```bash
idf.py -B cmake-build-debug-esp-idf build merge-bin
```

Eseguilo dalla radice del progetto ESP-IDF, dopo aver salvato. Ferma e riavvia Wokwi per osservare l'immagine aggiornata.

I suggerimenti `idf.py flash`, `idf.py -p PORT flash` e `idf.py monitor` riguardano una scheda fisica e la sua porta. Non sono necessari per avviare Wokwi; verifica porta e scheda prima del flash reale.

### File che non sono codice applicativo

| File / impostazione | Funzione |
|---|---|
| `main/CMakeLists.txt` | Sorgenti e dipendenze del componente applicativo. |
| `diagram.json` | Scheda e collegamenti virtuali Wokwi. |
| `wokwi.toml` | Percorsi dell'immagine firmware e dell'ELF letti dal simulatore. |
| `sdkconfig` | Configurazione del progetto ESP-IDF. |
| `-DIDF_TARGET=esp32s3` | Opzione CMake usata nel profilo di CLion. |

### Laboratorio ROS/Webots

Sul Mac, dalla cartella del laboratorio, in terminali distinti:

```bash
docker version
bash start-host.sh
bash start-robot.sh
bash drive.sh
```

- `docker version`: verifica client e servizio Docker.
- `start-host.sh`: server locale che permette al launch ROS di aprire Webots sul Mac.
- `start-robot.sh`: avvia il contenitore e il launch del robot.
- `drive.sh`: nodo ROS per guida da tastiera.

Gli script sono del laboratorio, non API ESP32 e non firmware di produzione.

```bash
docker compose exec ros bash
source /opt/ros/jazzy/setup.bash
ros2 topic list
ros2 topic info /cmd_vel
ros2 topic echo /scan --once --qos-reliability best_effort
ros2 topic echo /odom --once
```

La prima riga viene eseguita sul Mac dalla cartella del laboratorio; le successive nella shell Linux del contenitore.

```bash
docker compose stop
```

Arresta il contenitore del laboratorio. Ferma prima la guida e il launch in modo ordinato.

## 14. Come consultare questo riferimento

Prima di usare una funzione, chiediti:

1. È la versione per task o per ISR?
2. Può bloccare il chiamante? Per quanto tempo?
3. Qual è l'esito e come lo controllo?
4. I dati sono copiati oppure passo un indirizzo?
5. Chi possiede la risorsa e per quanto deve restare valida?
6. È configurazione da eseguire una volta o lavoro da ripetere?

Mutex, semafori, API PWM/LEDC ed encoder/PCNT non sono ancora stati studiati operativamente in questo percorso: non li aggiungiamo come ricette da applicare senza spiegazione.

## Fonti di riferimento

API e firme confrontate con gli header dell'ESP-IDF 6.1 installato, in particolare `driver/gpio.h`, `freertos/task.h`, `freertos/queue.h` e le aggiunte ESP-IDF ai task.

- [ESP-IDF 6.1: GPIO ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32s3/api-reference/peripherals/gpio.html)
- [ESP-IDF 6.1: documentazione ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32s3/)
- [FreeRTOS: documentazione ufficiale](https://www.freertos.org/Documentation/00-Overview)
- [Wokwi: periferiche ESP32 supportate](https://docs.wokwi.com/guides/esp32)
- [Cyberbotics: ponte ed esempi ROS-Webots](https://github.com/cyberbotics/webots_ros2)

Per gli aspetti specifici ESP-IDF, come dimensione stack e multicore, usa la documentazione Espressif della versione installata invece di trasferire automaticamente i dettagli da un corso STM32.
