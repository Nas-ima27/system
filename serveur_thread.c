/*
 * ============================================================
 *   SERVEUR  –  Chat Non-Bloquant avec Threads (pthreads)
 * ============================================================
 * Compilation : gcc -o serveur_thread serveur_thread.c -lpthread
 * Utilisation : ./serveur_thread <port>
 *
 * Architecture :
 *   - Thread RECEPTION : reçoit les messages du client en continu
 *   - Thread ENVOI     : lit le clavier et envoie en continu
 *   Les deux tournent EN PARALLELE → non bloquant
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

/* ── Constantes ─────────────────────────────────────────── */
#define TAILLE_MSG  256
#define MAX_CONN    5
#define MOT_QUITTER "quitter"

/* ── Variables globales partagées entre threads ─────────── */
int    socket_client;                /* socket partagé          */
char   pseudo_local[TAILLE_MSG];     /* notre pseudo            */
char   pseudo_distant[TAILLE_MSG];   /* pseudo du client        */
int    session_active = 1;           /* flag d'arrêt            */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* ================================================================
 *  THREAD RECEPTION  –  reçoit les messages du client
 * ================================================================ */
void *thread_reception(void *arg)
{
    char message_recu[TAILLE_MSG];

    while (session_active) {
        memset(message_recu, 0, TAILLE_MSG);
        int n = recv(socket_client, message_recu, TAILLE_MSG - 1, 0);

        if (n <= 0) {
            /* Client déconnecté ou erreur */
            printf("\n[INFO] Client déconnecté.\n");
            pthread_mutex_lock(&mutex);
            session_active = 0;
            pthread_mutex_unlock(&mutex);
            break;
        }

        /* Affiche le message reçu */
        printf("\r%s > %s\n%s > ", pseudo_distant, message_recu, pseudo_local);
        fflush(stdout);

        if (strcmp(message_recu, MOT_QUITTER) == 0) {
            printf("[INFO] %s a quitté la session.\n", pseudo_distant);
            pthread_mutex_lock(&mutex);
            session_active = 0;
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
    return NULL;
}

/* ================================================================
 *  THREAD ENVOI  –  lit le clavier et envoie au client
 * ================================================================ */
void *thread_envoi(void *arg)
{
    char message_envoye[TAILLE_MSG];

    while (session_active) {
        memset(message_envoye, 0, TAILLE_MSG);
        printf("%s > ", pseudo_local);
        fflush(stdout);

        if (fgets(message_envoye, TAILLE_MSG, stdin) == NULL) break;
        message_envoye[strcspn(message_envoye, "\n")] = '\0';

        send(socket_client, message_envoye, strlen(message_envoye), 0);

        if (strcmp(message_envoye, MOT_QUITTER) == 0) {
            pthread_mutex_lock(&mutex);
            session_active = 0;
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
    return NULL;
}

/* ================================================================
 *  FONCTION CHAT  –  lance les deux threads
 * ================================================================ */
void chat(int sock)
{
    socket_client  = sock;
    session_active = 1;

    /* ── Échange des pseudos ───────────────────────────────── */
    memset(pseudo_local,   0, TAILLE_MSG);
    memset(pseudo_distant, 0, TAILLE_MSG);

    printf("Saisir votre pseudo : ");
    fgets(pseudo_local, TAILLE_MSG, stdin);
    pseudo_local[strcspn(pseudo_local, "\n")] = '\0';

    send(socket_client, pseudo_local, strlen(pseudo_local), 0);
    recv(socket_client, pseudo_distant, TAILLE_MSG - 1, 0);

    printf("\n✔ Connecté avec : %s\n", pseudo_distant);
    printf("Tapez '%s' pour quitter.\n\n", MOT_QUITTER);

    /* ── Création des deux threads ─────────────────────────── */
    pthread_t tid_recv, tid_send;

    pthread_create(&tid_recv, NULL, thread_reception, NULL);
    pthread_create(&tid_send, NULL, thread_envoi,     NULL);

    /* ── Attendre la fin des deux threads ──────────────────── */
    pthread_join(tid_recv, NULL);
    pthread_join(tid_send, NULL);

    printf("[INFO] Session terminée.\n\n");
}

/* ================================================================
 *  MAIN
 * ================================================================ */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int desc_serveur;
    struct sockaddr_in adresse_locale, adresse_distante;
    socklen_t taille = sizeof(adresse_distante);

    /* Création socket */
    desc_serveur = socket(AF_INET, SOCK_STREAM, 0);
    if (desc_serveur < 0) { perror("socket"); exit(EXIT_FAILURE); }

    /* Option pour réutiliser le port immédiatement */
    int opt = 1;
    setsockopt(desc_serveur, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Adresse locale */
    memset(&adresse_locale, 0, sizeof(adresse_locale));
    adresse_locale.sin_family      = AF_INET;
    adresse_locale.sin_addr.s_addr = INADDR_ANY;
    adresse_locale.sin_port        = htons(atoi(argv[1]));

    /* Bind */
    if (bind(desc_serveur, (struct sockaddr *)&adresse_locale, sizeof(adresse_locale)) != 0) {
        perror("bind"); close(desc_serveur); exit(EXIT_FAILURE);
    }

    /* Listen */
    if (listen(desc_serveur, MAX_CONN) != 0) {
        perror("listen"); close(desc_serveur); exit(EXIT_FAILURE);
    }

    printf("[OK] Serveur en écoute sur le port %s ...\n", argv[1]);

    /* Boucle d'acceptation */
    while (1) {
        int desc_client = accept(desc_serveur,
                                 (struct sockaddr *)&adresse_distante,
                                 &taille);
        if (desc_client > 0) {
            printf("✔ Client connecté : %s:%d\n",
                   inet_ntoa(adresse_distante.sin_addr),
                   ntohs(adresse_distante.sin_port));

            chat(desc_client);
            close(desc_client);
        }
    }

    close(desc_serveur);
    return 0;
}
