/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 1
 * Fichier : inode.c
 * Module de gestion des inodes.
 **/
#include <stdlib.h> 
#include <stdio.h>  
#include <time.h>  
#include <string.h> 
#include "inode.h"
#include "bloc.h"

// Nombre maximal de blocs dans un inode
#define NB_BLOCS_DIRECTS 10

// Définition d'un inode
struct sInode
{
  // Numéro de l'inode
  unsigned int numero;
  // Le type du fichier : ordinaire, répertoire ou autre
  natureFichier type;
  // La taille en octets du fichier
  long taille;
  // Les adresses directes vers les blocs (NB_BLOCS_DIRECTS au maximum)
  tBloc blocDonnees[NB_BLOCS_DIRECTS];
  // Les dates : dernier accès à l'inode, dernière modification du fichier
  // et de l'inode
  time_t dateDerAcces, dateDerModif, dateDerModifInode;
};

/* V1
 * Crée et retourne un inode.
 * Entrées : numéro de l'inode et le type de fichier qui y est associé
 * Retour : l'inode créé ou NULL en cas de problème
 */
tInode CreerInode(int numInode, natureFichier type) {
    time_t t;

    // Allocation mémoire
    tInode inode = (tInode)malloc(sizeof(struct sInode));

    if (inode == NULL) {
        fprintf(stderr, "CreerInode: probleme creation\n");
        return NULL;
    }

    inode->numero = numInode;
    inode->type = type;
    inode->taille = 0;

    // Initialisation des blocs à NULL
    for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {
        inode->blocDonnees[i] = NULL;
    }

    // Gestion du temps
    t = time(NULL);
    inode->dateDerAcces = t;
    inode->dateDerModif = t;
    inode->dateDerModifInode = t;

    return inode;
}

/* V1
 * Détruit un inode.
 * Entrée : l'inode à détruire
 * Retour : aucun
 */
void DetruireInode(tInode *pInode) {
    if (pInode != NULL && *pInode != NULL) {
        for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {
            if ((*pInode)->blocDonnees[i] != NULL) {
                DetruireBloc(&((*pInode)->blocDonnees[i]));
            }
        }
        free(*pInode);
        *pInode = NULL;
    }
}

/* V1
 * Récupère la date de dernier accès à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la date de dernier accès
 * Retour : la date de dernier accès à l'inode
 */
time_t DateDerAcces(tInode inode) {
    return inode->dateDerAcces;
}
/* V1
 * Récupère la date de dernière modification d'un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la date de dernière modification
 * Retour : la date de dernière modification de l'inode
 */
time_t DateDerModif(tInode inode) {
    return inode->dateDerModif;
}

/* V1
 * Récupère la date de dernière modification d'u fichier associé à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la date de dernière modification du fichier associé
 * Retour : la date de dernière modification du fichier associé à l'inode
 */
time_t DateDerModifFichier(tInode inode) {
    return inode->dateDerModif;
}

/* V1
 * Récupère le numéro d'un inode.
 * Entrée : l'inode pour lequel on souhaite connaître le numéro
 * Retour : le numéro de l'inode
 */
unsigned int Numero(tInode inode) {
    return inode->numero;
}
/* V1
 * Récupère la taille en octets du fichier associé à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la taille
 * Retour : la taille en octets du fichier associé à l'inode
 */
long Taille(tInode inode) {
    return inode->taille;
}

/* V1
 * Récupère le type du fichier associé à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître le tyep de fichier associé
 * Retour : le type du fichier associé à l'inode
 */
natureFichier Type(tInode inode) {
    return inode->type;
}

/* V1
 * Affiche les informations d'un inode
 * Entrée : l'inode dont on souhaite afficher les informations
 * Retour : aucun
 */
void AfficherInode(tInode inode) {
    if (inode == NULL) {
        printf("vide\n"); 
        return;
    }

    printf("Inode %u :\n", inode->numero); 
    printf("type : ");
    
    // Remplacement du switch par des if / else if
    if (inode->type == ORDINAIRE) {
        printf("Ordinaire\n");
    } else if (inode->type == REPERTOIRE) {
        printf("Repertoire\n");
    } else if (inode->type == AUTRE) {
        printf("Autre\n");
    } else {
        printf("Inconnu\n");
    }

    printf("taille : %ld octets\n", inode->taille);
    printf("date dernier accès : %s", ctime(&(inode->dateDerAcces)));
    printf("date dernière modification : %s", ctime(&(inode->dateDerModif)));
    printf("date dernière modification inode : %s", ctime(&(inode->dateDerModifInode)));
}

/* V1
 * Copie à l'adresse contenu les taille octets stockés dans un inode.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : l'inode, l'adresse de la zone où recopier et la taille en octets de l'inode
 * Retour : le nombre d'octets effectivement lus dans l'inode ou -1 en cas d'erreur
 */
long LireDonneesInode1bloc(tInode inode, unsigned char *contenu, long taille) {
    if (inode == NULL || inode->blocDonnees[0] == NULL) {
        return 0; 
    }
    
    inode->dateDerAcces = time(NULL);
    
    return LireContenuBloc(inode->blocDonnees[0], contenu, taille);
}

/* V1
 * Copie dans un inode les taille octets situés à l’adresse contenu.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : l'inode, l'adresse de la zone à recopier et sa taille en octets
 * Retour : le nombre d'octets effectivement écrits dans l'inode ou -1 en cas d'erreur
 */
long EcrireDonneesInode1bloc(tInode inode, unsigned char *contenu, long taille) {
    time_t t;

    if (inode == NULL) {
        return -1;
    }

    if (inode->blocDonnees[0] == NULL) {
        inode->blocDonnees[0] = CreerBloc();
        if (inode->blocDonnees[0] == NULL) {
            return -1; 
        }
    }
    
    long octetsEcrits = EcrireContenuBloc(inode->blocDonnees[0], contenu, taille);
    
    if (octetsEcrits > inode->taille) {
        inode->taille = octetsEcrits;
    }

    t = time(NULL);
    inode->dateDerModif = t;
    inode->dateDerModifInode = t;

    return octetsEcrits;
}