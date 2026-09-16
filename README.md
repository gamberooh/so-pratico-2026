# Sistemi Operativi - Esercizi Pratici

Raccolta e organizzazione degli esercizi pratici d'esame, suddivisi per linguaggio e categorizzati per feature/argomento all'interno delle cartelle `c/` e `py/`.

---

## 📁 Struttura Linguaggio C (`c/`)

| Feature / Cartella | File | Descrizione / Argomento |
| :--- | :--- | :--- |
| **`c/signals_ipc/`** | [`sigtx.c`](c/signals_ipc/sigtx.c)<br>[`sigrx.c`](c/signals_ipc/sigrx.c)<br>[`extsigtx.c`](c/signals_ipc/extsigtx.c)<br>[`extsigrx.c`](c/signals_ipc/extsigrx.c) | **Segnali real-time & IPC**: Invio/ricezione dati con `sigqueue`, gestione payload tramite `sigval_t` (`sival_ptr`), handler con flag `SA_SIGINFO` e logica ACK. |
| **`c/inotify/`** | [`inotirun.c`](c/inotify/inotirun.c) | **Eventi Filesystem**: Monitoraggio di directory con `inotify_init` / `inotify_add_watch` (`IN_CLOSE_WRITE`) ed esecuzione comandi da file inseriti. |
| **`c/processes_timers/`** | [`timeout.c`](c/processes_timers/timeout.c)<br>[`timeout+.c`](c/processes_timers/timeout+.c) | **Controllo Processi & Timer**: Esecuzione di comandi con limite temporale tramite `timerfd_create`, monitoraggio con `pidfd_open` e `poll`, gestione riavvio su crash. |
| **`c/filesystem_traversal/`** | [`movereg.c`](c/filesystem_traversal/movereg.c)<br>[`undo.c`](c/filesystem_traversal/undo.c) | **Navigazione & Spostamento File**: Scansione con `opendir`/`readdir`, spostamento file regolari (`rename`) e relativo rollback/undo. |
| **`c/links_and_inodes/`** | [`symlinkcount.c`](c/links_and_inodes/symlinkcount.c)<br>[`samecount.c`](c/links_and_inodes/samecount.c)<br>[`ckfile.c`](c/links_and_inodes/ckfile.c) | **Hard link, Symlink & Inode**: Ispezione link simbolici (`readlink`, `lstat`), confronto coppie `st_dev` / `st_ino`, conteggio link duri. |
| **`c/hashing_crypto/`** | [`sha1index.c`](c/hashing_crypto/sha1index.c)<br>[`sha1update.c`](c/hashing_crypto/sha1update.c) | **Crittografia & Checksum**: Calcolo hash SHA-1 su file con libreria OpenSSL EVP (`EVP_DigestInit_ex`, `EVP_DigestUpdate`) e indicizzazione su albero. |
| **`c/file_metadata_mtime/`** | [`modif=.c`](c/file_metadata_mtime/modif=.c)<br>[`modifcmp.c`](c/file_metadata_mtime/modifcmp.c) | **Filtri Metadata & Timestamp**: Confronto `st_mtime` tra file e directory per trovare modifiche coincidenti o più recenti. |

---

## 🐍 Struttura Linguaggio Python (`py/`)

| Feature / Cartella | File | Descrizione / Argomento |
| :--- | :--- | :--- |
| **`py/hashing_duplicates/`** | [`rmsamehash.py`](py/hashing_duplicates/rmsamehash.py)<br>[`dremcont.py`](py/hashing_duplicates/dremcont.py) | **Deduplicazione tramite Hash**: Calcolo digest SHA1/SHA256 (`hashlib`) a blocchi e rimozione dei file duplicati tra cartelle. |
| **`py/symlinks_analysis/`** | [`slimout.py`](py/symlinks_analysis/slimout.py) | **Analisi Link Simbolici**: Ispezione tramite `os.readlink` e classificazione in link *interni* ed *esterni* usando `os.path.commonpath`. |
| **`py/filesystem_traversal/`** | [`reversedfs.py`](py/filesystem_traversal/reversedfs.py) | **Esplorazione Gerarchie**: Scansione con `os.walk` e output ricorsivo con ordinamento/profondità invertita. |
| **`py/file_metadata/`** | [`es3.py`](py/file_metadata/es3.py)<br>[`inotab.py`](py/file_metadata/inotab.py) | **Metadata & Inode**: Ricerca file con stessa dimensione (`st_size`) e generazione ordinata della tabella degli inode (`st_ino`). |
| **`py/text_processing/`** | [`ascii.py`](py/text_processing/ascii.py) | **Manipolazione Testo**: Conversione encoding e pulizia dei caratteri non-ASCII con sanitizzazione. |
| **`py/pattern_search/`** | [`lspattern.py`](py/pattern_search/lspattern.py) | **Ricerca Pattern nel Contenuto**: Ricerca testuale nei file di un albero con ordinamento dei risultati per `os.path.getmtime`. |
