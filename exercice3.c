#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Schéma de communication :
 *
 *   PÈRE ──(p1[1] écriture)──► p1 ──(p1[0] lecture)──► FILS
 *   PÈRE ◄─(p2[0] lecture)──── p2 ◄─(p2[1] écriture)── FILS
 */

#define NB_ENTIERS 5

int main(void) {
    int p1[2]; /* père → fils */
    int p2[2]; /* fils → père */

    /* Création des deux tubes */
    if (pipe(p1) == -1) { perror("pipe p1"); exit(EXIT_FAILURE); }
    if (pipe(p2) == -1) { perror("pipe p2"); exit(EXIT_FAILURE); }

    pid_t pid = fork();
    if (pid == -1) { perror("fork"); exit(EXIT_FAILURE); }

    /* ───────────────── PROCESSUS FILS ───────────────── */
    if (pid == 0) {
        /* Fermer les extrémités inutilisées */
        close(p1[1]); /* le fils ne écrit pas dans p1 */
        close(p2[0]); /* le fils ne lit pas dans p2   */

        int val, double_val;
        for (int i = 0; i < NB_ENTIERS; i++) {
            /* Lire un entier envoyé par le père via p1 */
            if (read(p1[0], &val, sizeof(int)) != sizeof(int)) {
                perror("fils: read p1");
                exit(EXIT_FAILURE);
            }
            printf("[Fils]  reçu : %d\n", val);
            fflush(stdout);

            /* Calculer le double et le renvoyer via p2 */
            double_val = val * 2;
            if (write(p2[1], &double_val, sizeof(int)) != sizeof(int)) {
                perror("fils: write p2");
                exit(EXIT_FAILURE);
            }
        }

        close(p1[0]);
        close(p2[1]);
        exit(EXIT_SUCCESS);
    }

    /* ───────────────── PROCESSUS PÈRE ───────────────── */
    /* Fermer les extrémités inutilisées */
    close(p1[0]); /* le père ne lit pas dans p1  */
    close(p2[1]); /* le père n'écrit pas dans p2 */

    int entiers[NB_ENTIERS] = {3, 7, 12, 25, 50};
    int resultat;

    for (int i = 0; i < NB_ENTIERS; i++) {
        /* Envoyer un entier au fils via p1 */
        printf("[Père]  envoi  : %d\n", entiers[i]);
        fflush(stdout);
        if (write(p1[1], &entiers[i], sizeof(int)) != sizeof(int)) {
            perror("père: write p1");
            exit(EXIT_FAILURE);
        }

        /* Lire le double renvoyé par le fils via p2 */
        if (read(p2[0], &resultat, sizeof(int)) != sizeof(int)) {
            perror("père: read p2");
            exit(EXIT_FAILURE);
        }
        printf("[Père]  double : %d\n\n", resultat);
        fflush(stdout);
    }

    close(p1[1]);
    close(p2[0]);

    /* Attendre la fin du fils */
    wait(NULL);
    printf("[Père]  Fils terminé. Fin du programme.\n");

    return EXIT_SUCCESS;
}
