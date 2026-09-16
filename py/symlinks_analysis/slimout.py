# Scrivere un programma Python o uno script bash slinout che elenchi tutti i link simbolici presenti nel
# sottoalbero del file system che ha come radice la directory passata come parametro (o la current
# working directory se slinout viene chiamato senza parametri).
# I link simbolici devono essere suddivisi in interni, che cioè puntano ad altro file o directory nel
# sottoalbero considerato, o esterni, che cioè indicano un file o directory al di fuori del sotoalbero.
# (attenzione: il target dei link simbolici può essere assoluto o relativo)

import os, sys


def slimout(dirname=None):
    if (dirname == None):
        dirname = os.getcwd()
    elif (not (os.path.exists(dirname) and os.path.isdir(dirname))):
        print("error: input must be an existing directory", file=sys.stderr)
        return
    base_dir = os.path.realpath(dirname)
    in_links = []
    out_links = []

    for (root, dirs, files) in os.walk(base_dir):
        for item in dirs + files:
            full_path = os.path.join(root, item)
            if (os.path.islink(full_path)):
                target = os.readlink(full_path)
                try:
                    common = os.path.commonpath([base_dir, target])
                    if common == base_dir :
                        in_links.append((full_path, target))
                    else:
                        out_links.append((full_path, target))
                except ValueError:
                        out_links.append((full_path, target))

    print("In links:\n")

    for link, target in in_links:
        print(f"Link: {link} -> Target: {target}\n")
    print("Out links:\n")

    for link, target in out_links:
        print(f"Link: {link} -> Target: {target}\n")


if __name__ == "__main__":
    argc = len(sys.argv) 
    if (argc > 2) :
        print(f"usage: ${sys.argv[0]} (opt)<dirname> ")
    elif argc == 1:
        slimout()
    else:
        slimout(sys.argv[1])