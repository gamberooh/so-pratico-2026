#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#define MAXFILEPATH 1024 

void print_files(char* dirpath, time_t mtime) {
	DIR *dir = opendir(dirpath);
	printf("Same mtime:\n");
	if(!dir) return;

	struct dirent* dent = readdir(dir);
	while(dent != NULL) {

		if (strcmp(".", dent->d_name) == 0 || strcmp("..", dent->d_name) == 0) {
			dent = readdir(dir);
			continue;
		}

		char full_path[MAXFILEPATH];
		strncpy(full_path, dirpath, MAXFILEPATH);
		strncat(full_path, "/", MAXFILEPATH - strlen(full_path) - 1);
		strncat(full_path, dent->d_name, MAXFILEPATH - strlen(full_path) - 1);
		
		struct stat ent_st;	
		stat(full_path, &ent_st);
		
		if (S_ISREG(ent_st.st_mode) && (mtime == ent_st.st_mtime)) {
			printf("%s\n", full_path);
		}
		dent = readdir(dir);		
	}
	closedir(dir);
}

int main(int argc, char* argv[]) {
	switch (argc) {
		case 2: {
			struct stat st;
			stat(argv[1], &st);
			printf("Case: one file\n");
			print_files(".", st.st_mtime);
			break;
		}
		case 3: { 
			struct stat st1, st2;			
			stat(argv[1], &st1);
			stat(argv[2], &st2);

			if(S_ISDIR(st2.st_mode)) {
				printf("Case: two files\n");
				print_files(argv[2], st1.st_mtime);
			} else if (S_ISREG(st2.st_mode)){
				if (st1.st_mtime == st2.st_mtime) {	
					printf("Same mtime\n");
				}
			} else {
				perror("Non ammesso");
			}
			break;
		}
		default: {
			printf("Usage: ./%s [file] opt: [file/dir]", argv[0]);
			break;
		}
	}
}
