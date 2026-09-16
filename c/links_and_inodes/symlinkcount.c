#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define MAXPATH 1024

// Scrivere un programma che presi come parametri i pathname di
// un file f e di una directory d stampi l'elenco dei link simbolici che puntano a f presenti nel sottoalbero
// del file system generato dalla directory d.

void explore_tree(const char *current_dir, const struct stat *target_stat) {

    DIR *dir = opendir(current_dir);
    if (!dir) {
        return;
    }

    struct dirent *dent;
    char path[MAXPATH];

    while ((dent = readdir(dir)) != NULL) {

        if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0) {
            continue;
        }

        /* Costruzione sicura del percorso completo */
        int len = snprintf(path, sizeof(path), "%s/%s", current_dir, dent->d_name);
        if (len < 0 || len >= (int)sizeof(path)) {
            fprintf(stderr, "Path troppo lungo, ignorato: %s/%s\n", current_dir, dent->d_name);
            continue;
        }

        struct stat sb;
        if (lstat(path, &sb) != 0) {
            continue; /* Ignora file non accessibili */
        }

        /* Se è una sottodirectory, scendi ricorsivamente */
        if (S_ISDIR(sb.st_mode)) {
            explore_tree(path, target_stat);
        } else if (S_ISLNK(sb.st_mode)) {
            struct stat original_stat;
            if (stat(path, &original_stat) == 0) {
                if (
                    (original_stat.st_ino == target_stat->st_ino)
                     &&
                    (original_stat.st_dev == target_stat->st_dev)
                )   printf("%s\n", path);
                    
            } else {
                perror("stat");
            }
        } 
        
    }

    closedir(dir);
}

void symlinkcount(const char *filename, const char *dirname) {
    struct stat target_stat;

    /* Recupera i metadati del file f */
    if (stat(filename, &target_stat) != 0) {
        perror("Errore stat sul file sorgente");
        exit(EXIT_FAILURE);
    }

    /* Verifica che f sia un file regolare */
    if (!S_ISREG(target_stat.st_mode)) {
        fprintf(stderr, "Errore: '%s' non e' un file regolare.\n", filename);
        exit(EXIT_FAILURE);
    }

    if (target_stat.st_nlink > 0)
        explore_tree(dirname, &target_stat);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
        fprintf(stderr, "Uso: %s <file> <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }
    symlinkcount(argv[1], argv[2]);

    return EXIT_SUCCESS;
}
