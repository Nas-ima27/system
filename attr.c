#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>

int main(int argc, char *argv[]) {
    struct stat buffer;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (stat(argv[1], &buffer) == -1) { perror("stat"); return EXIT_FAILURE; }

    /* Type */
    printf("Type         : ");
    if      (S_ISREG(buffer.st_mode))  printf("fichier ordinaire\n");
    else if (S_ISDIR(buffer.st_mode))  printf("repertoire\n");
    else if (S_ISLNK(buffer.st_mode))  printf("lien symbolique\n");
    else if (S_ISCHR(buffer.st_mode))  printf("fichier special caractere\n");
    else if (S_ISBLK(buffer.st_mode))  printf("fichier special bloc\n");
    else if (S_ISFIFO(buffer.st_mode)) printf("tube nomme (FIFO)\n");
    else if (S_ISSOCK(buffer.st_mode)) printf("socket\n");

    /* Droits rwxrwxrwx */
    printf("Droits       : ");
    printf("%c%c%c", (buffer.st_mode & S_IRUSR)?'r':'-',
                     (buffer.st_mode & S_IWUSR)?'w':'-',
                     (buffer.st_mode & S_IXUSR)?'x':'-');
    printf("%c%c%c", (buffer.st_mode & S_IRGRP)?'r':'-',
                     (buffer.st_mode & S_IWGRP)?'w':'-',
                     (buffer.st_mode & S_IXGRP)?'x':'-');
    printf("%c%c%c\n",(buffer.st_mode & S_IROTH)?'r':'-',
                      (buffer.st_mode & S_IWOTH)?'w':'-',
                      (buffer.st_mode & S_IXOTH)?'x':'-');

    /* Date modification */
    printf("Modifie le   : %s", ctime(&buffer.st_mtime));

    /* Proprietaire */
    printf("Proprietaire : %s\n", getpwuid(buffer.st_uid)->pw_name);

    /* Taille */
    printf("Taille       : %ld octets\n", (long)buffer.st_size);

    /* Liens */
    printf("Liens        : %ld\n", (long)buffer.st_nlink);

    return EXIT_SUCCESS;
}
