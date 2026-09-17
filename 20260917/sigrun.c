#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXTEST 9

void sig_handler(int signum, siginfo_t *info, void *context) {
    if (signum == SIGUSR1) {
        union {
            char str[8]; 
            void* ptr;
        } convert;

        convert.ptr = info->si_value.sival_ptr;

        printf("Server: ricevuto SIGUSR1 dal client PID: %d\n", info->si_pid);
        printf("Server: path ricevuto -> %s\n", convert.str);

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
        } else if (pid == 0) {
            execl(convert.str, convert.str, (char *)NULL);
            perror("execl fallita");
            exit(EXIT_FAILURE);
        }
    }
}

void sigserver() {
    struct sigaction sa;

    sa.sa_sigaction = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Errore sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Server in ascolto... PID: %d\n", getpid());

    while (1) {
        pause();
    }
}

void sigclient(pid_t server_pid, char* path_to_exec) {
    union {
        char str[8];
        void* ptr;
    } convert;

    convert.ptr = 0;
    strncpy(convert.str, path_to_exec, sizeof(convert.str));

    union sigval sv;
    sv.sival_ptr = convert.ptr; 

    if (sigqueue(server_pid, SIGUSR1, sv) == -1) {
        perror("sigqueue");
        exit(EXIT_FAILURE);
    }
    
    printf("Client: path '%s' inviato con successo al server PID %d\n", path_to_exec, server_pid);
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