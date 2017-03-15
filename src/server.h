#ifndef SERVER_H
#define SERVER_H

#define TAILLE_MAX_NOM 256
#define TAILLE_MAX_MESSAGE 256
#define TAILLE_MAX_PSEUDO 50

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include "vector.h"

static const char Q_CMD[] = "/q"; // Commande "quitter"
static const char W_CMD[] = "/w"; // Comamnde "message privé"
static const char F_CMD[] = "/f"; // Commande "envoi de fichier"
static const char L_CMD[] = "/l"; // Commande "liste utilisateurs"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct arg_thread {
	int sock;
	char* pseudo;
} arg_thread;

void *connection(void *);

char* join_strings(vector_char *strings, int count);

#endif