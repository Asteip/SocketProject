#ifndef VECTOR_H
#define VECTOR_H

#define VECTOR_INIT_CAPACITY 4

/* IMPLEMENTATION OF VECTOR : http://eddmann.com/posts/implementing-a-dynamic-vector-array-in-c/ */

typedef struct vector {
    int * items;
    int capacity;
    int total;
} vector;

static void vector_resize(vector *, int);

void vector_init(vector *);
int vector_total(vector *);
void vector_add(vector *, int);
void vector_set(vector *, int, int);
int vector_get(vector *, int);
void vector_delete(vector *, int);
void vector_free(vector *);

#endif