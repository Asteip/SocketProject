#ifndef SERVER_H
#define SERVER_H

#define TAILLE_MAX_NOM 256
#define TAILLE_MAX_MESSGE 256
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