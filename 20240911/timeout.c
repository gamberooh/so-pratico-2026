#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/pidfd.h>
#include <sys/timerfd.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/syscall.h>

/*
    Scrivere un programma timeout che esegua un programma e lo termini se supera una durata
    massima prefissata. timeout ha almeno due argomenti: il primo è la durata massima in millisecondi, i
    parametri dal secondo in poi sono il programma da lanciare coi rispettivi argomenti.
    Es:
    timeout 5000 sleep 2
    temina in due secondi (sleep termina in tempo).
    timeout 3000 sleep 5
    passati tre secondi il programma sleep viene terminato.
    Tmeout deve essere scritto usando le system call poll, pidfd_open, timerfd*.
*/

void mytimeout(time_t timeout, char* program[]) {
    pid_t child = fork();
    
    if (child < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child == 0) {
        execvp(program[0], program);
        perror("execvp");
        exit(EXIT_FAILURE);        
    }
    
    int pfd = pidfd_open(child, 0);
    if (pfd < 0) {
        perror("pidfd_open");
        exit(EXIT_FAILURE);
    }

    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
    if (tfd < 0) {
        perror("timerfd_create");
        close(pfd);
        exit(EXIT_FAILURE);
    }

    struct itimerspec timeout_spec = {
        .it_interval = { 0, 0 }, // Esecuzione singola (non periodico)
        .it_value = {
            .tv_sec = timeout / 1000,
            .tv_nsec = (timeout % 1000) * 1000000L
        }
    };

    if (timerfd_settime(tfd, 0, &timeout_spec, NULL)) {
        perror("timerfd_settime");
        close(pfd);
        close(tfd);
        exit(EXIT_FAILURE);
    }

    struct pollfd fds[2];
    fds[0].fd = pfd;
    fds[0].events = POLLIN;
    fds[1].fd = tfd;
    fds[1].events = POLLIN;

    int pollRet = poll(fds, 2, -1); //file desc, num fds, -1 mode: when en event occurs
    if (pollRet < 0) {
        perror("poll");
    } else if (fds[0].revents & POLLIN) {
        printf("[timeout] program terminated successfully\n");
    } else if (fds[1].revents & POLLIN) {
        pidfd_send_signal(pfd, SIGKILL, NULL, 0);
        printf("[timeout] timer blocked the program execution\n");
    }

    waitpid(child, NULL, 0);
    close(pfd);
    close(tfd);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("usage: %s <ms> <program>\n", argv[0]);
        return EXIT_FAILURE;
    }
    long ms = atol(argv[1]);
    
    if (ms <= 0) {
        printf("error: timeout must be positive\n");
        return EXIT_FAILURE;
    }

    mytimeout(ms, &argv[2]);
    return EXIT_SUCCESS;
}