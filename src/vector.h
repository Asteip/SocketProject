#ifndef VECTOR_H
#define VECTOR_H

#define VECTOR_INIT_MAX_SIZE 5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************
 INT VECTOR
*************************************/

/*!
 * \brief Structure de type vector pour un tableau d'entier
 * \details Permet de manipuler dynamiquement la taille d'un tableau d'entiers
 */

typedef struct vector_int {
    int * items;
    int max_size;
    int size;
} vector_int;

/*!
 * \brief Redimensionne la taille du vector
 * \param vector_int* Le vector à redimensionner
 * \param int a nouvelle taille du vector
 */
static void vector_int_resize(vector_int *, int);

/*!
 * \brief Créé un vector
 * \return vector_int* Le vector créé
 */
vector_int * vector_int_create();

/*!
 * \brief Retourne le nombre d'éléments d'un vector
 * \param vector_int* Le vector à analyser
 * \return La taille du vector
 */
int vector_int_size(vector_int *);

/*!
 * \brief Ajoute un élément au vector
 * \param vector_int* Le vector dans lequel l'élément doit être ajouté
 * \param int L'entier à ajouter au vector
 */
void vector_int_add(vector_int *, int);

/*!
 * \brief Modifie l'élément à un index donné
 * \param vector_int* Le vector dans lequel l'élément doit être modifié
 * \param int L'index de l'élément à modifier
 * \param int Le nouvel entier à placer à l'index donné
 */
void vector_int_set(vector_int *, int, int);

/*!
 * \brief Renvoi l'élément à l'index donné
 * \pre L'index doit être compris entre 0 et la taille du vector
 * \param vector_int* Le vector à parcourir
 * \param int L'index de l'élément à récupérer
 * \return L'élément à la position donné, 0 si l'élément n'existe pas
 */
int vector_int_get(vector_int *, int);

/*!
 * \brief Supprime l'élément à la position donnée
 * \pre L'index doit être compris entre 0 et la taille du vector
 * \param vector_int* Le vector dans lequel l'élément doit être supprimé
 * \param int l'index de l'élément à supprimer
 */
void vector_int_delete(vector_int *, int);

/*!
 * \brief Recherche l'index de l'élément passé en paramètre
 * \param int l'élément à rechercher dans le tableau
 */
int vector_int_search(vector_int *, int);

/*!
 * \brief Libère la mémoire du tableau d'entiers
 * \param vector* Le vector à détruire
 */
void vector_int_free(vector_int *);

/************************************
 CHAR VECTOR
*************************************/

/*!
 * \brief Structure de type vector pour un tableau de char
 * \details Permet de manipuler dynamiquement la taille d'un tableau de char
 */

typedef struct vector_char {
    char **items;
    int max_size;
    int size;
} vector_char;

/*!
 * \brief Redimensionne la taille du vector
 * \param vector_char* Le vector à redimensionner
 * \param int a nouvelle taille du vector
 */
static void vector_char_resize(vector_char *, int);

/*!
 * \brief Créé un vector
 * \return vector_char* Le vector créé
 */
vector_char * vector_char_create();

/*!
 * \brief Retourne le nombre d'éléments d'un vector
 * \param vector_char* Le vector à analyser
 * \return La taille du vector
 */
int vector_char_size(vector_char *);

/*!
 * \brief Ajoute un élément au vector
 * \param vector_char* Le vector dans lequel l'élément doit être ajouté
 * \param char Le char à ajouter au vector
 */
void vector_char_add(vector_char *, char*);

/*!
 * \brief Modifie l'élément à un index donné
 * \param vector_char* Le vector dans lequel l'élément doit être modifié
 * \param int L'index de l'élément à modifier
 * \param char Le nouveau char à placer à l'index donné
 */
void vector_char_set(vector_char *, int, char*);

/*!
 * \brief Renvoi l'élément à l'index donné
 * \pre L'index doit être compris entre 0 et la taille du vector
 * \param vector_char* Le vector à parcourir
 * \param int L'index de l'élément à récupérer
 * \return L'élément à la position donné, 0 si l'élément n'existe pas
 */
char* vector_char_get(vector_char *, int);

/*!
 * \brief Supprime l'élément à la position donnée
 * \pre L'index doit être compris entre 0 et la taille du vector
 * \param vector_char* Le vector dans lequel l'élément doit être supprimé
 * \param int l'index de l'élément à supprimer
 */
void vector_char_delete(vector_char *, int);

/*!
 * \brief Recherche l'index de l'élément passé en paramètre
 * \param char l'élément à rechercher dans le tableau
 */
int vector_char_search(vector_char *, char*);

/*!
 * \brief Libère la mémoire du tableau d'entiers
 * \param vector* Le vector à détruire
 */
void vector_char_free(vector_char *);

#endif