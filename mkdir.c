#include <stdio.h>
#include <sys/stat.h>

int main() {
    char nom[50];

    printf("Entrer le nom du dossier a creer : ");
    scanf("%s", nom);

    if (mkdir(nom, 0777) == 0) {
        printf("Dossier cree avec succes.\n");
    } else {
        printf("Erreur lors de la creation du dossier.\n");
    }

    return 0;
}