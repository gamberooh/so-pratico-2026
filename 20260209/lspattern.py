import os
import sys

""" Esercizio 3: Python o bash: 10 punti
Scrivere uno script bash o un programma python che preso come parametro un pattern (stringa
ASCII) fornisca in output l'elenco dei file del sottoalbero che ha come radice la directory corrente che
nel loro contenuto includano il pattern.. La lista di output deve essere ordinata dal file con tempo di
ultima modifica più antico al file con ultima modifica più recente. """

def find_pattern (pattern) :
    # Esplorazione directories
    matched_files = []
    for root, dirs, files in os.walk(os.getcwd()):
        for filename in files:
            path = os.path.join(root, filename)
            try: 
                with open(path, "r", encoding="utf-8", errors="ignore") as f:
                    for line in f:
                            if (pattern in line):
                                matched_files.append(path)
                                break;
            except Exception:
                pass
    matched_files.sort(key=os.path.getmtime)
    
    i = 1
    for file in matched_files:
        print(f"{i}: {file}")
        i+=1

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Uso: {sys.argv[0]} <pattern>")
        sys.exit(1)
    find_pattern(sys.argv[1])
