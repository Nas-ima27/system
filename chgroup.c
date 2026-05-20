#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>

int main(int argc, char *argv[]) {
    struct stat buffer;
    struct group *gr;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <fichier> <nouveau_groupe>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Recuperer le GID du nouveau groupe */
    gr = getgrnam(argv[2]);
    if (gr == NULL) { fprintf(stderr, "groupe introuvable: %s\n", argv[2]); return EXIT_FAILURE; }

    /* Recuperer l'UID actuel du fichier (on le garde) */
    if (stat(argv[1], &buffer) == -1) { perror("stat"); return EXIT_FAILURE; }

    /* Changer juste le groupe, garder le meme UID */
    if (chown(argv[1], buffer.st_uid, gr->gr_gid) == -1) {
        perror("chown"); return EXIT_FAILURE;
    }

    printf("Groupe de %s change en %s (GID=%d)\n", argv[1], argv[2], gr->gr_gid);
    return EXIT_SUCCESS;
}
