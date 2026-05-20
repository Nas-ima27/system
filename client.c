/*
 * ============================================================
 *   CLIENT  –  Chat Walkie-Talkie  (TCP / POSIX Sockets)
 * ============================================================
 * Compilation : gcc -o client client.c
 * Utilisation : ./client <ip_serveur> <port>
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

/* ── Constantes ─────────────────────────────────────────── */
#define TAILLE_MSG   256
#define MOT_QUITTER  "quitter"

/* ── Prototype ───────────────────────────────────────────── */
void chat(int desc_socket);

/* ================================================================
 *  MAIN
 * ================================================================ */
int main(int argc, char *argv[])
{
    /* ── 1. Vérification du nombre d'arguments ─────────────── */
    if (argc != 3) {
        fprintf(stderr, "Usage : %s <ip_serveur> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* ── 2. Initialisation des variables ───────────────────── */
    int    desc;              /* descripteur du socket client */
    int    err;
    struct sockaddr_in adresse_distante;   /* adresse du serveur */

    /* ── 3. Création du socket ─────────────────────────────── */
    desc = socket(AF_INET, SOCK_STREAM, 0);
    if (desc < 0) {
        perror("Erreur : création du socket");
        exit(EXIT_FAILURE);
    }
    printf("[OK] Socket créé (desc=%d)\n", desc);

    /* ── 4. Préparation de l'adresse du serveur ────────────── */
    memset(&adresse_distante, 0, sizeof(adresse_distante));
    adresse_distante.sin_family = AF_INET;
    adresse_distante.sin_port   = htons(atoi(argv[2]));

    /* Conversion de l'IP texte → binaire */
    err = inet_pton(AF_INET, argv[1], &adresse_distante.sin_addr);
    if (err <= 0) {
        fprintf(stderr, "Erreur : adresse IP invalide '%s'\n", argv[1]);
        close(desc);
        exit(EXIT_FAILURE);
    }

    /* ── 5. Connexion au serveur (connect) ─────────────────── */
    err = connect(desc,
                  (struct sockaddr *)&adresse_distante,
                  sizeof(adresse_distante));
    if (err != 0) {
        perror("Erreur : connect");
        close(desc);
        exit(EXIT_FAILURE);
    }
    printf("[OK] Connecté au serveur %s:%s\n\n", argv[1], argv[2]);

    /* ── 6. Lance la session de chat ───────────────────────── */
    chat(desc);

    /* ── 7. Fermeture ──────────────────────────────────────── */
    close(desc);
    return 0;
}

/* ================================================================
 *  FONCTION CHAT
 * ================================================================ */
void chat(int desc_socket)
{
    char pseudo_local[TAILLE_MSG];    /* psl : pseudo client   */
    char pseudo_distant[TAILLE_MSG];  /* psd : pseudo serveur  */
    char message_envoye[TAILLE_MSG];  /* me  : message envoyé  */
    char message_recu[TAILLE_MSG];    /* mr  : message reçu    */

    /* ── Initialisation des buffers ────────────────────────── */
    memset(pseudo_local,   0, TAILLE_MSG);
    memset(pseudo_distant, 0, TAILLE_MSG);
    memset(message_envoye, 0, TAILLE_MSG);
    memset(message_recu,   0, TAILLE_MSG);

    /* ── Échange des pseudos ───────────────────────────────── */
    printf("Saisir votre pseudo : ");
    fgets(pseudo_local, TAILLE_MSG, stdin);
    pseudo_local[strcspn(pseudo_local, "\n")] = '\0';

    /* Reçoit d'abord le pseudo du serveur */
    recv(desc_socket, pseudo_distant, TAILLE_MSG - 1, 0);

    /* Envoie notre pseudo au serveur */
    send(desc_socket, pseudo_local, strlen(pseudo_local), 0);

    printf("Connecté avec : %s\n\n", pseudo_distant);
    printf("Tapez '%s' pour quitter.\n\n", MOT_QUITTER);

    /* ── Boucle de conversation ─────────────────────────────
     *   Protocole walkie-talkie : le client REÇOIT EN PREMIER
     *   (le serveur parle en premier), puis répond.
     * ─────────────────────────────────────────────────────── */
    do {
        /* --- Tour du serveur : réception --- */
        memset(message_recu, 0, TAILLE_MSG);
        recv(desc_socket, message_recu, TAILLE_MSG - 1, 0);

        printf("%s > %s\n", pseudo_distant, message_recu);

        if (strcmp(message_recu, MOT_QUITTER) == 0) break;

        /* --- Tour du client : envoi --- */
        memset(message_envoye, 0, TAILLE_MSG);
        printf("%s > ", pseudo_local);
        fgets(message_envoye, TAILLE_MSG, stdin);
        message_envoye[strcspn(message_envoye, "\n")] = '\0';

        send(desc_socket, message_envoye, strlen(message_envoye), 0);

    } while (strcmp(message_envoye, MOT_QUITTER) != 0);

    printf("[INFO] Session terminée.\n");
}
