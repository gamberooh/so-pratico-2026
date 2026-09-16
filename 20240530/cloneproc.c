#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define MAXPATH 1024
/*
    Scrivere un programma cloneproc dato il pid di un processo passato come unico parametro, è in grado di eseguirne una copia.
    (deve rieseguire lo stesso file con lo stresso argv.
    consiglio: cercare in /proc/pid/exe e /proc/pid/cmdline le informazioni necessarie (dove pid è il numero
    di processo.
    scrivere inoltre un semplice programma che ne dimostri il funzionamento.
*/

void cloneproc(char* pid) {
    char exe_path[MAXPATH];
    char cmd_path[MAXPATH];
    char exe_target[MAXPATH];

    snprintf(exe_path, sizeof(exe_path), "/proc/%s/exe", pid);
    snprintf(cmd_path, sizeof(cmd_path), "/proc/%s/cmdline", pid);

    ssize_t len = readlink(exe_path, exe_target, sizeof(exe_target) -1);
    if (len < 0) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    exe_target[len] = '\0';

    int fd = open(cmd_path, O_RDONLY);
    if (fd < 0) {
        perror("open /proc/pid/cmdline");
        exit(EXIT_FAILURE);
    }

    char cmd_buf[4*MAXPATH];
    ssize_t tot_read = read(fd, cmd_buf, sizeof(cmd_buf) -1 );
    close(fd);

    if (tot_read < 0) {
        perror("read cmdline");
        exit(EXIT_FAILURE);
    }

    char* argv[265];
    int argc = 0;

    char* p = cmd_buf;
    while (p < cmd_buf + tot_read && argc < 255) {
        argv[argc++] = p;
        p += strlen(p) + 1; // Salta al prossimo argomento dopo il terminatore \0
    }
    argv[argc] = NULL; // la cella finale va nulla per permettere l'esecuzione con `execvp`

    if (argc == 0) {
        fprintf(stderr, "Errore: nessun argomento trovato in cmdline.\n");
        exit(EXIT_FAILURE);
    }

    printf("Clonando il processo %s...\n", pid);
    printf("Eseguibile: %s\n", exe_target);


    pid_t child = fork();
    if (child < 0) {
        perror("fork");
        return;
    }
    
    if (child == 0){
        execvp(exe_path, argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        printf("Processo clonato avviato con PID figlio: %d\n", pid);
    }

}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s <pid>", argv[0]);
        exit(EXIT_FAILURE);
    }
    cloneproc(argv[1]);
    return EXIT_SUCCESS;
}

