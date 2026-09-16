#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    
    printf("--- TARGET AVANZATO ---\n");
    printf("Mio PID: %d\n", getpid());
    printf("Directory corrente: %s\n", cwd);
    printf("Variabile TEST_VAR = %s\n", getenv("TEST_VAR"));
    
    sleep(20);
    return 0;
}