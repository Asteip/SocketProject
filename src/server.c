#include "server.h"

/* vector qui va contenir tous les clients */
vector_int *list_client = NULL;

/* vector qui va contenir tous les pseudos */
vector_char *list_pseudo = NULL;

void *connection(void *pArgs){
	arg_thread *args = pArgs;

	char buffer[TAILLE_MAX_MESSAGE];
	char mesg_erreur[50];

	int longueur = 0;
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

	/* on previent les autres utilisateur de la connexion d'un nouveau client */
	char whoConnect[80];

	strcpy(whoConnect, "Connexion du client : ");
	strcat(whoConnect, args->pseudo);

	for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
					
		if((write(vector_int_get(list_client, i), whoConnect, strlen(whoConnect) + 1)) < 0)
			printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
			// TODO -> gestion d'erreur.
	}

	while((longueur = read(args->sock, buffer, sizeof(buffer))) > 0 && est_connecte == 1){

		/* pretraitements */

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
		
		// Si le début de la ligne commence par "/" on regarde si c'est une commande existante
		if(buffer[0] == '/'){
			
			if(strlen(partie1) == strlen(Q_CMD) && strstr(partie1, Q_CMD) != NULL){ // COMMANDE q (quitter le serveur)
				char whoQuit[80];

				strcpy(whoQuit, "Déconnexion du client : ");
				strcat(whoQuit, args->pseudo);

				for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
					
					if((write(vector_int_get(list_client, i), whoQuit, strlen(whoQuit) + 1)) < 0)
						printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
						// TODO -> gestion d'erreur.
				}

				est_connecte = 0;
			}
			else if(strlen(partie1) == strlen(W_CMD) && strstr(partie1, W_CMD) != NULL){ // COMMANDE w (message privé)
				int dest = vector_char_search(list_pseudo, partie2);

				strcpy(mesg_erreur, "Le client n'existe pas.");

				if(dest != -1){
					if((write(vector_int_get(list_client, dest), partie3, strlen(partie3) + 1)) < 0)
						printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, dest));
						// TODO -> gestion d'erreur.
				}
				else{
					if((write(args->sock, mesg_erreur, strlen(mesg_erreur) + 1)) < 0)
						printf("erreur : impossible d'envoyer le message au client.\n");
						// TODO -> gestion d'erreur.
				}
			}
			else if(strlen(partie1) == strlen(L_CMD) && strstr(partie1, L_CMD) != NULL){ // COMMANDE l (liste les pseudo des clients connectes)
				char *msg_list_pseudo;

				msg_list_pseudo = join_strings(list_pseudo, vector_char_size(list_pseudo));

				// TODO Checker si le nombre de caractères du pseudo dépasse les 255 caractères.
				
				if((write(args->sock, msg_list_pseudo, strlen(msg_list_pseudo) + 1)) < 0)
					printf("erreur : impossible d'envoyer le message au client.\n");
					// TODO -> gestion d'erreur.

				free(msg_list_pseudo);
			}
			else if(strlen(partie1) == strlen(F_CMD) && strstr(partie1, F_CMD) != NULL){ // COMMANDE f (envoi de fichier)
				
				// TODO A implémenter -> envoi de fichier.

			}
			else{
				strcpy(mesg_erreur, "Cette commande n'existe pas.");

				if((write(args->sock, mesg_erreur, strlen(mesg_erreur) + 1)) < 0)
					printf("erreur : impossible d'envoyer le message au client.\n");
					// TODO -> gestion d'erreur.
			}
		}
		else{
			/* envoi du message a tous les autres utilisateurs */
			for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
				
				if((write(vector_int_get(list_client, i), buffer, strlen(buffer) + 1)) < 0)
					printf("erreur : impossible d'envoyer le message au client : %d.\n", vector_int_get(list_client, i));
					// TODO -> gestion d'erreur.
			}
		}

		// Clean des buffer
		memset(buffer,0,sizeof(buffer));
		memset(tmp_buffer,0,sizeof(tmp_buffer));
		memset(partie1,0,sizeof(partie1));
		memset(partie2,0,sizeof(partie2));
		memset(partie3,0,sizeof(partie3));

		free(pch);
	}

	/* Si le serveur ne reçoit plus de message d'un client alors celui-ci est considéré comme déconnecté */
	printf("Déconnexion du client %s (num : %d).\n", args->pseudo, args->sock);

	close(args->sock);

	index_client = vector_int_search(list_client, args->sock); // on calcul l'emplacement du client.
	vector_int_delete(list_client, index_client);
	vector_char_delete(list_pseudo, index_client);

	free(args->pseudo);
	free(args);

	pthread_exit(NULL);
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
	hostent* ptr_hote; 

	/* les infos recuperees sur le service de la machine */
	servent* ptr_service; 

	/* nom de la machine locale */
	char machine[TAILLE_MAX_NOM+1]; 

	/* recuperation du nom de la machine */
	gethostname(machine,TAILLE_MAX_NOM);
	
	/* thread pour chaque connexion cliente */
	pthread_t thread;

	/* pseudo du client */
	char pseudo[TAILLE_MAX_PSEUDO];
	int longueur_pseudo;

	list_client = vector_int_create();
	list_pseudo = vector_char_create();
	
	/* recuperation de la structure d'adresse en utilisant le nom */
	if ((ptr_hote = gethostbyname("localhost")) == NULL) { // Temporaire -> problème avec le pc de Sitraka
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

		// clean du pseudo
		memset(pseudo,0,sizeof(pseudo));

		/* adresse_client_courant sera renseignée par accept via les infos du connect */
		if ((nouv_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&adresse_client_courant), &longueur_adresse_courante)) >= 0) {
			
			// lecture du pseudo
			if((longueur_pseudo = read(nouv_socket_descriptor, pseudo, sizeof(pseudo))) > 0) {
				arg_thread * params = malloc(sizeof(arg_thread));
				params->sock = nouv_socket_descriptor;
				params->pseudo = malloc(sizeof(char) * TAILLE_MAX_PSEUDO);
				strcpy(params->pseudo, pseudo);

				vector_int_add(list_client, params->sock);
				vector_char_add(list_pseudo, params->pseudo);

				// TODO -> verifier si le pseudo n'est pas déjà dans la liste.

				if(pthread_create(&thread, NULL, connection, (void *) params) == -1) {
					printf("erreur : impossible de créer le thread pour le client : %d.\n", nouv_socket_descriptor);
					// TODO -> gestion d'erreur.
			    }
			}
			else{
				printf("erreur : pas de pseudo renseigné.\n");
				// TODO -> gestion d'erreur.
			}
		}
		else{
			printf("erreur : impossible d'accepter la connexion avec le client : %d.\n", socket_descriptor);
		}
	} 

	exit(0);
}
