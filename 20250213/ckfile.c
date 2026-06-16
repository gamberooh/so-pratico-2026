#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXPATH 1024
#define SOFT 1
#define HARD 0

void print_links(char* filename, char* dirname, int flag) {
	struct stat target_st;
	
	if (stat(filename, &target_st) != 0) {
		perror("errore stat");
		return;
	}

	DIR* dir = opendir(dirname);
	if (!dir) return;
	
	struct dirent* dent;

	while ((dent = readdir(dir)) != NULL) {
		if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0) continue;
		
		char path[MAXPATH];
		char source_abs_path[MAXPATH];
		char target_abs_path[MAXPATH];
		snprintf(path, MAXPATH, "%s/%s", dirname, dent->d_name);
		
		realpath(filename, source_abs_path);
		realpath(path, target_abs_path);
		
		struct stat curr_st;
		if (flag == SOFT) {
			if (lstat(path, &curr_st) == 0 && S_ISLNK(curr_st.st_mode))
				printf("symlink: %s\n", path);
			else if (S_ISDIR(curr_st.st_mode))
				print_links(filename, path, flag);

		} else if (flag == HARD){

			if (stat(path, &curr_st) == 0 && S_ISREG(curr_st.st_mode)) {
				if (curr_st.st_ino == target_st.st_ino &&
				    curr_st.st_dev == target_st.st_dev &&
				    strcmp(source_abs_path, target_abs_path) != 0) 
						printf("hlink: %s\n", path);
			} else if (S_ISDIR(curr_st.st_mode))
				print_links(filename, path, flag); 
				
		}			
	}
    closedir(dir);

}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("usage. ./%s [-flag] <file> <dir>", argv[0]);
		return -1;
	} else if (strcmp(argv[1], "-s") == 0)
		print_links(argv[2], argv[3], SOFT);
	else if (strcmp(argv[1], "-l") == 0) 
		print_links(argv[2], argv[3], HARD);
	else
		printf("usare flag: -s o -l");
}
