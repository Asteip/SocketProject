#include "client.h"

/* entier égal à 1 tant que la connexion est établie */
int is_connected = 0;

void *envoi(void *pArgs){
	arg_thread_envoi *args = pArgs;

	int cpt = 0;

	/* variables de pretraitements */
	char tmp_buffer[TAILLE_MAX_MESSAGE];
	char partie1[TAILLE_MAX_MESSAGE];
	char partie2[TAILLE_MAX_MESSAGE];
	char partie3[TAILLE_MAX_MESSAGE];

	/* on clean les buffer avant de commencer */
	memset(tmp_buffer,0,sizeof(args->mesg));
	memset(partie1,0,sizeof(args->mesg));
	memset(partie2,0,sizeof(args->mesg));
	memset(partie3,0,sizeof(args->mesg));

	/* pretraitements */

	char *pch;

	strcpy(tmp_buffer, args->mesg);
	pch = strtok(tmp_buffer, " ");

	while(pch != NULL){
		
		if(cpt == 0){
			strcpy(partie1, pch);
			++cpt;
		}
		else if(cpt == 1){
			strcpy(partie2, pch);
			++cpt;
		}
		else{
			strcat(partie3, pch);
			strcat(partie3, " ");
			++cpt;
		}

		pch = strtok(NULL, " ");
	}

	if(strlen(partie1) == strlen(Q_CMD) && strstr(partie1, Q_CMD) != NULL){ // COMMANDE quit (quitter le serveur)
		is_connected = 0;
		printf("Déconnexion... \n");

		/* envoi du message */
		if ((write(args->sock, args->mesg, strlen(args->mesg))) < 0)
			printf("erreur : impossible d'ecrire le message destine au serveur.\n");
	}
	else if(strlen(partie1) == strlen(F_CMD) && strstr(partie1, F_CMD) != NULL){ // COMMANDE file (envoi de fichier)
		
		// TODO A implémenter -> envoi de fichier.

	}
	else{
		/* envoi du message */
		if ((write(args->sock, args->mesg, strlen(args->mesg))) < 0) 
			printf("erreur : impossible d'ecrire le message destine au serveur.\n");
	}

	free(pch);

	pthread_exit(NULL);
}

void *reception(void *pArgs){
	arg_thread_reception * args = pArgs;

	char buffer[TAILLE_MAX_MESSAGE];
	int buffer_size;

	memset(buffer,0,sizeof(buffer));

	while(is_connected == 1 && (buffer_size = read(args->sock, buffer, sizeof(buffer))) > 0) {		
		printf("\nReponse du serveur : \n");

		if((write(1,buffer,buffer_size)) < 0)
			printf("erreur : impossible d'ecrire le message sur la sortie standard.\n");

		printf("\n");
		
		memset(buffer,0,sizeof(buffer));
	}
	
	// arrêt du programme si le client ne reçoit plus de message
	is_connected = 0;
	
	printf("\nfin de la reception.\n");
	close(args->sock);
	printf("connexion avec le serveur fermee, fin du programme.\n");
	
	pthread_exit(NULL);
}

void clean(const char *buffer, FILE *fp){
    char *p = strchr(buffer,'\n');
    if (p != NULL)
        *p = 0;
    else{
        int c;
        while ((c = fgetc(fp)) != '\n' && c != EOF);
    }
}

int main(int argc, char **argv) {
	/* descripteur de socket */
	int socket_descriptor; 
	
	/* adresse de socket local */
	sockaddr_in adresse_locale; 
	
	/* info sur une machine hote */
	hostent * ptr_host; 
	
	/* info sur service */
	servent * ptr_service; 
	
	/* nom du programme */
	char * prog; 
	
	/* nom de la machine distante */
	char * host; 

	/* pseudo du client */
	char * pseudo;
	
	/* message envoyé */
	char mesg[TAILLE_MAX_MESSAGE] = "";

	/* thread pour l'envoi de message */
	pthread_t thread_envoi;
	arg_thread_envoi params_envoi;
	params_envoi.mesg = malloc(sizeof(char) * TAILLE_MAX_MESSAGE);

	/* thread pour la lecture de message */
	pthread_t thread_reception;
	arg_thread_reception params_reception;
	
	if (argc != 3) {
		perror("usage : client <adresse-serveur> <pseudo (sans espace)>");
		exit(1);
	}

	prog = argv[0];
	host = argv[1];
	pseudo = argv[2];

	if (strlen(pseudo) > TAILLE_MAX_PSEUDO - 1){
		perror("erreur : le pseudo ne doit pas dépasser les 49 caracteres");
		exit(1);
	}

	pseudo[strlen(pseudo) + 1] = '\0';

	if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
	}

	/* copie caractere par caractere des infos de ptr_host vers adresse_locale */
	bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
	adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */

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

	//Envoi du pseudo au serveur
	if ((write(socket_descriptor, pseudo, strlen(pseudo))) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}

	is_connected = 1;

	params_reception.sock = socket_descriptor;

	if(pthread_create(&thread_reception, NULL, reception, (void *) &params_reception) == -1) {
		perror("pthread_create : reception");
		exit(1);
    }

	while(is_connected == 1){
		printf("Message (inferieur a 255 caracteres) : ");
    	
    	if((fgets(mesg, sizeof(mesg), stdin)) == NULL)
    		printf("erreur : impossible de recupere le message saisie.\n");

    	clean(mesg, stdin);

		params_envoi.sock = socket_descriptor;
		memset(params_envoi.mesg,0,sizeof(params_envoi.mesg));
    	strcpy(params_envoi.mesg, mesg);

    	if(pthread_create(&thread_envoi, NULL, envoi, (void *) &params_envoi) == -1) {
			perror("pthread_create : probleme lors de la creation du thread d'envoi.");
			exit(1);
		}
	}	

	exit(0);
}
