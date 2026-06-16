#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <openssl/evp.h>

#define MAXPATH 1024
#define BUFSIZE 8192

// Calcola SHA1 e scrive l'hash in formato esadecimale nel buffer 'target'
int create_hash(char* filepath, char* target) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashlen;
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    
    EVP_DigestInit_ex(context, EVP_sha1(), NULL);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) { EVP_MD_CTX_free(context); return -1; }

    char buf[BUFSIZE];
    ssize_t bytes;
    while((bytes = read(fd, buf, sizeof(buf))) > 0)
        EVP_DigestUpdate(context, buf, bytes);
    
    EVP_DigestFinal_ex(context, hash, &hashlen);
    close(fd);
    EVP_MD_CTX_free(context);

    for(int i = 0; i < 20; i++) sprintf(target + (i * 2), "%02x", hash[i]);
    target[40] = '\0';
    return 0;
}

// Funzione di indicizzazione
void sha1index(char* pathname) {
    char sha1_dir[MAXPATH];
    snprintf(sha1_dir, sizeof(sha1_dir), "%s/.sha1", pathname);
    mkdir(sha1_dir, 0755);

    DIR *dir = opendir(pathname);
    if (!dir) return;

    struct dirent* dent;
    while ((dent = readdir(dir)) != NULL) {
        if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0 || strcmp(dent->d_name, ".sha1") == 0)
            continue;

        char full_path[MAXPATH], abs_path[MAXPATH], link_path[MAXPATH], hash[41];
        snprintf(full_path, sizeof(full_path), "%s/%s", pathname, dent->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
            if (realpath(full_path, abs_path) && create_hash(abs_path, hash) == 0) {
                snprintf(link_path, sizeof(link_path), "%s/%s", sha1_dir, hash);
                if (symlink(abs_path, link_path) == -1 && errno != EEXIST)
                    perror("Errore symlink");
            }
        }
    }
    closedir(dir);
}

// Funzione di aggiornamento
void sha1update(char* pathname) {
    char sha1_dir[MAXPATH];
    snprintf(sha1_dir, sizeof(sha1_dir), "%s/.sha1", pathname);
    DIR *dir = opendir(sha1_dir);
    if (!dir) return;

    struct dirent* dent;
    while ((dent = readdir(dir)) != NULL) {
        if (dent->d_name[0] == '.') continue;
        char link_path[MAXPATH], hash[41], abs_path[MAXPATH];
        snprintf(link_path, sizeof(link_path), "%s/%s", sha1_dir, dent->d_name);

        struct stat st_link, st_source;
        if (stat(link_path, &st_source) == -1) {
            unlink(link_path); // File sparito
        } else {
            lstat(link_path, &st_link);
            if (st_source.st_mtime > st_link.st_mtime) { // File modificato
                realpath(link_path, abs_path);
                create_hash(abs_path, hash);
                if (strcmp(hash, dent->d_name) != 0) {
                    unlink(link_path);
                    char new_link[MAXPATH];
                    snprintf(new_link, sizeof(new_link), "%s/%s", sha1_dir, hash);
                    symlink(abs_path, new_link);
                }
            }
        }
    }
    closedir(dir);
}

int main(int argc, char* argv[]) {
    char* path = (argc > 1) ? argv[1] : ".";
    printf("Operazione in corso su: %s\n", path);
    sha1index(path);
    sha1update(path);
    return 0;
}