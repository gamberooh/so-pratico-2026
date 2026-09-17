import os
import sys


def palindromizer(filepath):
  try:
    with open(filepath, "rb") as f:
      content = f.read()

    if len(content) <= 1:
      return

    # Trova il suffisso palindromo più lungo per minimizzare i byte da aggiungere
    n = len(content)
    padding = b""
    for i in range(n):
      sub = content[i:]
      if sub == sub[::-1]:
        prefix = content[:i]
        padding = prefix[::-1]
        break

    # Scrive il contenuto originale più il padding palindromo
    with open(filepath, "wb") as f:
      f.write(content + padding)

    print(f"File palindromizzato: {filepath}")
  except OSError as e:
    print(f"Errore con il file {filepath}: {e}")


def depalindromizer():
  base = os.path.realpath(os.getcwd())
  for root, _, files in os.walk(base):
    for filename in files:
      filepath = os.path.join(root, filename)

      if os.path.abspath(filepath) == os.path.abspath(__file__):
        continue

      try:
        with open(filepath, "rb") as f:
          content = f.read()

        if content and content == content[::-1]:
          original_content = None
          n = len(content)

          for i in range(n // 2, n):
            victim = content[:i]
            for j in range(len(victim)):
              sub = victim[j:]
              if sub == sub[::-1]:
                padding = victim[:j][::-1]
                if victim + padding == content:
                  original_content = victim
                  break
            if original_content:
              break

          if original_content is None:
            original_content = content[: n // 2]

          with open(filepath, "wb") as f:
            f.write(original_content)

          print(f"Depalindromizzato (ripristinato): {filepath}")

      except OSError:
        continue


if __name__ == "__main__":
  if len(sys.argv) < 2:
    print(f"Usage: {sys.argv[0]} -p <filename> (per palindromizzare)")
    print(f"       {sys.argv[0]} -u (per depalindromizzare la directory)")
  elif sys.argv[1] == "-p":
    if len(sys.argv) < 3:
      print("[-] Errore: Specificare un file da palindromizzare.")
    else:
      palindromizer(sys.argv[2])
  elif sys.argv[1] == "-u":
    depalindromizer()
  else:
    print(f"Usage: {sys.argv[0]} -p <filename> | -u")
