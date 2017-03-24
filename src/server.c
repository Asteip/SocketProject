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
	int index_client = 0;

	/* on clean le buffer avant de commencer */
	memset(buffer,0,sizeof(buffer));

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
	while((long_buffer = read(args->sock, buffer, sizeof(buffer))) > 0){
		char **splitMessage = NULL;

		/* prétraitements du message reçu : on sépare la partie commande et option de la partie message */
		splitMessage = traitementMessage(buffer);
		
		/* Si le début de la ligne commence par "/" on regarde si c'est une commande existante */
		if(buffer[0] == '/'){
			
			if(strlen(splitMessage[0]) == strlen(W_CMD) && strcmp(splitMessage[0], W_CMD) == 0){ // COMMANDE w (message privé)
				int dest = vector_char_search(list_pseudo, splitMessage[1]);
				char whoSend[TAILLE_MAX_MESSAGE];

				if(dest != -1){
					strcpy(whoSend, "[");
					strcat(whoSend, args->pseudo);
					strcat(whoSend, " (w)]: ");
					strcat(whoSend, splitMessage[2]);

					if((write(vector_int_get(list_client, dest), whoSend, strlen(whoSend) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, dest));
						
						strcpy(mesg_erreur, "Impossible de joindre le client en message prive.");

						if((write(args->sock, mesg_erreur, strlen(mesg_erreur) + 1)) < 0){
							printf("erreur : impossible d'envoyer le message au client.\n");
							insert_message_unsend(mesg_erreur, args->sock, 3);
						}
					}

					if((write(args->sock, whoSend, strlen(whoSend) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client.\n");
						insert_message_unsend(whoSend, args->sock, 3);
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
			else if(strlen(splitMessage[0]) == strlen(L_CMD) && strcmp(splitMessage[0], L_CMD) == 0){ // COMMANDE l (liste les pseudos des clients connectes)

				for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
					if((write(args->sock, vector_char_get(list_pseudo,i), strlen(vector_char_get(list_pseudo,i)) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client.\n");
						insert_message_unsend(vector_char_get(list_pseudo,i), args->sock, 3);
					}

					sleep(1);
				}				
			}
			else if(strlen(splitMessage[0]) == strlen(N_CMD) && strcmp(splitMessage[0], N_CMD) == 0){ // COMMANDE n (changement de pseudo)
				char whoChange[TAILLE_MAX_MESSAGE];
				char modifNom[TAILLE_MAX_MESSAGE];

				if(vector_char_search(list_pseudo,splitMessage[1]) == -1 && strlen(splitMessage[1]) < TAILLE_MAX_PSEUDO && strlen(splitMessage[1]) > 0){
					strcpy(whoChange, "* ");
					strcat(whoChange, args->pseudo);
					strcat(whoChange, " s'est renommé en ");
					strcat(whoChange, splitMessage[1]);
					strcat(whoChange, " *");

					strcpy(args->pseudo, splitMessage[1]);

					/* on renvoi au client la commande /n suivi de son pseudo si ce dernier a été accepté */
					strcpy(modifNom, N_CMD);
					strcat(modifNom, " ");
					strcat(modifNom, splitMessage[1]);

					/* on autorise l'utilisateur à changer de nom si celui-ci respecte les conditions */
					if((write(args->sock, modifNom, strlen(modifNom) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client.\n");
						insert_message_unsend(modifNom, args->sock, 3);
					}

					sleep(1);

					for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
					
						if((write(vector_int_get(list_client, i), whoChange, strlen(whoChange) + 1)) < 0){
							printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
							insert_message_unsend(whoChange, vector_int_get(list_client, i), 3);
						}
					}
				}
				else{
					strcpy(mesg_erreur, "Le pseudo existe déjà ou dépasse les 19 caractères autorisés.");

					if((write(args->sock, mesg_erreur, strlen(mesg_erreur) + 1)) < 0){
						printf("erreur : impossible d'envoyer le message au client.\n");
						insert_message_unsend(mesg_erreur, args->sock, 3);
					}
				}
			}
			else{
				strcpy(mesg_erreur, "Cette commande n'existe pas (/h pour la liste des commandes).");

				if((write(args->sock, mesg_erreur, strlen(mesg_erreur) + 1)) < 0){
					printf("erreur : impossible d'envoyer le message au client.\n");
					insert_message_unsend(mesg_erreur, args->sock, 3);
				}
			}
		}
		else{
			char whoSend[TAILLE_MAX_MESSAGE];

			strcpy(whoSend, "[");
			strcat(whoSend, args->pseudo);
			strcat(whoSend, "]: ");
			strcat(whoSend, buffer);

			/* envoi du message a tous les autres utilisateurs */
			for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
				
				if((write(vector_int_get(list_client, i), whoSend, strlen(whoSend) + 1)) < 0){
					printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
					insert_message_unsend(whoSend, vector_int_get(list_client, i), 3);
				}
			}
		}

		/* Clean du buffer */
		memset(buffer,0,sizeof(buffer));

		/* libération de la mémoire */
		for(int i = 0 ; i < 3 ; ++i)
			free(splitMessage[i]);

		free(splitMessage);
	}

	/* Si le serveur ne reçoit plus de message d'un client alors celui-ci est considéré comme déconnecté */
	printf("Déconnexion du client %s (num : %d).\n", args->pseudo, args->sock);

	/* On prévient les autres utilisateurs de sa déconnexion */
	char whoQuit[TAILLE_MAX_MESSAGE];

	strcpy(whoQuit, "* ");
	strcat(whoQuit, args->pseudo);
	strcat(whoQuit, " s'est déconnecté. *");

	for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
		if(vector_int_get(list_client, i) != args->sock){
			if((write(vector_int_get(list_client, i), whoQuit, strlen(whoQuit) + 1)) < 0){
				printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
				insert_message_unsend(whoQuit, vector_int_get(list_client, i), 3);
			}
		}
	}

	/* fermeture du socket et suppression du pseudo et du socket dans les listes */
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
	while(1){
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

char **traitementMessage(char *message){
	char **result = NULL;
	char tmp_message[TAILLE_MAX_MESSAGE];
	char *pch;
	int cpt = 0;

	result = malloc(sizeof(char*) * 3);
	
	for (int i = 0 ; i < 3 ; ++i) {
	    result[i] = (char *) malloc(TAILLE_MAX_MESSAGE);
	    memset(result[i],0,sizeof(result[i]));
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
	if ((ptr_hote = gethostbyname(machine)) == NULL) { // TODO changer en "machine". Temporaire -> problème avec le pc de Sitraka
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
	while(1) {
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

	return 0;
}
