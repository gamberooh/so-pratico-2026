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
    Estendere l'esercizio 1 per fare in modo che se prima del timeout il programma termina
    con un errore, al termine del timeout il programma venga riattivato.
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

    int terminated = 0;
    while(!terminated) {

        struct pollfd fds[2];
        fds[0].fd = pfd;
        fds[0].events = POLLIN;
        fds[1].fd = tfd;
        fds[1].events = POLLIN;
    
        int pollRet = poll(fds, 2, -1); //file desc, num fds, -1 mode: when en event occurs
        if (pollRet < 0) {
            perror("poll");
        } else if (fds[0].revents & POLLIN) {
            int status;
            waitpid(child, &status, 0);
            close(pfd);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                printf("[timeout] program terminated successfully\n");
                terminated = 1;
                continue;
            }
            
            printf("[timeout] terminato con errore, in attesa del timeout\n");
            
            // struttura poll che aspetta il termine del timer residuo
            struct pollfd timerPoll = { .fd = tfd, .events = POLLIN };
            int timerPollRet = poll(&timerPoll, 1, -1);
            
            printf("[timeout] timeout scaduto: rilancio il programma\n");

        } else if (fds[1].revents & POLLIN) {
            u_int64_t expirations;
            read(tfd, &expirations, sizeof(expirations));

            printf("[timeout] timer blocked the program execution\n");
            kill(child, SIGKILL);
            waitpid(child, NULL, 0);
            close(pfd);
            terminated = 1;            
        }
    }

    close(tfd);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("usage: %s <program>\n ", argv[0]);
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