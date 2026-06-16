import os
import sys
def convert2ascii(in_path, out_path):
    try:
        with open(in_path, 'r', encoding="utf-8") as fin:
            content = fin.read();
            fin.flush()
            fin.close()
        clean_content = content.encode('ascii', 'replace').decode('ascii')

        with open(out_path, 'w', encoding='ascii') as fout:
            fout.write(clean_content)
            fout.flush()
            fout.close()
        print(f"Conversione completata: {out_path}")
    except FileNotFoundError:
        print(f"{in_path} non esiste.")
    except Exception as e:
        print(f"Si e' verificato un errore {e}")
    
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("usage python3 no8cat.py <filein> <fileout>")
    else:
        convert2ascii(sys.argv[1], sys.argv[2])
