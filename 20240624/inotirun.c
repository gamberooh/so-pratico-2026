#include <sys/inotify.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#define MAXPATH 1024

#define EVENT_BUF_LEN (1024 * (sizeof(struct inotify_event) + 16))

/*
Usando inotify scrivere un programma inotirun che ha come parametro il pathname di una
directory vuota che chiameremo D. Quando vengono inseriti file in D questi vengono eseguiti 
(uno alla volta) e cancellati. I file in D hanno il seguente formato:
* il pathname dell'eseguibile
* una riga per ogni elemento di argv.
Es:
/bin/ls
ls
-l
/tmp
*/

void inotirun(char* dirname) {
	
	DIR* dir = opendir(dirname);

	if(!dir) {
		if (mkdir(dirname, 0755) < 0) {
			perror("mkdir");
			exit(EXIT_FAILURE);
		}
	} else {
		closedir(dir);
	}

	int intfy_fd = inotify_init();
	if (intfy_fd < 0) {
		perror("inotify_init");
		exit(EXIT_FAILURE);
	}
	int intfy_watch = inotify_add_watch(intfy_fd, dirname, IN_CLOSE_WRITE);
	if (intfy_watch < 0) {
		perror("inotify_watch");
		exit(EXIT_FAILURE);
	}

	char buf[EVENT_BUF_LEN];
	int valid = 1;
	while(valid) {
		int len = read(intfy_fd, buf, EVENT_BUF_LEN);
		if (intfy_watch < 0) {
			perror("read");
			valid = 0;
		}

		int i = 0;
        while (i < len) {
            struct inotify_event *event = (struct inotify_event *) &buf[i];
            
            // Verifichiamo che ci sia un nome file e che sia un file regolare (non sottodirectory)
            if (event->len && !(event->mask & IN_ISDIR)) {
                
                // 1. Costruire il path completo del file creato (dirname + "/" + event->name)
                char filepath[MAXPATH];
                snprintf(filepath, sizeof(filepath), "%s/%s", dirname, event->name);

                // 2. Aprire il file e leggere il contenuto (eseguibile + argomenti)
                // Suggerimento: usa fopen/getline per leggere le righe comodamente
                
                // 3. Effettuare la fork()
                // - Nel figlio: ricostruire l'array argv[] ed eseguire execv()
                // - Nel padre: fare wait() per attendere la fine del figlio
                
                // 4. Cancellare il file con unlink(filepath)
            }
            i += sizeof(struct inotify_event) + event->len;
        }
	}
	close(intfy_fd);

}

int main(int argc, char* argv[]) {
	char dirname[MAXPATH];
	if (strncpy(argv[1], dirname, sizof(dirname) != 0)) {
		perror("string copy");	
	}
	inotirun(dirname);
}

