#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

/* Données à sauvegarder */
static int donnees[] = {10, 20, 30, 40, 50};
static int nb_donnees = 5;

/* Gestionnaire du signal SIGINT (Ctrl-C) */
void gestionnaire_sigint(int sig) {
    (void)sig; /* éviter warning unused */

    printf("\n[SIGINT reçu] Sauvegarde des données dans 'sauvegarde.txt'...\n");

    /* O_WRONLY | O_CREAT | O_TRUNC via fopen "w" :
       - crée le fichier s'il n'existe pas
       - le remplace (tronque) s'il existe déjà               */
    FILE *f = fopen("sauvegarde.txt", "w");
    if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nb_donnees; i++) {
        fprintf(f, "%d\n", donnees[i]);
    }

    fclose(f);
    printf("[OK] Données sauvegardées. Fin du programme.\n");
    exit(EXIT_SUCCESS);
}

int main(void) {
    /* Installer le gestionnaire pour SIGINT */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = gestionnaire_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Programme en cours d'exécution. Appuyez sur Ctrl-C pour sauvegarder.\n");

    /* Boucle principale */
    while (1) {
        printf("Travail en cours...\n");
        sleep(2);
    }

    return EXIT_SUCCESS;
}
