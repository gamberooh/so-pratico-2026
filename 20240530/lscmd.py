import os
import sys
from collections import defaultdict

def lscmd():
    base = "/proc"
    current_uid = os.getuid()
    
    # Hash map (dizionario) per raggruppare: 
    # Chiave: pathname dell'eseguibile, Valore: lista di PID
    groups = defaultdict(list)

    for dirent in os.listdir(base):
        if not dirent.isdigit():
            continue
        
        pid = dirent
        proc_path = os.path.join(base, pid)
        exe_link = os.path.join(proc_path, "exe")

        try:
            # Filtriamo solo i processi dell'utente corrente
            if os.stat(proc_path).st_uid != current_uid:
                continue
            
            # Leggiamo il link simbolico /proc/<pid>/exe per avere il pathname
            exe_path = os.readlink(exe_link)
            
            # Aggiungiamo il PID alla lista corrispondente nel dizionario
            groups[exe_path].append(pid)
            
        except (PermissionError, FileNotFoundError, OSError):
            # I processi possono terminare mentre li leggiamo, ignoriamo gli errori
            continue

    # Stampiamo in ordine alfabetico dei pathname (e ordiniamo anche i PID numericamente)
    for exe_path in sorted(groups.keys()):
        # Ordiniamo i PID convertendoli in interi per averli in ordine logico
        sorted_pids = sorted(groups[exe_path], key=int)
        
        # Stampiamo nel formato richiesto: pathname e poi i PID spaziati
        print(f"{exe_path} " + " ".join(sorted_pids))

if __name__ == "__main__":
    if len(sys.argv) != 1:
        print(f"usage: {sys.argv[0]}", file=sys.stderr)
        sys.exit(1)

    lscmd()