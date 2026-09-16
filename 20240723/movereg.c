#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#define MAX_PATH 1024
/*
Scrivere un programma che crei nella directory corrente (se non esiste già) una sottodirectory di
nome ... (tre punti).
Tutti i file (regolari) presenti nella directory devono essere spostati nella sottodirectory ... e
ogni file deve essere sostituito nella dir corrente con un link simbolico (relativo, non assoluto) alla
nuova locazione. Usare la system call rename per fare la sostituzione in modo atomico (in nessun
istante il file deve risultare inesistente).
*/

void moveregfiles() {
    const char* dots = "...";
    DIR *dir = opendir(dots);

    if (!dir) {
        int flag = mkdir(dots, 0755);
        if (flag < 0) {
            perror("mkdir");
        } else if (flag > 0) {
            return;
        }
    }
    dir = opendir("."); //cwd
    struct dirent* dent;
    
    char path[MAX_PATH];
    while ((dent = readdir(dir)) != NULL) {
        // evito directory
        if (
            strcmp(dent->d_name, ".") == 0
             ||
            strcmp(dent->d_name, "..") == 0
             ||
            strcmp(dent->d_name, "...") == 0
        ) continue;
        
        /* Costruzione sicura del percorso completo */
        int len = snprintf(path, sizeof(path), "%s/%s", ".", dent->d_name);
        if (len < 0 || len >= (int)sizeof(path)) {
            fprintf(stderr, "Path troppo lungo, ignorato: %s/%s\n", ".", dent->d_name);
            continue;
        }

        struct stat st;
        if(lstat(path, &st) != 0) {
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            // file regolari vengono spostati nella directory dots
            char newPath [MAX_PATH];
            char tmpLink [MAX_PATH];

            snprintf(newPath, sizeof(newPath), "%s/%s", dots, dent->d_name);
            snprintf(tmpLink, sizeof(tmpLink), "tmp_%s", dent->d_name);
            
            if(rename(path, newPath) != 0) {
                perror("rename");
                continue;
            }
            if (symlink(newPath, tmpLink) != 0) {
                perror("symlink");
                continue;
            }
            if (rename(tmpLink, path) != 0) {
                perror("rename symlink");
                unlink(tmpLink);
            }

        }
    }
    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc != 1) {
        printf("usage: %s", argv[0]);
        return EXIT_FAILURE;
    } 
    moveregfiles();
}
