import sys
import os
import stat

def permdir(dirname):
    if not os.path.exists(dirname) or not os.path.isdir(dirname):
        print(f"error: '{dirname}' non esiste o non è una directory")
        sys.exit(1)
        
    base_dir = os.path.realpath(dirname)
    
    for entry in os.listdir(base_dir):
        filepath = os.path.join(base_dir, entry)
        
        # Consideriamo solo i file regolari (escludiamo directory o link)
        if not os.path.isfile(filepath) or os.path.islink(filepath):
            continue
            
        try:
            st = os.stat(filepath)
            perm_str = stat.filemode(st.st_mode) # Es. "-rwxr-xr-x"
        except OSError:
            continue
            
        if not os.path.exists(perm_str):
            os.makedirs(perm_str, exist_ok=True)
        
        symlink_path = os.path.join(perm_str, entry)
        
        if os.path.lexists(symlink_path): # pulisco eventuali dangling links
            os.remove(symlink_path)
            
        os.symlink(filepath, symlink_path)
        print(f"Creato link: {symlink_path} -> {filepath}")

if __name__ == "__main__":
    if len(sys.argv) == 2:
        permdir(sys.argv[1])
    else:
        print(f"Uso: python3 {sys.argv[0]} <pathname_directory>")
        sys.exit(1)