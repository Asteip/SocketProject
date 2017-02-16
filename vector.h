#ifndef VECTOR_H
#define VECTOR_H

#define VECTOR_INIT_MAX_SIZE 5

/*!
 * \brief Structure de type vector pour un tableau d'entier
 * \details Permet de manipuler dynamiquement la taille d'un tableau d'entiers
 * d'entier
 * \ref http://eddmann.com/posts/implementing-a-dynamic-vector-array-in-c
 */

typedef struct vector {
    int * items;
    int max_size;
    int size;
} vector;

/*!
 * \brief Redimensionne la taille du vector
 * \param vector* Le vector à redimensionner
 * \param int a nouvelle taille du vector
 */
static void vector_resize(vector *, int);

/*!
 * \brief Créé un vector
 * \return vector* Le vector créé
 */
vector * vector_create();

/*!
 * \brief Retourne le nombre d'éléments d'un vector
 * \param vector* Le vector à analyser
 * \return La taille du vector
 */
int vector_size(vector *);

/*!
 * \brief Ajoute un élément au vector
 * \param vector* Le vector dans lequel l'élément doit être ajouté
 * \param int L'entier à ajouter au vector
 */
void vector_add(vector *, int);

/*!
 * \brief Modifie l'élément à un index donné
 * \param vector* Le vector dans lequel l'élément doit être modifié
 * \param int L'index de l'élément à modifier
 * \param int Le nouvel entier à placer à l'index donné
 */
void vector_set(vector *, int, int);

/*!
 * \brief Renvoi l'élément à l'index donné
 * \pre L'index doit être compris entre 0 et la taille du vector
 * \param vector* Le vector à parcourir
 * \param int L'index de l'élément à récupérer
 * \return L'élément à la position donné, 0 si l'élément n'existe pas
 */
int vector_get(vector *, int);

/*!
 * \brief Supprime l'élément à la position donnée
 * \pre L'index doit être compris entre 0 et la taille du vector
 * \param vector* Le vector dans lequel l'élément doit être supprimé
 * \param int l'index de l'élément à supprimer
 */
void vector_delete(vector *, int);

/*!
 * \brief Libère la mémoire du tableau d'entiers
 * \param vector* Le vector à détruire
 */
void vector_free(vector *);

#endif