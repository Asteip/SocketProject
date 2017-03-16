#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "src/vector.c"

const int N = 9;

void init_msgs(vector_char *msgs){
	for(int i=0; i<N; ++i){
		vector_char_add(msgs,"");
	}
}



void insert_msg_env(vector_char *msgs_env, vector_char *msgs_rec ,char* msg){
	for(int i=vector_char_size(msgs_env) -1 ; i>0; --i){
		vector_char_set(msgs_env,i,vector_char_get(msgs_env,i-1));
		vector_char_set(msgs_rec,i,vector_char_get(msgs_rec,i-1));
	}
	vector_char_set(msgs_env,0,msg);
	vector_char_set(msgs_rec,0,"\0");
}

void insert_msg_rec(vector_char *msgs_rec, vector_char *msgs_env ,char* msg){
	for(int i=vector_char_size(msgs_rec) -1 ; i>0; --i){
		vector_char_set(msgs_rec,i,vector_char_get(msgs_rec,i-1));
		vector_char_set(msgs_env,i,vector_char_get(msgs_env,i-1));
	}
	vector_char_set(msgs_rec,0,msg);
	vector_char_set(msgs_env,0,"\0");
}


int main(void){
	WINDOW *haut, *bas;
	char *msg= "Mon_pseudo";
	int taille= strlen(msg);
	char *str = malloc (256 * sizeof (char));

	vector_char *msgs_env = vector_char_create() ;
	vector_char *msgs_rec = vector_char_create() ;
	
	init_msgs(msgs_env);
	init_msgs(msgs_rec);
	int tour =0;
	initscr();

	while(1){
		clear();   
        attron(A_STANDOUT);
    	mvprintw(0, (COLS / 2) - (taille / 2), msg);
		attroff(A_STANDOUT);
		haut= subwin(stdscr, 2*LINES /3  -1, COLS, 1, 0);       
    	bas= subwin(stdscr, LINES / 3, COLS, 2*LINES / 3, 0); 
	    box(haut, ACS_VLINE, ACS_HLINE);
	    box(bas, ACS_VLINE, ACS_HLINE);
	    
	    mvwprintw(haut, 1, 1, "Messages :");
	    for(int i=0; i<N ; ++i){
	    	mvwprintw(haut, (2*LINES)/3 -3 -i,COLS - strlen(vector_char_get(msgs_env,i)) - 2,vector_char_get(msgs_env,i));
	 		mvwprintw(haut, (2*LINES)/3 -3 -i,2,vector_char_get(msgs_rec,i));
 		
	  	}
		

	    
	    mvwprintw(bas, 1, 1, "Répondre :");
		move(2*LINES/3 + 2, 1);
	 
	    wrefresh(haut);
		wrefresh(bas); 

		char *str = malloc (256 * sizeof (char));
		strcpy(str, "\0");
		getstr(str);
		if(strcmp(str, "\0")!=0) 
			insert_msg_env(msgs_env, msgs_rec, str);
		if(tour == 2)
			insert_msg_rec(msgs_rec, msgs_env, "oui sava");
		++tour;
		}                
    endwin();              
	free(haut);
	free(bas);




	
	return 0;
}