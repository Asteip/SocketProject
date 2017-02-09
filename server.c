/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h> 
#include <pthread.h>
#include "vector.h"

/* pour les sockets */
#include <sys/socket.h>
#include <netdb.h> 

/* pour hostent, servent */
#include <string.h> 

/* pour bcopy, ... */  
#define TAILLE_MAX_NOM 256

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct arg_thread {
	int sock;
} arg_thread;

/* déclaration du vector qui va contenir tous les clients */
vector list_client;

void *connection(void * pArgs){
	char buffer[256];
	int longueur;
	arg_thread * args = pArgs;

	while((longueur = read(args->sock, buffer, sizeof(buffer))) > 0){

		sleep(3);

		/* envoi du message a tous les autres utilisateurs */
		for(int i = 0 ; i < vector_total(&list_client) ; i++){
			write(vector_get(&list_client, i), buffer, strlen(buffer)+1);
		}
	}

	close((int) args->sock);
	pthread_exit(NULL);
}

main(int argc, char **argv) {
	/* descripteur de socket */
	int socket_descriptor; 

	/* [nouveau] descripteur de socket */
	int nouv_socket_descriptor; 

	/* longueur d'adresse courante d'un client */
	int longueur_adresse_courante; 

	/* structure d'adresse locale*/
	sockaddr_in adresse_locale; 

	/* adresse client courant */
	sockaddr_in adresse_client_courant; 

	/* les infos recuperees sur la machine hote */
	hostent* ptr_hote; 

	/* les infos recuperees sur le service de la machine */
	servent* ptr_service; 

	/* nom de la machine locale */
	char machine[TAILLE_MAX_NOM+1]; 

	/* recuperation du nom de la machine */
	gethostname(machine,TAILLE_MAX_NOM);
	

	/* thread pour chaque connexion cliente */
	pthread_t thread;

	/* arguments de chaque thread */
	arg_thread params;

	/* vector contenant chaque connexion cliente */
	vector_init(&list_client);
	
	/* recuperation de la structure d'adresse en utilisant le nom */
	if ((ptr_hote = gethostbyname("localhost")) == NULL) { //localhost en dur car problème pc Sitraka
		perror("erreur : impossible de trouver le serveur a partir de son nom.");
		exit(1);
	}

	/* initialisation de la structure adresse_locale avec les infos recuperees */
	/* copie de ptr_hote vers adresse_locale */
	bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
	adresse_locale.sin_family = ptr_hote->h_addrtype; 

	/* ou AF_INET */
	adresse_locale.sin_addr.s_addr = INADDR_ANY; 

	adresse_locale.sin_port = htons(5000);

	printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));
	
	/* creation de la socket */
	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le client.");
		exit(1);
	}

	/* association du socket socket_descriptor à la structure d'adresse adresse_locale */
	if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de lier la socket a l'adresse de connexion.");
		exit(1);
	}

	/* initialisation de la file d'ecoute */
	listen(socket_descriptor,5);

	/* attente des connexions et traitement des donnees recues */
	for(;;) {
		longueur_adresse_courante = sizeof(adresse_client_courant);

		/* adresse_client_courant sera renseignée par accept via les infos du connect */
		if ((nouv_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&adresse_client_courant), &longueur_adresse_courante)) < 0) {
			perror("erreur : impossible d'accepter la connexion avec le client.");
			exit(1);
		}
		else{
			vector_add(&list_client, nouv_socket_descriptor);
			params.sock = nouv_socket_descriptor;
			if(pthread_create(&thread, NULL, connection, (void *) &params) == -1) {
				perror("pthread_create");
				exit(1);
		    }
		}
	}    

	exit(0);
}
