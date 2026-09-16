#include <stdio.h>
#include <unistd.h>

int main(int argc, char argv[]) {
    printf("---AVVIO TARGET---\n");
    printf("Pid: %d\n", getpid());
    printf("sleep di 60s\n");
    sleep(60);
    printf("---TERMINO TARGET---\n");
    return 0;  
}