#include "server.h"

/* vector qui va contenir tous les clients */
vector_int *list_client = NULL;

/* vector qui va contenir tous les pseudos */
vector_char *list_pseudo;

void *connection(void *pArgs){
	char buffer[TAILLE_MAX_MESSGE];
	int longueur = 0;
	int is_connected = 1;
	int index_deco = 0;
	arg_thread *args = pArgs; // Cast du pArgs qui est de type void*

	char quitCmd[] = "/quit";
	char wCmd[] = "/w";
	char fileCmd[] = "/file";
	char lCmd[] = "/l";

	memset(buffer,0,sizeof(buffer));

	printf("Connexion du client %s (num : %d)\n", args->pseudo, args->sock);

	while((longueur = read(args->sock, buffer, sizeof(buffer))) > 0 && is_connected == 1){

		//sleep(3);
		
		// Si le début de la ligne commence par "/" on regarde si c'est une commande
		if(buffer[0] == '/'){
			
			// COMMANDE quit (quitter le serveur)
			if(strstr(buffer, quitCmd) != NULL){
				char whoQuit[30];
				char bufPos[6];

				strcpy(whoQuit, "Déconnexion du client : ");
				sprintf(bufPos, "%d", args->sock);
				strcat(whoQuit, bufPos);

				for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
					write(vector_int_get(list_client, i), whoQuit, strlen(whoQuit) + strlen(bufPos) + 1);
				}

				is_connected = 0;
			}

			// COMMANDE w (message privé)
			if(strstr(buffer, wCmd) != NULL){
				// A implémenter -> fonction message privé.
			}

			// COMMANDE file (envoi de fichier)
			if(strstr(buffer, fileCmd) != NULL){
				// A implémenter -> envoi de fichier.
			}

			// COMMANDE list (list les pseudo des clients connectes)
			if(strstr(buffer, lCmd) != NULL){
				char *msg_list_pseudo = join_strings(list_pseudo, vector_char_size(list_pseudo));
				write(args->sock, msg_list_pseudo, strlen(msg_list_pseudo) + 1);
				free(msg_list_pseudo);
			}
		}
		else{
			/* envoi du message a tous les autres utilisateurs */
			for(int i = 0 ; i < vector_int_size(list_client) ; ++i){
				write(vector_int_get(list_client, i), buffer, strlen(buffer)+1);
			}
		}

		// Clean du buffer
		memset(buffer,0,sizeof(buffer));
	}

	// Si le serveur ne reçoit plus de message d'un client alors celui-ci est considéré comme déconnecté
	printf("Déconnexion du client %d\n", args->sock);

	close(args->sock);
	index_deco = vector_int_search(list_client, args->sock);
	vector_int_delete(list_client, index_deco);
	vector_char_delete(list_pseudo, index_deco);
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

		// clean du pseudo
		memset(pseudo,0,sizeof(pseudo));

		/* adresse_client_courant sera renseignée par accept via les infos du connect */
		if ((nouv_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&adresse_client_courant), &longueur_adresse_courante)) < 0) {
			perror("erreur : impossible d'accepter la connexion avec le client.");
		}
		else{
			// lecture du pseudo
			if((longueur_pseudo = read(nouv_socket_descriptor, pseudo, sizeof(pseudo))) > 0) {
				arg_thread * params = malloc(sizeof(arg_thread));
				params->sock = nouv_socket_descriptor;
				params->pseudo = malloc(sizeof(char) * TAILLE_MAX_PSEUDO);
				strcpy(params->pseudo, pseudo);

				vector_int_add(list_client, params->sock);
				vector_char_add(list_pseudo, params->pseudo);

				if(pthread_create(&thread, NULL, connection, (void *) params) == -1) {
					perror("erreur : impossible de créer le thread pour ce client");
			    }
			}
			else{
				perror("erreur : pas de pseudo.");
			}
		}
	} 

	exit(0);
}
