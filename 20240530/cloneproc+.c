#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define MAXPATH 1024
/*
    Scrivere una estensione del programma dell'esercizio1 cloneproc+ in grado di clonare
    anche la directory corrente e l'ambiente (environment) .
    scrivere inoltre un semplice programma che ne dimostri il funzionamento.
*/

void cloneproc(char* pid) {
    char exe_path[MAXPATH];
    char cmd_path[MAXPATH];
    char curr_dir_path[MAXPATH];
    char env_path[MAXPATH];

    char exe_target[MAXPATH];
    char curr_dir_target[MAXPATH];

    snprintf(exe_path, sizeof(exe_path), "/proc/%s/exe", pid);
    snprintf(cmd_path, sizeof(cmd_path), "/proc/%s/cmdline", pid);
    snprintf(curr_dir_path, sizeof(curr_dir_path), "/proc/%s/cwd", pid);
    snprintf(env_path, sizeof(env_path), "/proc/%s/environ", pid);

    ssize_t exe_len = readlink(exe_path, exe_target, sizeof(exe_target) -1);
    if (exe_len < 0) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    exe_target[exe_len] = '\0';

    ssize_t cwd_len = readlink(exe_path, exe_target, sizeof(exe_target) -1);
    if (cwd_len < 0) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    exe_target[cwd_len] = '\0';

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

    int fd_env = open(cmd_path, O_RDONLY);
    if (fd_env < 0) {
        perror("open /proc/pid/environ");
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

    char env_buf[16 * MAXPATH]; // L'ambiente può essere più grande
    ssize_t tot_read_env = read(fd_env, env_buf, sizeof(env_buf) - 1);
    close(fd_env);

    if (tot_read_env < 0) {
        perror("read environ");
        exit(EXIT_FAILURE);
    }
    env_buf[tot_read_env] = '\0';

    // Ricostruiamo envp (array di stringhe chiave=valore)
    char *envp[256];
    int envc = 0;
    char *e = env_buf;
    while (e < env_buf + tot_read_env && envc < 255) {
        envp[envc++] = e;
        e += strlen(e) + 1;
    }
    envp[envc] = NULL;

    printf("Clonando il processo %s...\n", pid);
    printf("Eseguibile: %s\n", exe_target);
    printf("Direfctory di lavoro: %s\n", curr_dir_target);

    pid_t child = fork();
    if (child < 0) {
        perror("fork");
        return;
    }
    
    if (child == 0){
        execve(exe_target, argv, envp);
        perror("execve");
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

