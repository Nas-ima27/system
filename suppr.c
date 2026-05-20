#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* unlink decremente le compteur de liens du fichier */
    if (unlink(argv[1]) == -1) { perror("unlink"); return EXIT_FAILURE; }

    printf("Fichier %s supprime avec succes\n", argv[1]);
    return EXIT_SUCCESS;
}
