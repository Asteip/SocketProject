#include "server.h"

/* vector qui va contenir tous les clients */
vector_int *list_client = NULL;

/* vector qui va contenir tous les pseudos */
vector_char *list_pseudo = NULL;

/* vector qui va contenir les messages non transmis suite à une erreur */
vector_char *list_unsend_msg = NULL;

/* vector qui va contenir les clients destinataires des messages non transmis */
vector_int *list_unsend_sock = NULL;

/* vector qui va contenir les compteurs permettant de gérer les timeout des messages non transmis */
vector_int *list_unsend_cpt = NULL;

void *connection(void *pArgs){
	arg_thread *args = pArgs;

	char buffer[TAILLE_MAX_MESSAGE];
	char mesg_erreur[TAILLE_MAX_MESSAGE];

	int long_buffer = 0;
	int est_connecte = 1;
	int index_client = 0;
	int cpt = 0;

	/* variables de pretraitements */
	char tmp_buffer[TAILLE_MAX_MESSAGE];
	char partie1[TAILLE_MAX_MESSAGE];
	char partie2[TAILLE_MAX_MESSAGE];
	char partie3[TAILLE_MAX_MESSAGE];

	/* on clean les buffer avant de commencer */
	memset(buffer,0,sizeof(buffer));
	memset(tmp_buffer,0,sizeof(tmp_buffer));
	memset(partie1,0,sizeof(partie1));
	memset(partie2,0,sizeof(partie2));
	memset(partie3,0,sizeof(partie3));

	printf("Connexion du client %s (num : %d).\n", args->pseudo, args->sock);

	/* on previent les autres utilisateurs de la connexion d'un nouveau client */
	char whoConnect[TAILLE_MAX_MESSAGE];

	strcpy(whoConnect, "* ");
	strcat(whoConnect, args->pseudo);
	strcat(whoConnect, " s'est connecté. *");

	for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
					
		if((write(vector_int_get(list_client, i), whoConnect, strlen(whoConnect) + 1)) < 0){
			printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
			insert_message_unsend(whoConnect, vector_int_get(list_client, i), 3);
		}
	}

	/* tant que l'utilisateur est connecté, on lit les messages reçus */
	while((long_buffer = read(args->sock, buffer, sizeof(buffer))) > 0 && est_connecte == 1){

		/* prétraitements du message reçu : on sépare la partie commande et options de la partie message */

		char *pch;

		cpt = 0;
		strcpy(tmp_buffer, buffer);
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
		
		/* Si le début de la ligne commence par "/" on regarde si c'est une commande existante */
		if(buffer[0] == '/'){
			
			if(strlen(partie1) == strlen(Q_CMD) && strcmp(partie1, Q_CMD) == 0){ // COMMANDE q (quitter le serveur)
				char whoQuit[TAILLE_MAX_MESSAGE];

				strcpy(whoQuit, "* ");
				strcat(whoQuit, args->pseudo);
				strcat(whoQuit, " s'est déconnecté. *");

				for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
					
					if((write(vector_int_get(list_client, i), whoQuit, strlen(whoQuit) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
						insert_message_unsend(whoQuit, vector_int_get(list_client, i), 3);
					}
				}

				est_connecte = 0;
			}
			else if(strlen(partie1) == strlen(W_CMD) && strcmp(partie1, W_CMD) == 0){ // COMMANDE w (message privé)
				int dest = vector_char_search(list_pseudo, partie2);
				char whoSend[TAILLE_MAX_MESSAGE];

				if(dest != -1){
					strcpy(whoSend, "(");
					strcat(whoSend, args->pseudo);
					strcat(whoSend, ") ");
					strcat(whoSend, partie3);

					if((write(vector_int_get(list_client, dest), whoSend, strlen(whoSend) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, dest));
						
						strcpy(mesg_erreur, "Impossible de joindre le client en message prive.");

						if((write(args->sock, mesg_erreur, strlen(mesg_erreur) + 1)) < 0){
							printf("erreur : impossible d'envoyer le message au client.\n");
							insert_message_unsend(mesg_erreur, args->sock, 3);
						}
					}
				}
				else{
					strcpy(mesg_erreur, "Le client n'existe pas.");

					if((write(args->sock, mesg_erreur, strlen(mesg_erreur) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client.\n");
						insert_message_unsend(mesg_erreur, args->sock, 3);
					}
				}
			}
			else if(strlen(partie1) == strlen(L_CMD) && strcmp(partie1, L_CMD) == 0){ // COMMANDE l (liste les pseudos des clients connectes)
				char *msg_list_pseudo;

				msg_list_pseudo = join_strings(list_pseudo, vector_char_size(list_pseudo));

				// TODO Checker si le nombre de caractères du pseudo dépasse les 255 caractères.
				
				if((write(args->sock, msg_list_pseudo, strlen(msg_list_pseudo) + 1)) < 0){
					printf("erreur : impossible d'envoyer le message au client.\n");
					insert_message_unsend(msg_list_pseudo, args->sock, 3);
				}

				free(msg_list_pseudo);
			}
			else if(strlen(partie1) == strlen(N_CMD) && strcmp(partie1, N_CMD) == 0){ // COMMANDE n (changement de pseudo)
				char whoChange[TAILLE_MAX_MESSAGE];

				if(vector_char_search(list_pseudo,partie2) == -1 && strlen(partie2) < TAILLE_MAX_PSEUDO){
					strcpy(whoChange, "* ");
					strcat(whoChange, args->pseudo);
					strcat(whoChange, " s'est renommé en ");
					strcat(whoChange, partie2);
					strcat(whoChange, " *");

					strcpy(args->pseudo, partie2);

					for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
					
						if((write(vector_int_get(list_client, i), whoChange, strlen(whoChange) + 1)) < 0){
							printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
							insert_message_unsend(whoChange, vector_int_get(list_client, i), 3);
						}
					}
				}
				else{
					strcpy(mesg_erreur, "Le pseudo existe déjà ou dépasse les 49 caractères autorisés.");

					if((write(args->sock, mesg_erreur, strlen(mesg_erreur) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client.\n");
						insert_message_unsend(mesg_erreur, args->sock, 3);
					}
				}
			}
			else if(strlen(partie1) == strlen(H_CMD) && strcmp(partie1, H_CMD) == 0){ // COMMANDE h (help)
				
				char helpMsg[TAILLE_MAX_MESSAGE];
				strcpy(helpMsg, "Commandes disponibles : \n");
				strcat(helpMsg, "  /q : quitter\n");
				strcat(helpMsg, "  /w <pseudo> : message privé\n");
				strcat(helpMsg, "  /l : liste utilisateurs\n");
				strcat(helpMsg, "  /n <nouveau nom> : changer de pseudo\n");
				strcat(helpMsg, "  /h : help\n");

				if((write(args->sock, helpMsg, strlen(helpMsg) + 1)) < 0){
					printf("erreur : impossible d'envoyer le message au client.\n");
					insert_message_unsend(helpMsg, args->sock, 3);
				}

			}
			else{
				strcpy(mesg_erreur, "Cette commande n'existe pas. Tapez /h pour obtenir la liste des commandes disponibles.");

				if((write(args->sock, mesg_erreur, strlen(mesg_erreur) + 1)) < 0){
					printf("erreur : impossible d'envoyer le message au client.\n");
					insert_message_unsend(mesg_erreur, args->sock, 3);
				}
			}
		}
		else{
			/* envoi du message a tous les autres utilisateurs */
			for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
				
				if((write(vector_int_get(list_client, i), buffer, strlen(buffer) + 1)) < 0){
					printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
					insert_message_unsend(buffer, vector_int_get(list_client, i), 3);
				}
			}
		}

		/* Clean des buffer */
		memset(buffer,0,sizeof(buffer));
		memset(tmp_buffer,0,sizeof(tmp_buffer));
		memset(partie1,0,sizeof(partie1));
		memset(partie2,0,sizeof(partie2));
		memset(partie3,0,sizeof(partie3));

		/* on libère la mémoire */
		free(pch);
	}

	/* Si le serveur ne reçoit plus de message d'un client alors celui-ci est considéré comme déconnecté */
	printf("Déconnexion du client %s (num : %d).\n", args->pseudo, args->sock);

	close(args->sock);

	index_client = vector_int_search(list_client, args->sock); // on calcul l'emplacement du client pour le supprimer.
	vector_int_delete(list_client, index_client);
	vector_char_delete(list_pseudo, index_client);

	free(args->pseudo);
	free(args);

	pthread_exit(NULL);
}

void *renvoi_message_unsend(void *args){

	/* tentative de renvoi des messages non envoyés */
	for(;;){
		for(int i = 0 ; i < vector_char_size(list_unsend_msg) ; ++i){
			char *msg = vector_char_get(list_unsend_msg, i);
			int msg_sock = vector_int_get(list_unsend_sock, i);
			int msg_cpt = vector_int_get(list_unsend_cpt, i);

			if((write(msg_sock, msg, strlen(msg) + 1)) < 0){
				vector_int_set(list_unsend_cpt, i, msg_cpt - 1);

				if(vector_int_get(list_unsend_cpt, i) == 0){
					vector_char_delete(list_unsend_msg, i);
					vector_int_delete(list_unsend_sock, i);
					vector_int_delete(list_unsend_cpt, i);
					free(msg);
				}
			}
			else{
				vector_char_delete(list_unsend_msg, i);
				vector_int_delete(list_unsend_sock, i);
				vector_int_delete(list_unsend_cpt, i);
				free(msg);
			}
		}
	}
}

void insert_message_unsend(char *message, int socket, int cpt){
	char *cpy_message;

	strcpy(cpy_message, message);

	vector_char_add(list_unsend_msg, cpy_message);
	vector_int_add(list_unsend_sock, socket);
	vector_int_add(list_unsend_cpt, cpt);
}

char* join_strings(vector_char *strings, int count){
	char* str = NULL;
	size_t total_length = 0;
	size_t length = 0;
	int i = 0;

	for(i = 0 ; i < count ; i++){
		total_length += strlen(vector_char_get(strings, i));
		
		if(vector_char_get(strings, i)[strlen(vector_char_get(strings, i))-1] != '\n')
			++total_length; 
	}

	++total_length;     

	str = (char*)malloc(total_length);
	str[0] = '\0';


	for(i = 0 ; i < count ; i++){
		strcat(str, vector_char_get(strings, i));
		length = strlen(str);

		if(str[length-1] != '\n'){
			str[length] = '\n';
			str[length+1] = '\0';
		}
	}

	return str;
}

int main(int argc, char **argv) {
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
	hostent *ptr_hote; 

	/* les infos recuperees sur le service de la machine */
	servent *ptr_service; 

	/* nom de la machine locale */
	char machine[TAILLE_MAX_NOM+1]; 

	/* recuperation du nom de la machine */
	gethostname(machine,TAILLE_MAX_NOM);
	
	/* thread pour chaque connexion cliente */
	pthread_t thread;

	/* pseudo du client */
	char pseudo[TAILLE_MAX_PSEUDO];
	int longueur_pseudo;

	/* création des vectors */
	list_client = vector_int_create();
	list_pseudo = vector_char_create();
	list_unsend_msg = vector_char_create();
	list_unsend_sock = vector_int_create();
	list_unsend_cpt = vector_int_create();
	
	/* recuperation de la structure d'adresse en utilisant le nom */
	if ((ptr_hote = gethostbyname("localhost")) == NULL) { // TODO changer en "machine". Temporaire -> problème avec le pc de Sitraka
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

	/* lancement du thread de renvoi de message en cas d'erreur */
	if(pthread_create(&thread, NULL, renvoi_message_unsend, NULL) == -1) {
		perror("erreur : impossible de créer le thread de gestion de message non-envoye.");
		exit(1);
    }

	/* attente des connexions et traitement des donnees recues */
	for(;;) {
		longueur_adresse_courante = sizeof(adresse_client_courant);

		/* clean du pseudo */
		memset(pseudo,0,sizeof(pseudo));

		/* adresse_client_courant sera renseignée par accept via les infos du connect */
		if ((nouv_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&adresse_client_courant), &longueur_adresse_courante)) >= 0) {
			
			/* lecture du pseudo */
			if((longueur_pseudo = read(nouv_socket_descriptor, pseudo, sizeof(pseudo))) > 0) {
				
				if(vector_char_search(list_pseudo,pseudo) == -1){
					arg_thread * params = malloc(sizeof(arg_thread));
					params->sock = nouv_socket_descriptor;
					params->pseudo = malloc(sizeof(char) * TAILLE_MAX_PSEUDO);
					strcpy(params->pseudo, pseudo);

					vector_int_add(list_client, params->sock);
					vector_char_add(list_pseudo, params->pseudo);

					if(pthread_create(&thread, NULL, connection, (void *) params) == -1) {
						printf("erreur : impossible de créer le thread pour le client : %d.\n", nouv_socket_descriptor);
				    }
				}
				else{
					char mesg_erreur[] = "Le pseudo existe deja.";

					if((write(nouv_socket_descriptor, mesg_erreur, strlen(mesg_erreur) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client : %d.\n", nouv_socket_descriptor);
					}

					close(nouv_socket_descriptor);
				}
			}
			else{
				printf("erreur : pas de pseudo renseigné.\n");
				
				char mesg_erreur[] = "Pas de pseudo renseigne.";

				if((write(nouv_socket_descriptor, mesg_erreur, strlen(mesg_erreur) + 1)) < 0){
					printf("erreur : impossible d'envoyer le message au client : %d.\n", nouv_socket_descriptor);
				}

				close(nouv_socket_descriptor);
			}
		}
		else{
			printf("erreur : impossible d'accepter la connexion avec le client : %d.\n", socket_descriptor);
		}
	} 

	exit(0);
}
