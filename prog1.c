#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    // Vérification du nombre d'arguments
    if (argc < 3) {
        printf("Usage : %s <nombre_copies> <prog1> <prog2> ...\n", argv[0]);
        return 1;
    }

    // Nombre de copies
    int n = atoi(argv[1]);

    // Parcourir les programmes
    for (int i = 2; i < argc; i++) {

        printf("Programme : %s\n", argv[i]);

        // Créer n copies
        for (int j = 0; j < n; j++) {

            pid_t pid = fork();

            if (pid == 0) {
                // Processus fils
                execlp(argv[i], argv[i], NULL);

                // Si execlp échoue
                perror("Erreur execlp");
                exit(1);
            }

            else if (pid < 0) {
                perror("Erreur fork");
                exit(1);
            }
        }
    }

    // Attendre tous les fils
    while (wait(NULL) > 0);

    return 0;
}