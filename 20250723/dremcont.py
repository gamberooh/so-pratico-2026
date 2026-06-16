import os
import sys
import hashlib


def get_hash(filepath, block_size=65536):
    hasher = hashlib.sha256()
    try:
        with open(filepath, 'rb') as f:
            while data := f.read(block_size):
                hasher.update(data)
        return hasher.hexdigest()
    except (OSError, IOError):
        return None


def dremcont(f, d):
    if not os.path.isfile(f):
        print("Input file error", file=sys.stderr)
        return
    target_size = os.path.getsize(f)
    target_digest = get_hash(f)

    for root, dirs, files in os.walk(d):
        for name in files:
            if os.path.abspath(f) != os.path.abspath(name):
                filepath = os.path.join(root, name)
                if target_size == os.path.getsize(filepath):
                    if target_digest == get_hash(filepath):
                        print(f"removing {filepath}...\n")
                        os.remove(filepath)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"usage: python3 {sys.argv[0]} <filename> <dirname>\n", file=sys.stderr)
        sys.exit(1)
    else:
        dremcont(sys.argv[1], sys.argv[2])
