/*
 * ============================================================
 *   SERVEUR  –  Chat Walkie-Talkie  (TCP / POSIX Sockets)
 * ============================================================
 * Compilation : gcc -o serveur serveur.c
 * Utilisation : ./serveur <port>
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>   /* inet_ntoa, htons … */
#include <sys/socket.h>
#include <sys/types.h>

/* ── Constantes ─────────────────────────────────────────── */
#define TAILLE_MSG   256
#define MAX_CONN     5
#define MOT_QUITTER  "quitter"

/* ── Prototype ───────────────────────────────────────────── */
void chat(int socket_client, const char *ip_client, int port_client);

/* ================================================================
 *  MAIN
 * ================================================================ */
int main(int argc, char *argv[])
{
    /* ── 1. Vérification du nombre d'arguments ─────────────── */
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* ── 2. Initialisation des variables ───────────────────── */
    int    desc_serveur;           /* descripteur socket serveur   */
    int    desc_client;            /* descripteur socket client     */
    int    err;
    struct sockaddr_in adresse_locale;
    struct sockaddr_in adresse_distante;
    socklen_t taille_addr = sizeof(adresse_distante);

    /* ── 3. Création du socket ─────────────────────────────── */
    desc_serveur = socket(AF_INET, SOCK_STREAM, 0);
    if (desc_serveur < 0) {
        perror("Erreur : création du socket");
        exit(EXIT_FAILURE);
    }
    printf("[OK] Socket créé (desc=%d)\n", desc_serveur);

    /* ── 4. Préparation de l'adresse locale ────────────────── */
    memset(&adresse_locale, 0, sizeof(adresse_locale));
    adresse_locale.sin_family      = AF_INET;
    adresse_locale.sin_addr.s_addr = INADDR_ANY;          /* toutes interfaces */
    adresse_locale.sin_port        = htons(atoi(argv[1])); /* port en arg       */

    /* ── 5. Liaison (bind) ─────────────────────────────────── */
    err = bind(desc_serveur,
               (struct sockaddr *)&adresse_locale,
               sizeof(adresse_locale));
    if (err != 0) {
        perror("Erreur : bind");
        close(desc_serveur);
        exit(EXIT_FAILURE);
    }
    printf("[OK] Bind sur le port %s\n", argv[1]);

    /* ── 6. Écoute ─────────────────────────────────────────── */
    err = listen(desc_serveur, MAX_CONN);
    if (err != 0) {
        perror("Erreur : listen");
        close(desc_serveur);
        exit(EXIT_FAILURE);
    }
    printf("[OK] Serveur en écoute … (port %s)\n\n", argv[1]);

    /* ── 7. Boucle principale d'acceptation ────────────────── */
    while (1) {
        desc_client = accept(desc_serveur,
                             (struct sockaddr *)&adresse_distante,
                             &taille_addr);
        if (desc_client > 0) {
            char *ip_client   = inet_ntoa(adresse_distante.sin_addr);
            int   port_client = ntohs(adresse_distante.sin_port);

            printf("✔  Client connecté : %s:%d\n", ip_client, port_client);

            /* Lance la session de chat */
            chat(desc_client, ip_client, port_client);

            close(desc_client);
            printf("[INFO] Client déconnecté.\n\n");
        }
    }

    close(desc_serveur);
    return 0;
}

/* ================================================================
 *  FONCTION CHAT
 * ================================================================ */
void chat(int socket_client, const char *ip_client, int port_client)
{
    char pseudo_local[TAILLE_MSG];    /* psl : pseudo serveur  */
    char pseudo_distant[TAILLE_MSG];  /* psd : pseudo client   */
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
    pseudo_local[strcspn(pseudo_local, "\n")] = '\0';   /* retire le \n */

    /* Envoie notre pseudo au client */
    send(socket_client, pseudo_local, strlen(pseudo_local), 0);

    /* Reçoit le pseudo du client */
    recv(socket_client, pseudo_distant, TAILLE_MSG - 1, 0);

    printf("Connecté avec : %s (%s:%d)\n\n", pseudo_distant, ip_client, port_client);
    printf("Tapez '%s' pour quitter.\n\n", MOT_QUITTER);

    /* ── Boucle de conversation ─────────────────────────────
     *   Protocole walkie-talkie : serveur parle EN PREMIER,
     *   puis reçoit la réponse du client, et ainsi de suite.
     * ─────────────────────────────────────────────────────── */
    do {
        /* --- Tour du serveur : envoi --- */
        memset(message_envoye, 0, TAILLE_MSG);
        printf("%s > ", pseudo_local);
        fgets(message_envoye, TAILLE_MSG, stdin);
        message_envoye[strcspn(message_envoye, "\n")] = '\0';

        send(socket_client, message_envoye, strlen(message_envoye), 0);

        if (strcmp(message_envoye, MOT_QUITTER) == 0) break;

        /* --- Tour du client : réception --- */
        memset(message_recu, 0, TAILLE_MSG);
        recv(socket_client, message_recu, TAILLE_MSG - 1, 0);

        printf("%s > %s\n", pseudo_distant, message_recu);

    } while (strcmp(message_recu, MOT_QUITTER) != 0);

    printf("[INFO] Session terminée.\n");
}
