#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

void afficher_info(const char *nom, struct stat *info) {
    printf("\n===== %s =====\n", nom);

    printf("Taille : %ld octets\n", info->st_size);
    printf("Inode : %ld\n", info->st_ino);
    printf("Nombre de liens : %ld\n", info->st_nlink);
    printf("UID : %d\n", info->st_uid);
    printf("GID : %d\n", info->st_gid);

    printf("Permissions : %o\n", info->st_mode & 0777);

    if (S_ISREG(info->st_mode))
        printf("Type : fichier regulier\n");
    else if (S_ISDIR(info->st_mode))
        printf("Type : repertoire\n");
    else if (S_ISLNK(info->st_mode))
        printf("Type : lien symbolique\n");
    else
        printf("Type : autre\n");

    printf("Derniere modification : %s", ctime(&info->st_mtime));
}

int main(int argc, char *argv[]) {

    struct stat info_stat;
    struct stat info_lstat;
    struct stat info_fstat;

    int fd;

    if (argc != 2) {
        printf("Usage : %s fichier\n", argv[0]);
        return 1;
    }

    // 1. stat()
    if (stat(argv[1], &info_stat) == -1) {
        perror("stat");
        return 1;
    }

    // 2. lstat()
    if (lstat(argv[1], &info_lstat) == -1) {
        perror("lstat");
        return 1;
    }

    // 3. open + fstat()
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (fstat(fd, &info_fstat) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    close(fd);

    // Affichage
    afficher_info("STAT", &info_stat);
    afficher_info("LSTAT", &info_lstat);
    afficher_info("FSTAT", &info_fstat);

    return 0;
}