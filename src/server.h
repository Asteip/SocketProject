#ifndef SERVER_H
#define SERVER_H

#define TAILLE_MAX_NOM 256 // Taille max du nom de la machine
#define TAILLE_MAX_MESSAGE 256 // Taille max d'un message : 226 (message) + 20 (pseudo) + 10(marge)
#define TAILLE_MAX_PSEUDO 20 // Taille max d'un pseudo
#define h_addr h_addr_list[0] // Résolution bug sur pc de la fac

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include "vector.h"

static const char W_CMD[] = "/w"; // Comamnde "message privé"
static const char L_CMD[] = "/l"; // Commande "liste utilisateurs"
static const char N_CMD[] = "/n"; // Commande "changer de nom"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct arg_thread {
	int sock;
	char* pseudo;
} arg_thread;

// FONCTIONS THREAD

void *connection(void *);

void *renvoi_message_unsend(void *);

// AUTRES FONCTIONS

void insert_message_unsend(char *, int , int);

#endif