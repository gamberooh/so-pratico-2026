# Scrivere uno script che prende in input da linea di comando il nome di due directory ed elimina (da
# entrambe le directory) tutti i file che hanno la stessa hash sha1. Più precisamente se c'è un file nella
# prima directory e uno nella seconda che hanno la stessa hash sha1 tutti i file che hanno la stessa hash
# sha1 presenti nelle due directory vanno cancellati.
# (Se esistono file con la stessa hash sha1 ma solo in una delle due directory, non sono da cancellare.)

import os, sys
import hashlib
from collections import defaultdict

def hash_file(filepath):
    hasher = hashlib.sha1()
    # lettura a blocchi
    with open (filepath, "rb") as f:
        while(chunk := f.read(65536)):
            hasher.update(chunk)
    return hasher.hexdigest()

def find_and_hash_files(dir):
    base_dir = os.path.realpath(dir)
    hash_map = defaultdict(list)
    for (root, _, files) in os.walk(base_dir):
        for f in files:
            file_path = os.path.join(root, f)
            if (os.path.islink(file_path)):
                continue
            try:
                digest = hash_file(file_path)
                hash_map[digest].append(file_path)
            except (OSError, PermissionError) as e:
                print(f"Attenzione: impossibile leggere {file_path}: {e}", file=sys.stderr)

    return hash_map


def rmsamesha(dir1, dir2):
    if (not os.path.exists(dir1) or not os.path.isdir(dir1)):
        print("error: dir1 invalid argument")

    if (not os.path.exists(dir2) or not os.path.isdir(dir2)):
        print("error: dir2 invalid argument")

    map1 = find_and_hash_files(dir1)
    map2 = find_and_hash_files(dir2)

    common_hashes = set(map1.keys()) & set(map2.keys())

    for digest in common_hashes:
        for file_path in map1[digest] + map2[digest]:
            try:
                os.remove(file_path)
                print(f"rm: {file_path}\n")
            except OSError as e:
                print(f"Errore rimozione {file_path}: {e}", file=sys.stderr)

if __name__ == "__main__":
    if (len(sys.argv) != 3) :
        print(f"usage: {sys.argv[0]} <dir1> <dir2>", file=sys.stderr)
        sys.exit(1)
    if (sys.argv[1] == sys.argv[2]) :
        print(f"cannot use the same dir\nusage: {sys.argv[0]} <dir1> <dir2>", file=sys.stderr)
        sys.exit(1)

    rmsamesha(sys.argv[1], sys.argv[2])
