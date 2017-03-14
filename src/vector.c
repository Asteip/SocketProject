#include "vector.h"

/************************************
 INT VECTOR
*************************************/

static void vector_int_resize(vector_int *v, int max_size){
    int *items = realloc(v->items, sizeof(int) * max_size);
    if (items){
        v->items = items;
        v->max_size = max_size;
    }
}

vector_int * vector_int_create(){
    vector_int * v = malloc(sizeof(vector_int));

    v->max_size = VECTOR_INIT_MAX_SIZE;
    v->size = 0;
    v->items = malloc(sizeof(int) * v->max_size);

    return v;
}

int vector_int_size(vector_int *v){
    return v->size;
}

void vector_int_add(vector_int *v, int item){
    int threshold = (int) v->max_size * 0.7;

    if (v->size >= threshold) 
        vector_int_resize(v, v->max_size * 2);
    
    v->items[v->size] = item;
    v->size++;
}

void vector_int_set(vector_int *v, int index, int item){
    if (index >= 0 && index < v->size) 
        v->items[index] = item;
}

int vector_int_get(vector_int *v, int index){
    int result = 0;

    if (index >= 0 && index < v->size)
        result = v->items[index];
    else
        printf("ERROR : there is no items at this index.\n");

    return result;
}

void vector_int_delete(vector_int *v, int index){
    if (index < 0 || index >= v->size){
        printf("ERROR : there is no items at this index.\n");
        return;
    }

    for (int i = index ; i < v->size - 1 ; ++i) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = 0;
    }

    v->size--;

    if (v->size > 0 && v->size == v->max_size / 4)
        vector_int_resize(v, v->max_size / 2);
}

int vector_int_search(vector_int *v, int item){
    int i = 0;
    int found = 0;
    int result = -1;

    while(i < v->size && found == 0){
        if(v->items[i] == item){
            found = 1;
            result = i;
        }

        i++;
    }

    return result;
}

void vector_int_free(vector_int *v){
    free(v->items);
}

/************************************
 CHAR VECTOR
*************************************/

static void vector_char_resize(vector_char *v, int max_size){
    char **items = realloc(v->items, sizeof(char*) * max_size);
    if (items){
        v->items = items;
        v->max_size = max_size;
    }
}

vector_char * vector_char_create(){
    vector_char * v = malloc(sizeof(vector_char));

    v->max_size = VECTOR_INIT_MAX_SIZE;
    v->size = 0;
    v->items = malloc(sizeof(char*) * v->max_size);

    return v;
}

int vector_char_size(vector_char *v){
    return v->size;
}

void vector_char_add(vector_char *v, char *item){
    int threshold = (int) v->max_size * 0.7;

    if (v->size >= threshold) 
        vector_char_resize(v, v->max_size * 2);
    
    v->items[v->size] = item;
    v->size++;
}

void vector_char_set(vector_char *v, int index, char *item){
    if (index >= 0 && index < v->size) 
        v->items[index] = item;
}

char * vector_char_get(vector_char *v, int index){
    char *result = NULL;

    if (index >= 0 && index < v->size)
        result = v->items[index];
    else
        printf("ERROR : there is no items at this index.\n");

    return result;
}

void vector_char_delete(vector_char *v, int index){
    if (index < 0 || index >= v->size){
        printf("ERROR : there is no items at this index.\n");
        return;
    }

    for (int i = index ; i < v->size - 1 ; ++i) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->size--;

    if (v->size > 0 && v->size == v->max_size / 4)
        vector_char_resize(v, v->max_size / 2);
}

int vector_char_search(vector_char *v, char *item){
    int i = 0;
    int found = 0;
    int result = -1;

    while(i < v->size && found == 0){
        if(v->items[i] == item){
            found = 1;
            result = i;
        }

        i++;
    }

    return result;
}

void vector_char_free(vector_char *v){
    for(int i = 0 ; i < v->max_size ; ++i){
        free(v->items[i]);
    }

    free(v->items);
}