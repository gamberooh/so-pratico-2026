#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#define MAXPATH 1024 
#define MAXEXEC 128
/*
 *Scrivere un programma search_name che deve cercare nel sottoalbero della directory corrente tutti i
file eseguibili con un nome file specifico passato come primo e unico parametro indicando per ogni
file il tipo di eseguibile (script o eseguibile binario).
Ad esempio il comando:
./search_name testprog
deve cercare i file eseguibili chiamati testprog nell'albero della directory corrente. Poniamo
siano ./testprog, ./dir1/testprog, ./dir/dir3/testprog, search_name deve stampare:
./testprog: script
./dir1/testprog: ELF executable
./dir/dir3/testprog: ELF executable
 * */

const char* get_filetype(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        return "unknown";
    }

    unsigned char magic[4];
    size_t n = fread(magic, 1, 4, f);
    fclose(f);

    if (n >= 4 && magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F') {
        return "ELF executable";
    }
    if (n >= 2 && magic[0] == '#' && magic[1] == '!') {
        return "script";
    }
    return "unknown";
}

void explore_tree(char* dirname, char* exe_name) {
    DIR* dir = opendir(dirname);
    if (!dir) {
        return;
    }
    struct dirent* dent = NULL;
    char* filename[MAXEXEC];
    const char* filetype[MAXEXEC];

    int i = 0;
    while((dent = readdir(dir)) != NULL && i < MAXEXEC) {
        if (
            strcmp(dent->d_name, ".") == 0
             ||
            strcmp(dent->d_name, "..") == 0
        ) continue;
        
        char path[MAXPATH];
        int len = snprintf(path, sizeof(path), "%s/%s", dirname, dent->d_name);
        if (len < 0 || len >= (int)sizeof(path)) {
            fprintf(stderr, "Path troppo lungo, ignorato: %s/%s\n", dirname, dent->d_name);
            continue;
        }

        struct stat st;
        if(stat(path, &st) != 0) {
            continue;
        }

        if(S_ISREG(st.st_mode)) {
            // Controlla se il nome corrisponde ed è eseguibile
            if (strcmp(dent->d_name, exe_name) != 0) {
                continue;
            }            
            if (!(st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
                continue;
            }

            // match con l'eseguibile: salviamo il percorso completo e il tipo
            filename[i] = strdup(path);
            filetype[i] = get_filetype(path);
            i++;

        } else if(S_ISDIR(st.st_mode)) {
            explore_tree(path, exe_name);
        }
    }

    for (int j = 0; j < i; j++) {
        printf("%s: %s\n", filename[j], filetype[j]);
        free(filename[j]);
    }

    closedir(dir);
} 

void searchname(char* exe_name) {
    explore_tree(".", exe_name);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s <exec_name>\n", argv[0]);
        return EXIT_FAILURE;
    }
    searchname(argv[1]);
}