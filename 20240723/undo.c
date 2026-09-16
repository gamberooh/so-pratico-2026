#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_PATH 1024

void undo() {
    const char* dots = "...";
    DIR *dir = opendir("."); // cwd
    if (!dir) {
        perror("opendir");
        return;
    }
    
    struct dirent* dent;
    while ((dent = readdir(dir)) != NULL) {
        // Ignora '.', '..' e la cartella '...'
        if (strcmp(dent->d_name, ".") == 0 ||
            strcmp(dent->d_name, "..") == 0 ||
            strcmp(dent->d_name, "...") == 0) {
            continue;
        }

        struct stat st;
        if (lstat(dent->d_name, &st) != 0) {
            continue;
        }

        if (S_ISLNK(st.st_mode)) {
            char expected_target[MAX_PATH];
            char actual_target[MAX_PATH];
            
            snprintf(expected_target, sizeof(expected_target), "%s/%s", dots, dent->d_name);

            ssize_t len = readlink(dent->d_name, actual_target, sizeof(actual_target) - 1);
            if (len < 0) {
                perror("readlink");
                continue;
            }
            actual_target[len] = '\0';

            if (strcmp(expected_target, actual_target) == 0) {
                // rename() sovrascrive atomicamente il link con il vero file.
                if (rename(expected_target, dent->d_name) != 0) {
                    perror("rename");
                }
            }
        }
    }
    closedir(dir);

    // Rimuove la cartella ... solo se è vuota
    if (rmdir(dots) != 0) {
        perror("rmdir dots");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        return EXIT_FAILURE;
    } 
    undo();
    return EXIT_SUCCESS;
}