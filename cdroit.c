#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    struct stat buffer;
    mode_t bit = 0;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <fichier> <user|group|other> <lecture|ecriture|execution|rien>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (stat(argv[1], &buffer) == -1) { perror("stat"); return EXIT_FAILURE; }

    mode_t mode = buffer.st_mode;

    /* Cas "rien" : on enleve tout pour le type demande */
    if (strcmp(argv[3], "rien") == 0) {
        if      (strcmp(argv[2], "user")  == 0) mode &= ~(S_IRUSR | S_IWUSR | S_IXUSR);
        else if (strcmp(argv[2], "group") == 0) mode &= ~(S_IRGRP | S_IWGRP | S_IXGRP);
        else if (strcmp(argv[2], "other") == 0) mode &= ~(S_IROTH | S_IWOTH | S_IXOTH);
        else { fprintf(stderr, "type invalide (user, group ou other)\n"); return EXIT_FAILURE; }
    }
    /* Sinon on AJOUTE un droit avec OR */
    else {
        if (strcmp(argv[2], "user") == 0) {
            if      (strcmp(argv[3], "lecture")   == 0) bit = S_IRUSR;
            else if (strcmp(argv[3], "ecriture")  == 0) bit = S_IWUSR;
            else if (strcmp(argv[3], "execution") == 0) bit = S_IXUSR;
        }
        else if (strcmp(argv[2], "group") == 0) {
            if      (strcmp(argv[3], "lecture")   == 0) bit = S_IRGRP;
            else if (strcmp(argv[3], "ecriture")  == 0) bit = S_IWGRP;
            else if (strcmp(argv[3], "execution") == 0) bit = S_IXGRP;
        }
        else if (strcmp(argv[2], "other") == 0) {
            if      (strcmp(argv[3], "lecture")   == 0) bit = S_IROTH;
            else if (strcmp(argv[3], "ecriture")  == 0) bit = S_IWOTH;
            else if (strcmp(argv[3], "execution") == 0) bit = S_IXOTH;
        }

        if (bit == 0) { fprintf(stderr, "argument invalide\n"); return EXIT_FAILURE; }
        mode |= bit;
    }

    if (chmod(argv[1], mode) == -1) { perror("chmod"); return EXIT_FAILURE; }

    printf("Droits modifies sur %s (nouveau mode: %o)\n", argv[1], mode & 0777);
    return EXIT_SUCCESS;
}
