#ifndef CLIENT_H
#define CLIENT_H

#define TAILLE_MAX_MESSAGE 256 // Taille max d'un message : 226 (message) + 20 (pseudo) + 10(marge)
#define TAILLE_MSG_SAISIE 226
#define TAILLE_MAX_PSEUDO 50
#define TAILLE_CONERSATION 8 // taille de la conversation
#define h_addr h_addr_list[0]

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include "vector.h"

static const char Q_CMD[] = "/q"; // Commande "quitter"
static const char L_CMD[] = "/l"; // Commande "liste utilisateurs"
static const char N_CMD[] = "/n"; // Commande "changer de nom"

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

// FONCTIONS THREAD

void *envoi(void *);

void *reception(void *);

// FONCTIONS IHM

void refresh_haut();

void refresh_bas();

// AUTRES FONCTIONS

char **traitementMessage(char *);

#endif


/*
char helpMsg[TAILLE_MAX_MESSAGE];
strcpy(helpMsg, "Commandes disponibles : \n");
strcat(helpMsg, "  /q : quitter\n");
strcat(helpMsg, "  /w <pseudo> : message privé\n");
strcat(helpMsg, "  /l : liste utilisateurs\n");
strcat(helpMsg, "  /n <nouveau nom> : changer de pseudo\n");
strcat(helpMsg, "  /h : help\n");

*/