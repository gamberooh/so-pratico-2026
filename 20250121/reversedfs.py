import os
import sys

def reverse_fs(dir):
    if ( not os.path.exists(dir) or not os.path.isdir(dir)):
        print(f"Errore: '{dir}' non esiste o non è una directory.", file=sys.stderr)
        return
    base_dir = os.path.abspath(dir)
    collected_files = []

    for (root, dirs, files) in os.walk(base_dir):
        rel_root = os.path.relpath(root, base_dir)

        if rel_root == ".":
            depth = 0
        else:
            depth += rel_root.count(os.sep) + 1

        for filename in files:
            full_path = os.path.join(root, filename)
            collected_files.append((depth, filename, full_path))    

    # collected_items[i] = (item[0] -> depth, item[1] -> filename, item[2] -> full_path)
    # in questo modo ordino inversamente sulla chiave item e direttamente sulla chiave filename
    collected_files.sort(key=lambda item: (-item[0], item[1]))
    for (depth, filename, full_path) in collected_files:
        print(full_path + "\n")



if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Uso: {sys.argv[0]} <pattern>")
        sys.exit(1)
    reverse_fs(sys.argv[1])
