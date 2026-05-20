#include <stdio.h>
#include <unistd.h>

int main() {
    char nom[50];

    printf("Entrer le nom du dossier a supprimer : ");
    scanf("%s", nom);

    if (rmdir(nom) == 0) {
        printf("Dossier supprime avec succes.\n");
    } else {
        printf("Erreur : dossier non vide ou inexistant.\n");
    }

    return 0;
}