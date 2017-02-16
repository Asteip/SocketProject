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

typedef struct arg_thread_envoi {
	int sock;
	char *mesg;
} arg_thread_envoi;

typedef struct arg_thread_reception {
	int sock;
} arg_thread_reception;

/* entier égal à 1 tant que la connexion est établie */
int is_connect = 0;

void *envoi(void *pArgs){
	arg_thread_envoi *args = pArgs;

	char quitCmd[] = "/quit";
	char wCmd[] = "/w";

	if(args->mesg[0] == '/'){
		if(strstr(args->mesg, quitCmd) != NULL){
			is_connect = 0;
			printf("Déconnexion... \n");
		}

		if(strstr(args->mesg, wCmd) != NULL){
			// A implémenter -> fonction message privé.
		}
	}

	if ((write(args->sock, args->mesg, strlen(args->mesg))) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}

	//sleep(3);

	pthread_exit(NULL);
}

void *reception(void *pArgs){
	char buffer[256];
	int longueur;
	arg_thread_reception * args = pArgs;

	while((longueur = read(args->sock, buffer, sizeof(buffer))) > 0) {
		printf("Reponse du serveur : \n");
		write(1,buffer,longueur);
	}

	is_connect = 0;
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
	
	/* message envoyé */
	char mesg[256] = "";

	/* thread pour l'envoi de message */
	pthread_t thread_envoi;
	arg_thread_envoi params_envoi;

	/* thread pour la lecture de message */
	pthread_t thread_reception;
	arg_thread_reception params_reception;
	
	if (argc != 2) {
		perror("usage : client <adresse-serveur>");
		exit(1);
	}

	prog = argv[0];
	host = argv[1];

	printf("nom de l'executable : %s \n", prog);
	printf("adresse du serveur  : %s \n", host);

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

	is_connect = 1;

	params_reception.sock = socket_descriptor;

	if(pthread_create(&thread_reception, NULL, reception, (void *) &params_reception) == -1) {
		perror("pthread_create : reception");
		exit(1);
    }

	while(is_connect == 1){
		printf("Message: ");
    	fgets(mesg, sizeof(mesg), stdin);
    	clean(mesg, stdin);

    	params_envoi.sock = socket_descriptor;
    	params_envoi.mesg = mesg;

    	if(pthread_create(&thread_envoi, NULL, envoi, (void *) &params_envoi) == -1) {
			perror("pthread_create ; envoi");
			exit(1);
		}
	}	

	printf("\nfin de la reception.\n");
	close(socket_descriptor);
	printf("connexion avec le serveur fermee, fin du programme.\n");

	exit(0);
}