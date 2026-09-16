#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define MAXPATH 1024

void explore_tree(const char *current_dir, const struct stat *target_stat) {
    DIR *dir = opendir(current_dir);
    if (!dir) {
        return;
    }

    struct dirent *dent;
    char path[MAXPATH];

    while ((dent = readdir(dir)) != NULL) {
        /* Salta i link alle directory corrente (.) e genitore (..) */
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
        } 
        /* Se è un file regolare, verifica i requisiti */
        else if (S_ISREG(sb.st_mode)) {
            /* 1. Stessa ampiezza in byte
             * 2. Non è un hard link (inode diverso OPPURE filesystem/device diverso)
             */
            if (sb.st_size == target_stat->st_size &&
               (sb.st_ino != target_stat->st_ino || sb.st_dev != target_stat->st_dev)) {
                printf("%s\n", path);
            }
        }
    }

    closedir(dir);
}

void samecont(const char *filename, const char *dirname) {
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

    explore_tree(dirname, &target_stat);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
        fprintf(stderr, "Uso: %s <file> <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }
    samecont(argv[1], argv[2]);

    return EXIT_SUCCESS;
}