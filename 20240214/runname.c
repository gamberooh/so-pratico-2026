#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#define MAXPATH 1024 
#define MAXEXEC 128

/*
Il programma run_name deve cercare nel sottoalbero della directory corrente tutti i
file eseguibili con un nome file specifico (primo parametro di run_name)
e li deve mettere in esecuzione uno dopo l'altro passando i successivi parametri.
Ad esempio il comando:
./run_name testprog a b c
deve cercare i file eseguibili chiamati testprog nell'albero della directory corrente.
Poniamo siano ./testprog, ./dir1/testprog, ./dir/dir3/testprog, run_name deve eseguire
testprog a b c
per 3 volte. Nella prima esecuzione la working directory deve essere la dir corrente '.', la seconda
deve avere come working directory './dir1' e la terza './dir2/dir3'.
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
void explore_tree(char* dirname, char* exe_name, char** params, int prog_len) {
    DIR* dir = opendir(dirname);
    if (!dir) {
        return;
    }
    struct dirent* dent = NULL;

    while((dent = readdir(dir)) != NULL) {
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

            // Trovato l'eseguibile: lo eseguiamo
            pid_t child = fork();
            
            switch (child) {
            case -1:
                perror("fork");
                closedir(dir);
                exit(EXIT_FAILURE);
            case 0: // Processo figlio
                // Cambia la working directory alla cartella che contiene l'eseguibile
                if (chdir(dirname) != 0) {
                    perror("chdir");
                    exit(EXIT_FAILURE);
                }
                
                params[prog_len] = NULL;
                // Esegue il programma usando il nome del file (es. "testprog")
                execvp(dent->d_name, params);
                perror("execvp");
                exit(EXIT_FAILURE);                
            default: // Processo padre
                {
                    int status;
                    waitpid(child, &status, 0);
                    // IMPORTANTE: Nessun exit qui, il padre deve continuare la ricerca!
                }
                break;
            }

        } else if(S_ISDIR(st.st_mode)) {
            explore_tree(path, exe_name, params, prog_len);
        }
    }

    closedir(dir);
}

void runname(char* exe_name, char** params, int prog_len) {
    explore_tree(".", exe_name, params, prog_len);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s <exec_name> <params> \n", argv[0]);
        return EXIT_FAILURE;
    }
    int prog_len = argc -2;
    runname(argv[1], &argv[2], prog_len);
    return EXIT_SUCCESS;
}