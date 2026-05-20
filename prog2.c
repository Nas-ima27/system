#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_NOM 100

// Structure de la liste chainee
typedef struct Node {
    char programme[MAX_NOM];
    struct Node *suivant;
} Node;

// Ajouter un programme a la liste
Node* ajouter(Node *tete, char nom[]) {

    Node *nouveau = (Node*) malloc(sizeof(Node));

    if (nouveau == NULL) {
        printf("Erreur allocation memoire\n");
        exit(1);
    }

    strcpy(nouveau->programme, nom);
    nouveau->suivant = NULL;

    // Si liste vide
    if (tete == NULL) {
        return nouveau;
    }

    // Aller a la fin
    Node *temp = tete;

    while (temp->suivant != NULL) {
        temp = temp->suivant;
    }

    temp->suivant = nouveau;

    return tete;
}

// Afficher la liste
void afficher(Node *tete) {

    Node *temp = tete;

    printf("\nListe des applications :\n");

    while (temp != NULL) {
        printf("- %s\n", temp->programme);
        temp = temp->suivant;
    }
}

// Executer les programmes
void executer(Node *tete) {

    Node *temp = tete;

    while (temp != NULL) {

        pid_t pid = fork();

        if (pid == 0) {

            // Processus fils
            execlp(temp->programme, temp->programme, NULL);

            perror("Erreur execlp");
            exit(1);
        }

        else if (pid < 0) {
            perror("Erreur fork");
        }

        temp = temp->suivant;
    }

    // Attendre tous les fils
    while (wait(NULL) > 0);
}

// Liberer la memoire
void liberer(Node *tete) {

    Node *temp;

    while (tete != NULL) {

        temp = tete;
        tete = tete->suivant;

        free(temp);
    }

    printf("\nMemoire liberee.\n");
}

int main() {

    Node *liste = NULL;

    int n;
    char nom[MAX_NOM];

    printf("Combien d'applications voulez-vous lancer ? ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {

        printf("Nom de l'application %d : ", i + 1);
        scanf("%s", nom);

        liste = ajouter(liste, nom);
    }

    afficher(liste);

    printf("\nExecution des applications...\n");

    executer(liste);

    liberer(liste);

    return 0;
}