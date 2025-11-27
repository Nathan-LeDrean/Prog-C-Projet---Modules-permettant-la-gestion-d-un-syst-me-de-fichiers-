/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 2
 * Fichier : sf.c
 * Module de gestion d'un systèmes de fichiers (simulé)
 **/

#include <stdlib.h> 
#include <stdio.h>  
#include <string.h> 
#include <time.h>   
#include "inode.h"
#include "sf.h"
#include "bloc.h"

// Taille maximale du nom du SF (ou nom du disque)
#define TAILLE_NOM_DISQUE 24

// Définition du super-bloc
struct sSuperBloc
{
  // Le nom du disque (ou du système de fichiers) (avec le '\0')
  char nomDisque[TAILLE_NOM_DISQUE+1];
  // La date de dernière modification du système de fichiers
  time_t dateDerModif;
};

// Type représentant le super-bloc
typedef struct sSuperBloc *tSuperBloc;

// Définition de la liste chaînée des inodes
struct sListeInodes
{
  // Références vers le premier et le dernier élément de la liste chaînée
  struct sListeInodesElement *premier, *dernier;
  // Nombre d'inodes dans la liste
  int nbInodes;
};

// Définition d'un élement de la liste chaînée des inodes dans le SF
struct sListeInodesElement
{
  // L'inode proprement dit
  tInode inode;
  // L'élément suivant dans la liste chaînée
  struct sListeInodesElement *suivant;
};

// Définition d'un système de fichiers (simplifié)
struct sSF
{
  // Le super-bloc
  tSuperBloc superBloc;
  // La liste chaînée des inodes
  struct sListeInodes listeInodes;
};

/* V2
*  Crée et retourne un super-bloc.
* Fonction non publique (static)
* Entrée : le nom du disque (ou du SF)
* Sortie : le super-bloc, ou NULL en cas de problème
*/
static tSuperBloc CreerSuperBloc(char nomDisque[]) {
    tSuperBloc sb;
    time_t t;

    sb = (tSuperBloc)malloc(sizeof(struct sSuperBloc));
    
    if (sb == NULL) {
        fprintf(stderr, "CreerSuperBloc : probleme creation\n");
        return NULL;
    }

    for (int i = 0; i < TAILLE_NOM_DISQUE && nomDisque[i] != '\0'; i++){
        sb->nomDisque[i] = nomDisque[i];
    }
    sb->nomDisque[TAILLE_NOM_DISQUE] = '\0'; 

    t = time(NULL);
    sb->dateDerModif = t;

    return sb;
} 

/* V2
*  Détruit un super-bloc.
* Fonction non publique (static)
* Entrée : le super-bloc à détruire
* Sortie : aucune
*/
static void DetruireSuperBloc(tSuperBloc *pSuperBloc) {
    if (pSuperBloc != NULL && *pSuperBloc != NULL) {
        free(*pSuperBloc);
        *pSuperBloc = NULL;
    }
}

/* V2
*  Affiche le contenu d'un super-bloc.
* Fonction non publique (static)
* Entrée : le super-bloc à afficher
* Sortie : aucune
*/
static void AfficherSuperBloc(tSuperBloc superBloc) {
    if (superBloc != NULL) {
        printf("Sf de nom %s, super bloc :\n", superBloc->nomDisque);
        printf("taille bloc = %d, date der modif = %s", TAILLE_BLOC, ctime(&(superBloc->dateDerModif)));
    }
}

/* V2
 * Crée un nouveau système de fichiers.
 * Entrée : nom du disque à associer au système de fichiers créé
 * Retour : le système de fichiers créé, ou NULL en cas d'erreur
 */
tSF CreerSF (char nomDisque[]) {
    tSF sf = (tSF)malloc(sizeof(struct sSF));
    
    if (sf == NULL) {
        fprintf(stderr, "CreerSF: probleme creation\n");
        return NULL;
    }

    sf->superBloc = CreerSuperBloc(nomDisque);
    
    if (sf->superBloc == NULL) {
        free(sf);
        return NULL;
    }

    sf->listeInodes.premier = NULL;
    sf->listeInodes.dernier = NULL;
    sf->listeInodes.nbInodes = 0;

    return sf;
}

/* V2
 * Détruit un système de fichiers et libère la mémoire associée.
 * Entrée : le SF à détruire
 * Sortie : aucune
 */
void DetruireSF(tSF *pSF) {
    struct sListeInodesElement *courant;
    struct sListeInodesElement *suivant;

    if (pSF != NULL && *pSF != NULL) {
        DetruireSuperBloc(&((*pSF)->superBloc));

        courant = (*pSF)->listeInodes.premier;
        
        while (courant != NULL) {
            suivant = courant->suivant;
            DetruireInode(&(courant->inode));
            free(courant);
            courant = suivant;
        }
        free(*pSF);
        *pSF = NULL;
    }
}

/* V2
 * Affiche les informations relative à un système de fichiers i.e;
 * le contenu du super-bloc et celui des différents inodes du SF
 * Entrée : le SF à afficher
 * Sortie : aucune
 */
void AfficherSF (tSF sf) {
    struct sListeInodesElement *courant;
    if (sf == NULL) {
        return;
    }

    AfficherSuperBloc(sf->superBloc);
    printf("Inodes :\n");
    courant = sf->listeInodes.premier;

    while (courant != NULL) {
        printf("--------------------\n");
        AfficherInode(courant->inode);
        courant = courant->suivant;
    }
}

/* V2
 * Ecrit un fichier d'un seul bloc dans le système de fichiers.
 * Entrées : le système de fichiers, le nom du fichier (sur disque) et son type dans le SF (simulé)
 * Sortie : le nombre d'octets effectivement écrits, -1 en cas d'erreur.
 */
long Ecrire1BlocFichierSF(tSF sf, char nomFichier[], natureFichier type) {
    FILE *fichierReel;
    unsigned char buffer[TAILLE_BLOC];

    if (sf == NULL) {
        return -1;
    }

    fichierReel = fopen(nomFichier, "rb");
    
    if (fichierReel == NULL) {
        perror("Erreur ouverture fichier"); 
        return -1;
    }

    long octetsLus = fread(buffer, 1, TAILLE_BLOC, fichierReel);
    fclose(fichierReel);
    tInode nouvelInode = CreerInode(sf->listeInodes.nbInodes, type);
    
    if (nouvelInode == NULL) {
        return -1;
    }

    long octetsEcrits = EcrireDonneesInode1bloc(nouvelInode, buffer, octetsLus);
    struct sListeInodesElement *nouveauMaillon = (struct sListeInodesElement *)malloc(sizeof(struct sListeInodesElement));
    
    if (nouveauMaillon == NULL) {
        DetruireInode(&nouvelInode);
        return -1;
    }

    nouveauMaillon->inode = nouvelInode;
    nouveauMaillon->suivant = NULL;

    if (sf->listeInodes.premier == NULL) {
        sf->listeInodes.premier = nouveauMaillon;
        sf->listeInodes.dernier = nouveauMaillon;
    } else {
        sf->listeInodes.dernier->suivant = nouveauMaillon;
        sf->listeInodes.dernier = nouveauMaillon;
    }

    sf->listeInodes.nbInodes++;

    time_t t = time(NULL);
    sf->superBloc->dateDerModif = t;

    return octetsEcrits;
}
