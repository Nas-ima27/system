#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

int main(int argc, char *argv[]) {
    struct stat buffer;
    struct passwd *pw;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <fichier> <nouveau_user>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Recuperer l'UID du nouveau user */
    pw = getpwnam(argv[2]);
    if (pw == NULL) { fprintf(stderr, "utilisateur introuvable: %s\n", argv[2]); return EXIT_FAILURE; }

    /* Recuperer le GID actuel (on le garde) */
    if (stat(argv[1], &buffer) == -1) { perror("stat"); return EXIT_FAILURE; }

    /* Changer juste le proprietaire, garder le meme GID */
    if (chown(argv[1], pw->pw_uid, buffer.st_gid) == -1) {
        perror("chown"); return EXIT_FAILURE;
    }

    printf("Proprietaire de %s change en %s (UID=%d)\n", argv[1], argv[2], pw->pw_uid);
    return EXIT_SUCCESS;
}
