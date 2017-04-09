#include "client.h"

/* entier égal à 1 tant que la connexion est établie */
int est_connecte = 0;

/* pseudo du client */
char *pseudo;

/* variables pour l'IHM */
vector_char *list_message = NULL;
WINDOW *haut, *bas;

void *envoi(void *pArgs){
	arg_thread_envoi *args = pArgs;

	/* envoi du message */
	int ret = write(args->sock, args->mesg, strlen(args->mesg));

	pthread_exit(NULL);
}

void *reception(void *pArgs){
	arg_thread_reception * args = pArgs;

	char buffer[TAILLE_MAX_MESSAGE];
	int buffer_size;

	memset(buffer,0,sizeof(buffer));

	/* message à afficher si l'utilisateur demande l'aide */
	char helpMsg[6][TAILLE_MAX_MESSAGE];
	strcpy(helpMsg[0], "Commandes disponibles : \n");
	strcat(helpMsg[1], "  /q : quitter\n");
	strcat(helpMsg[2], "  /w <pseudo> : message privé\n");
	strcat(helpMsg[3], "  /l : liste utilisateurs\n");
	strcat(helpMsg[4], "  /n <nouveau nom> : changer de pseudo\n");
	strcat(helpMsg[5], "  /h : help\n");

	while(est_connecte == 1 && (buffer_size = read(args->sock, buffer, sizeof(buffer))) > 0) {		
		char **splitMessage = traitementMessage(buffer);

		if(strlen(splitMessage[0]) == strlen(N_CMD) && strstr(splitMessage[0], N_CMD) != NULL){
			strcpy(pseudo, splitMessage[1]);

			attron(A_STANDOUT);
			mvprintw(0, (COLS / 2) - (strlen(pseudo) / 2), pseudo);
			attroff(A_STANDOUT);

			refresh();
		}
		else if(strlen(splitMessage[0]) == strlen(H_CMD) && strstr(splitMessage[0], H_CMD) != NULL){
			

			for(int nbh = 0 ; nbh < 6 ; ++nbh){
				for(int i = vector_char_size(list_message) -1 ; i > 0; --i){
					vector_char_set(list_message,i,vector_char_get(list_message,i-1));
				}
				vector_char_set(list_message,0,helpMsg[nbh]);
			}

			refresh_haut();
		}
		else{
			char *message = malloc (TAILLE_MAX_MESSAGE * sizeof (char));
			strcpy(message,buffer);

			for(int i = vector_char_size(list_message) -1 ; i > 0; --i){
				vector_char_set(list_message,i,vector_char_get(list_message,i-1));
			}

			vector_char_set(list_message,0,message);

			refresh_haut();
		}

		/* libération de la mémoire */
		for(int i = 0 ; i < 3 ; ++i)
			free(splitMessage[i]);

		free(splitMessage);

		memset(buffer,0,sizeof(buffer));
	}
	
	// arrêt du programme si le client ne reçoit plus de message
	est_connecte = 0;
	
	close(args->sock);
	
	pthread_exit(NULL);
}

 // Fonctions pour l'IHM

void refresh_haut(){
	wclear(haut);

	box(haut, ACS_VLINE, ACS_HLINE);
	mvwprintw(haut, 1, 1, "Messages :");

	for(int i=0; i < TAILLE_CONERSATION ; ++i){
		mvwprintw(haut, (2*LINES)/3 -3 -i,2,vector_char_get(list_message,i));
	}

	wrefresh(haut);
}

void refresh_bas(){
	wclear(bas);

    box(bas, ACS_VLINE, ACS_HLINE);
    mvwprintw(bas, 1, 1, "Répondre :");

	wrefresh(bas);

	move(2*LINES/3 + 2, 1);
}

char **traitementMessage(char *message){
	char **result = NULL;
	char tmp_message[TAILLE_MAX_MESSAGE];
	char *pch;
	int cpt = 0;

	result = malloc(sizeof(char*) * 3);
	
	for (int i = 0 ; i < 3 ; ++i) {
	    result[i] = (char *) malloc(TAILLE_MAX_MESSAGE);
	}

	strcpy(tmp_message, message);
	pch = strtok(tmp_message, " ");

	while(pch != NULL){
		
		if(cpt == 0){
			strcpy(result[0], pch);
			++cpt;
		}
		else if(cpt == 1){
			strcpy(result[1], pch);
			++cpt;
		}
		else{
			strcat(result[2], pch);
			strcat(result[2], " ");
			++cpt;
		}

		pch = strtok(NULL, " ");
	}

	free(pch);

	return result;
}

