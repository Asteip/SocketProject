#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

static void vector_resize(vector *v, int max_size){
    int *items = realloc(v->items, sizeof(int) * max_size);
    if (items){
        v->items = items;
        v->max_size = max_size;
    }
}

vector * vector_create(){
    vector * v = malloc(sizeof(vector));

    v->max_size = VECTOR_INIT_MAX_SIZE;
    v->size = 0;
    v->items = malloc(sizeof(int) * v->max_size);

    return v;
}

int vector_size(vector *v){
    return v->size;
}

void vector_add(vector *v, int item){
    int threshold = (int) v->max_size * 0.7;

    if (v->size >= threshold) 
        vector_resize(v, v->max_size * 2);
    
    v->items[v->size] = item;
    v->size++;
}

void vector_set(vector *v, int index, int item){
    if (index >= 0 && index < v->size) 
        v->items[index] = item;
}

int vector_get(vector *v, int index){
    int ret = 0;

    if (index >= 0 && index < v->size)
        ret = v->items[index];
    else
        printf("ERROR : there is no items at this index.\n");

    return ret;
}

void vector_delete(vector *v, int index){
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
        vector_resize(v, v->max_size / 2);
}

void vector_free(vector *v){
    free(v->items);
}