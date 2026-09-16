#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/wait.h>
#include <dirent.h>

#define MAXPATH 1024
#define MAXEVENTS 1024
#define FILENAME 16
#define EVENT_BUF_LEN (MAXEVENTS * (sizeof(struct inotify_event) + FILENAME))

/*
Estendere l'esercizio 1 creando il programma inotimrun che consenta per ogni file in D
l'esecuzione sequenziale di più programmi: I file da inserire in D possono contenere più
comandi separati da una riga vuota.
Es:
/bin/ls
ls
/tmp

/bin/cat
cat
/etc/hostname
*/

void exec_process(char* executable, char** program) {
	pid_t child = fork();

	if (child < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (child == 0) {
		execv(executable, program);
		perror("execv");
		exit(EXIT_FAILURE);        
	} else {
		int st;
		waitpid(child, &st, 0);
	}
}

void process_task_file(char* filepath) {
	int fd = open(filepath, O_RDONLY);
	if (fd < 0) {
		perror("open");
		return;
	}
	
	FILE* file_stream = fdopen(fd, "r");
	if (!file_stream) {
		perror("fdopen");
		close(fd);
		return;
	} 

	char* line = NULL;
	size_t len = 0;
	ssize_t read_bytes;

	int line_number = 0;
	char** args = NULL;

	while ((read_bytes = getline(&line, &len, file_stream)) != -1) {
		if (read_bytes > 0 && line[read_bytes - 1] == '\n') {
			line[read_bytes - 1] = '\0'; 
		}

		// Se la riga è vuota, termina il comando corrente ed eseguilo
		if (line[0] == '\0') {
			if (line_number > 0) {
				char **temp = realloc(args, (line_number + 1) * sizeof(char*));
				if (temp) {
					args = temp;
					args[line_number] = NULL;
					exec_process(args[0], &args[1]);
				}
				for (int i = 0; i < line_number; i++) {
					free(args[i]);
				}
				free(args);
				args = NULL;
				line_number = 0;
			}
			continue;
		}

		char **temp = realloc(args, (line_number + 1) * sizeof(char*));
		if (temp == NULL) {
			perror("realloc");
			break;
		}
		args = temp;
		args[line_number++] = strdup(line);
	}

	// Esegue l'ultimo comando se il file non terminava con una riga vuota
	if (line_number > 0) {
		char **temp = realloc(args, (line_number + 1) * sizeof(char*));
		if (temp) {
			args = temp;
			args[line_number] = NULL;
			exec_process(args[0], &args[1]);
		}
		for (int i = 0; i < line_number; i++) {
			free(args[i]);
		}
		free(args);
	}

	free(line);
	fclose(file_stream);
}

void inotimrun(char* D) {
	DIR* dir = opendir(D);

	if (!dir) {
		if (mkdir(D, 0755) < 0) {
			perror("mkdir");
			exit(EXIT_FAILURE);
		}
	} else {
		closedir(dir);
	}

	// monitoraggio sulla directory D
	int intfy_fd = inotify_init();
	if (intfy_fd < 0) {
		perror("inotify_init");
		exit(EXIT_FAILURE);
	}

	// Rimango in attesa della fine della creazione del file.
	int intfy_watch = inotify_add_watch(intfy_fd, D, IN_CLOSE_WRITE);
	if (intfy_watch < 0) {
		perror("inotify_watch");
		exit(EXIT_FAILURE);
	}

	char buf[EVENT_BUF_LEN] __attribute__ ((aligned(__alignof__(struct inotify_event))));
	int valid = 1;
	while (valid) {
		int len = read(intfy_fd, buf, EVENT_BUF_LEN);
		if (len < 0) {
			perror("read");
			valid = 0;
			break;
		}

		int i = 0;
		while (i < len) {
			struct inotify_event *event = (struct inotify_event *) &buf[i];
			
			// Verifichiamo che ci sia un nome file e che sia un file regolare (non sottodirectory)
			if (event->len && !(event->mask & IN_ISDIR)) {
				char filepath[MAXPATH];
				snprintf(filepath, sizeof(filepath), "%s/%s", D, event->name);

				process_task_file(filepath);
				unlink(filepath);
			}
			i += sizeof(struct inotify_event) + event->len;
		}
	}
	close(intfy_fd);
}

int main(int argc, char* argv[]) {
	if (argc == 2) {
		inotimrun(argv[1]);
	} else {
		printf("usage %s <dirname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}
