#include "client.h"

/* entier égal à 1 tant que la connexion est établie */
int est_connecte = 0;

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

	while(est_connecte == 1 && (buffer_size = read(args->sock, buffer, sizeof(buffer))) > 0) {		
		char* message = malloc (TAILLE_MAX_MESSAGE * sizeof (char));
		strcpy(message,buffer);

		for(int i = vector_char_size(list_message) -1 ; i > 0; --i){
			vector_char_set(list_message,i,vector_char_get(list_message,i-1));
		}

		vector_char_set(list_message,0,message);

		refresh_haut();

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

	/* pseudo du client */
	char *pseudo;

	/* thread pour l'envoi de message */
	pthread_t thread_envoi;
	arg_thread_envoi params_envoi;
	params_envoi.mesg = malloc(sizeof(char) * TAILLE_MAX_MESSAGE);

	/* thread pour la réception de message */
	pthread_t thread_reception;
	arg_thread_reception params_reception;

	/* variables de pretraitements */
	int cpt = 0;
	
	char tmp_message[TAILLE_MAX_MESSAGE];
	char partie1[TAILLE_MAX_MESSAGE];
	char partie2[TAILLE_MAX_MESSAGE];
	char partie3[TAILLE_MAX_MESSAGE];

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


	/* on clean les buffer avant de commencer */
	memset(tmp_message,0,sizeof(tmp_message));
	memset(partie1,0,sizeof(partie1));
	memset(partie2,0,sizeof(partie2));
	memset(partie3,0,sizeof(partie3));

	while(est_connecte == 1){
		char *message = malloc (sizeof (char) * TAILLE_MAX_MESSAGE);
		char *pch;

		memset(message,0,sizeof(message));

		wgetnstr(bas, message, TAILLE_MSG_SAISIE);

		refresh_bas();

		/* traitement du message avant l'envoi */
		if(message != NULL && (strcmp(message, "") != 0)){

			strcpy(tmp_message, message);
			pch = strtok(tmp_message, " ");

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

			if(strlen(partie1) == strlen(Q_CMD) && strstr(partie1, Q_CMD) != NULL){ // COMMANDE q (quitter le serveur)
				est_connecte = 0;
			}
			else{
				if(strlen(partie1) == strlen(N_CMD) && strcmp(partie1, N_CMD) == 0){ // COMMANDE n (changement de pseudo)
					if(strlen(partie2) < TAILLE_MAX_PSEUDO){
						strcpy(pseudo, partie2);

						attron(A_STANDOUT);
						mvprintw(0, (COLS / 2) - (strlen(pseudo) / 2), pseudo);
						attroff(A_STANDOUT);
					}

					refresh();
				}

				/* clean du buffer message */
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

			free(pch);
			free(message);
		}
	}

	endwin();              

	exit(0);
}
