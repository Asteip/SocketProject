/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

void *envoyer(int socket, char* msg){
	printf("Création du thread pour envoi de message \n");
	if ((write(socket, msg, strlen(msg))) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}
	printf("fermeture du thread \n");
	pthread_exit(NULL);
}

void *lire(int socket, char *buffer){
	printf("Création du thread pour lire un message \n");
	int longueur;
	while((longueur = read(socket, buffer, sizeof(buffer))) > 0) {
		printf("reponse du serveur : \n");
		write(1,buffer,longueur);
	}
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	/* descripteur de socket */
	int socket_descriptor; 
	
	/* longueur d'un buffer utilisé */
	int longueur; 
	
	/* adresse de socket local */
	sockaddr_in adresse_locale; 
	
	/* info sur une machine hote */
	hostent * ptr_host; 
	
	/* info sur service */
	servent * ptr_service; 
	
	/* nom du programme */
	char buffer[256];
	char * prog; 
	
	/* nom de la machine distante */
	char * host; 
	
	/* message envoyé */
	char * mesg; 

	/* thread pour l'envoi de message */
	pthread_t thread_envoi;

	/* thread pour la lecture de message */
	pthread_t thread_lecture;
	
	if (argc != 3) {
		perror("usage : client <adresse-serveur> <message-a-transmettre>");
		exit(1);
	}

	prog = argv[0];
	host = argv[1];
	mesg = argv[2];

	printf("nom de l'executable : %s \n", prog);
	printf("adresse du serveur  : %s \n", host);
	printf("message envoye      : %s \n", mesg);

	if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
	}

	/* copie caractere par caractere des infos de ptr_host vers adresse_locale */
	bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
	adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */

	/* SOLUTION 1 : utiliser un service existant, par ex. "irc" */
	/*
	if ((ptr_service = getservbyname("irc","tcp")) == NULL) {
		perror("erreur : impossible de recuperer le numero de port du service desire.");
		exit(1);
	}

	adresse_locale.sin_port = htons(ptr_service->s_port);
	*/

	/* SOLUTION 2 : utiliser un nouveau numero de port */

	adresse_locale.sin_port = htons(5000);

	printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));

	/* creation de la socket */
	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
	}

	/* tentative de connexion au serveur dont les infos sont dans adresse_locale */
	if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
	}

	printf("connexion etablie avec le serveur. \n");
	printf("envoi d'un message au serveur. \n");

	/* envoi du message vers le serveur 
	if ((write(socket_descriptor, mesg, strlen(mesg))) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}*/
	
	if(pthread_create(&thread_envoi, NULL, envoyer(socket_descriptor,mesg), NULL) != 0) {
		perror("pthread_envoi create");
		return EXIT_FAILURE;
	}

	/* mise en attente du prgramme pour simuler un delai de transmission */
	sleep(3);
	printf("message envoye au serveur. \n");

	 /*lecture de la reponse en provenance du serveur 
	while((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
		printf("reponse du serveur : \n");
		write(1,buffer,longueur);
	}*/
	printf("eto\n");
	if(pthread_create(&thread_lecture, NULL, lire(socket_descriptor,buffer), NULL) != 0) {
		perror("pthread_lecture create");
		return EXIT_FAILURE;
	}


	printf("\nfin de la reception.\n");
	close(socket_descriptor);
	printf("connexion avec le serveur fermee, fin du programme.\n");
	exit(0);
}