int main(int argc, char **argv) {
	/* descripteur de socket */
	int socket_descriptor; 
	
	/* adresse de socket local */
	sockaddr_in adresse_locale; 
	
	/* info sur une machine hote */
	hostent *ptr_host; 
	
	/* info sur service */
	servent *ptr_service; 
	
	/* nom du programme */
	char *prog; 
	
	/* nom de la machine distante */
	char *host; 

	/* thread pour l'envoi de message */
	pthread_t thread_envoi;
	arg_thread_envoi params_envoi;
	params_envoi.mesg = malloc(sizeof(char) * TAILLE_MAX_MESSAGE);

	/* thread pour la réception de message */
	pthread_t thread_reception;
	arg_thread_reception params_reception;

	/**********************************/
	/* initialisation de la connexion */
	/**********************************/
	
	if (argc != 3) {
		perror("usage : client <adresse-serveur> <pseudo (sans espace)>");
		exit(1);
	}

	prog = argv[0];
	host = argv[1];
	pseudo = argv[2];

	if (strlen(pseudo) > TAILLE_MAX_PSEUDO - 1){
		perror("erreur : le pseudo ne doit pas dépasser les 49 caracteres.");
		exit(1);
	}

	pseudo[strlen(pseudo) + 1] = '\0';

	if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
	}

	/* copie caractere par caractere des infos de ptr_host vers adresse_locale */
	bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
	adresse_locale.sin_family = AF_INET;

	adresse_locale.sin_port = htons(5000);

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

	/* envoi du pseudo au serveur */
	if ((write(socket_descriptor, pseudo, strlen(pseudo))) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}

	/* démarrage du thread de réception : est_connecte passe à 1 */
	est_connecte = 1;
	params_reception.sock = socket_descriptor;

	if(pthread_create(&thread_reception, NULL, reception, (void *) &params_reception) == -1) {
		perror("pthread_create : reception");
		exit(1);
    }
    
    /* initialisation de la conversation */
	list_message = vector_char_create() ;
	
	for(int i = 0; i < TAILLE_CONERSATION; ++i){
		vector_char_add(list_message,"");
	}
	
	/********************************/
	/* initialisation de la fenêtre */
	/********************************/

	initscr();

	/* affichage du pseudo */
	attron(A_STANDOUT);
	mvprintw(0, (COLS / 2) - (strlen(pseudo) / 2), pseudo);
	attroff(A_STANDOUT);

	refresh();

	/* création des parties "haut" et "bas" */
	haut = newwin(2*LINES /3  -1, COLS, 1, 0);
	bas = newwin(LINES / 3, COLS, 2*LINES / 3, 0);

	/* affichage fenêtre du haut */
	refresh_haut();

	/* affichage fenêtre du bas */
	refresh_bas();

	while(est_connecte == 1){
		char *message = malloc (sizeof (char) * TAILLE_MAX_MESSAGE);
		char **splitMessage = NULL;

		memset(message,0,sizeof(message));

		wgetnstr(bas, message, TAILLE_MSG_SAISIE);

		refresh_bas();

		if(message != NULL && (strcmp(message, "") != 0)){

			/* traitement du message avant l'envoi */
			splitMessage = traitementMessage(message);

			if(strlen(splitMessage[0]) == strlen(Q_CMD) && strstr(splitMessage[0], Q_CMD) != NULL){ // COMMANDE q (quitter le serveur)
				est_connecte = 0;
			}
			else{
				params_envoi.sock = socket_descriptor;
				memset(params_envoi.mesg,0,sizeof(params_envoi.mesg));
				strcpy(params_envoi.mesg, message);

				if(pthread_create(&thread_envoi, NULL, envoi, (void *) &params_envoi) == -1) {
					perror("pthread_create : probleme lors de la creation du thread d'envoi.");
					exit(1);
				}

				/* on attend que le client ait prévenu le serveur de sa déconnexion avant de quitter */
				if(est_connecte == 0){
					(void) pthread_join(thread_reception, NULL);
					endwin();
				}
			}

			/* libération de la mémoire */
			for(int i = 0 ; i < 3 ; ++i)
				free(splitMessage[i]);

			free(splitMessage);
			free(message);
		}
	}

	endwin();              

	exit(0);
}
