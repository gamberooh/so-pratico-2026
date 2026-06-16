#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define MAXPATH 1024
#define BUFSIZE 4096

int create_hash(char* filepath, char* target) {
	unsigned char hash[EVP_MAX_MD_SIZE];
	int hashlen;
	EVP_MD_CTX *context = EVP_MD_CTX_new();

	const EVP_MD *md = EVP_sha1();

	printf("init digest\n");
	EVP_DigestInit_ex(context, md, NULL);

	int fd = open(filepath, O_RDONLY);
	if (fd < 0) {
		EVP_MD_CTX_free(context);
		return -1;
	}
	char buf[BUFSIZE];
	size_t bytes;

	printf("writing digest...\n");
	while((bytes = read(fd, buf, sizeof(buf)))> 0) {
		EVP_DigestUpdate(context, buf, bytes);
	}
	printf("end writing digest\n");

	EVP_DigestFinal_ex(context, hash, &hashlen);
	close(fd);

	for(int i = 0; i < EVP_MAX_MD_SIZE; i++) {
		sprintf(target + (i*2), "%02x", hash[i]);
	}

	target[40] = '\0';
	printf("end digest\n");
	return 0;
}


void sha1index(char* pathname) {
	
	printf("creating dir .sha\n");
	DIR *sha1 = opendir(".sha1");
	if (sha1 == NULL) {
		char sha1path[MAXPATH];
		strncpy(sha1path, pathname, MAXPATH);
		strncat(sha1path, "/", MAXPATH - strlen(sha1path) - 1);
		strncat(sha1path, ".sha1", MAXPATH - strlen(sha1path) - 1);
		mkdir(sha1path, 0755);
	}
	closedir(sha1);

	printf(".sha1 created!\n");

	DIR *dir = opendir(pathname);
	if (dir == NULL) return;

	struct dirent* dent = readdir(dir);
	
	char full_path[MAXPATH];
	char link_path[MAXPATH];
	char hash[41];
	while (dent != NULL) {
		if(
			strcmp(".", dent->d_name) == 0 ||	
			strcmp("..", dent->d_name) == 0 ||
			strcmp(".sha1", dent->d_name) == 0
				) {
			dent = readdir(dir);
			continue;
		}
		strncpy(full_path, pathname, MAXPATH);
		strncat(full_path, "/", MAXPATH - strlen(full_path) - 1);
		strncat(full_path, dent->d_name, MAXPATH - strlen(full_path) - 1);
		
		struct stat st;
		

		printf("making symlink...\n");
		if(stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
			char abs_path[MAXPATH];
			if (realpath(full_path, abs_path) == NULL) {
				perror("Error: realpath");
				return;
			}

			if (create_hash(full_path, hash) == 0) {
				
				strncpy(link_path, pathname, MAXPATH);
				strncat(link_path, "/.sha1/", MAXPATH - strlen(link_path) -1);
				strncat(link_path, hash, MAXPATH - strlen(link_path) -1);
				if (symlink(abs_path, link_path) == -1)
					if (errno != EEXIST) perror("Errore symlink");
			}

		}
		dent = readdir(dir);
		printf("symlink created\n");
	}
	closedir(dir);
}

int main(int argc, char* argv) {
	switch (argc) {
		case 1: {		
			printf("avvio caso 1...\n");
			sha1index(".");
			printf("fine caso 1\n");
			break;
		}

		case 2:{

			printf("avvio caso 2...\n");
			sha1index(&argv[1]);
			printf("fine caso 2\n");
			break;
		}
		default: {
			printf("usage: ./%s ", argv[0]);
			return -1;
		}

	}
	return 0;
}
