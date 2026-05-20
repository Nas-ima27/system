/*
 * ============================================================
 *   CLIENT  –  Chat Non-Bloquant avec Threads (pthreads)
 * ============================================================
 * Compilation : gcc -o client_thread client_thread.c -lpthread
 * Utilisation : ./client_thread <ip_serveur> <port>
 *
 * Architecture identique au serveur :
 *   - Thread RECEPTION : reçoit en permanence
 *   - Thread ENVOI     : envoie en permanence
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
#define MOT_QUITTER "quitter"

/* ── Variables globales partagées ───────────────────────── */
int  desc_socket;
char pseudo_local[TAILLE_MSG];
char pseudo_distant[TAILLE_MSG];
int  session_active = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* ================================================================
 *  THREAD RECEPTION
 * ================================================================ */
void *thread_reception(void *arg)
{
    char message_recu[TAILLE_MSG];

    while (session_active) {
        memset(message_recu, 0, TAILLE_MSG);
        int n = recv(desc_socket, message_recu, TAILLE_MSG - 1, 0);

        if (n <= 0) {
            printf("\n[INFO] Serveur déconnecté.\n");
            pthread_mutex_lock(&mutex);
            session_active = 0;
            pthread_mutex_unlock(&mutex);
            break;
        }

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
 *  THREAD ENVOI
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

        send(desc_socket, message_envoye, strlen(message_envoye), 0);

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
    desc_socket    = sock;
    session_active = 1;

    memset(pseudo_local,   0, TAILLE_MSG);
    memset(pseudo_distant, 0, TAILLE_MSG);

    /* Le client reçoit le pseudo du serveur EN PREMIER */
    recv(desc_socket, pseudo_distant, TAILLE_MSG - 1, 0);

    printf("Saisir votre pseudo : ");
    fgets(pseudo_local, TAILLE_MSG, stdin);
    pseudo_local[strcspn(pseudo_local, "\n")] = '\0';

    send(desc_socket, pseudo_local, strlen(pseudo_local), 0);

    printf("\n✔ Connecté avec : %s\n", pseudo_distant);
    printf("Tapez '%s' pour quitter.\n\n", MOT_QUITTER);

    /* ── Création des deux threads ─────────────────────────── */
    pthread_t tid_recv, tid_send;

    pthread_create(&tid_recv, NULL, thread_reception, NULL);
    pthread_create(&tid_send, NULL, thread_envoi,     NULL);

    pthread_join(tid_recv, NULL);
    pthread_join(tid_send, NULL);

    printf("[INFO] Session terminée.\n");
}

/* ================================================================
 *  MAIN
 * ================================================================ */
int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage : %s <ip_serveur> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in adresse_distante;

    desc_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (desc_socket < 0) { perror("socket"); exit(EXIT_FAILURE); }

    memset(&adresse_distante, 0, sizeof(adresse_distante));
    adresse_distante.sin_family = AF_INET;
    adresse_distante.sin_port   = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &adresse_distante.sin_addr) <= 0) {
        fprintf(stderr, "IP invalide : %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (connect(desc_socket, (struct sockaddr *)&adresse_distante,
                sizeof(adresse_distante)) != 0) {
        perror("connect"); exit(EXIT_FAILURE);
    }

    printf("[OK] Connecté au serveur %s:%s\n\n", argv[1], argv[2]);

    chat(desc_socket);

    close(desc_socket);
    return 0;
}
