#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXTEST 9

volatile sig_atomic_t done = 0;
int server_exit_status = 0;

void server_sig_handler(int signum, siginfo_t *info, void *context) {
    if (signum == SIGUSR1) {
        union {
            char str[8];
            void* ptr;
        } convert;

        convert.ptr = info->si_value.sival_ptr;
        pid_t client_pid = info->si_pid;

        printf("Server: ricevuto SIGUSR1 dal client PID: %d\n", client_pid);
        printf("Server: esecuzione di -> %s\n", convert.str);

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
        } else if (pid == 0) {
            execl(convert.str, convert.str, (char *)NULL);
            perror("execl fallita");
            exit(EXIT_FAILURE);
        } else {
            int st;
            waitpid(pid, &st, 0);

            int exit_code = 0;
            if (WIFEXITED(st)) {
                exit_code = WEXITSTATUS(st);
                printf("Server: programma terminato con status %d\n", exit_code);
            } else {
                exit_code = -1;
                printf("Server: programma terminato in modo anomalo\n");
            }

            union sigval sv;
            sv.sival_int = exit_code;
            if (sigqueue(client_pid, SIGUSR2, sv) == -1) {
                perror("sigqueue risposta");
            }
        }
    }
}

void client_sig_handler(int signum, siginfo_t *info, void *context) {
    if (signum == SIGUSR2) {
        server_exit_status = info->si_value.sival_int;
        done = 1; 
    }
}

void sigserver() {
    struct sigaction sa;
    sa.sa_sigaction = server_sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Errore sigaction server");
        exit(EXIT_FAILURE);
    }

    printf("Server in ascolto... PID: %d\n", getpid());
    while (1) {
        pause();
    }
}

void sigclient(pid_t server_pid, char* path_to_exec) {
    struct sigaction sa;
    sa.sa_sigaction = client_sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("Errore sigaction client");
        exit(EXIT_FAILURE);
    }

    union {
        char str[8];
        void* ptr;
    } convert;

    convert.ptr = 0;
    strncpy(convert.str, path_to_exec, sizeof(convert.str));

    union sigval sv;
    sv.sival_ptr = convert.ptr;

    if (sigqueue(server_pid, SIGUSR1, sv) == -1) {
        perror("sigqueue invio");
        exit(EXIT_FAILURE);
    }
    
    printf("Client: path inviato. In attesa della terminazione del programma...\n");

    while (!done) {
        pause();
    }

    if (server_exit_status == 0) {
        printf("Client: Successo! Il programma remoto è terminato con status 0.\n");
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stderr, "Client: Errore! Il programma remoto è terminato con status %d.\n", server_exit_status);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso:\n  Server: %s -s\n  Client: %s -c [serverpid] [pathtoexec]\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    } else if (strcmp("-c", argv[1]) == 0){
        pid_t server_pid = (pid_t)atoi(argv[2]);
        char exec_path[MAXTEST];
        
        strncpy(exec_path, argv[3], sizeof(exec_path));
        exec_path[sizeof(exec_path) - 1] = '\0';
        
        sigclient(server_pid, exec_path);

    } else if (strcmp("-s", argv[1]) == 0) {
        sigserver();
    } else {
        printf("Comando non trovato. Flags validi: -s oppure -c [serverpid] [pathtoexec]\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}