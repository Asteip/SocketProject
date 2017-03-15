#ifndef CLIENT_H
#define CLIENT_H

#define TAILLE_MAX_PSEUDO 50
#define TAILLE_MAX_MESSAGE 256

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>

static const char Q_CMD[] = "/q"; // Commande "quitter"
static const char F_CMD[] = "/f"; // Commande "envoi de fichier"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct arg_thread_envoi {
	int sock;
	char *mesg;
} arg_thread_envoi;

typedef struct arg_thread_reception {
	int sock;
} arg_thread_reception;

void *envoi(void *);

void *reception(void *);

void clean(const char *, FILE *);

#endif