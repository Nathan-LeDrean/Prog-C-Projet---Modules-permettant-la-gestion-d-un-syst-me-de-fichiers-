/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 4
 * Fichier : repertoire.c
 * Module de gestion d'un répertoire d'un systèmes de fichiers (simulé)
 **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "repertoire.h"
#include "inode.h"

// Définition d'un répertoire
struct sRepertoire
{
  tEntreesRepertoire *table;
};

/* V4
 * Crée un nouveau répertoire.
 * Entrée : aucune
 * Sortie : le répertoire créé, ou NULL si problème
 */
tRepertoire CreerRepertoire(void) {
    tRepertoire rep = (tRepertoire)malloc(sizeof(struct sRepertoire));
    if (rep == NULL) {
        fprintf(stderr, "CreerRepertoire: probleme creation\n");
        return NULL;
    }

    long nbMax = TailleMaxFichier() / sizeof(struct sEntreesRepertoire);
    
    rep->table = (tEntreesRepertoire *)malloc(nbMax * sizeof(tEntreesRepertoire));
    if (rep->table == NULL) {
        fprintf(stderr, "CreerRepertoire: probleme creation table\n");
        free(rep);
        return NULL;
    }

    for (int i = 0; i < nbMax; i++) {
        rep->table[i] = NULL;
    }
    return rep;
}
/* V4
 * Détruit un répertoire et libère la mémoire associée.
 * Entrée : le répertoire à détruire
 * Sortie : aucune
 */
void DetruireRepertoire(tRepertoire *pRep) {
    if (pRep != NULL && *pRep != NULL) {
        long nbMax = TailleMaxFichier() / sizeof(struct sEntreesRepertoire);
        for (int i = 0; i < nbMax; i++) {
            if ((*pRep)->table[i] != NULL) {
                free((*pRep)->table[i]);
            }
        }
        free((*pRep)->table);
        free(*pRep);
        *pRep = NULL;
    }
}

/* V4
 * Écrit une entrée dans un répertoire.
 * Si l'entrée existe déjà dans le répertoire, le numéro d'inode associé est mis à jour.
 * Si l'entrée n'existe pas, elle est ajoutée dans le répertoire.
 * Entrées : le répertoire destination, le nom de l'entrée à écrire,
 *           le numéro d'inode associé à l'entrée
 * Retour : 0 si l'entrée est écrite avec succès, -1 en cas d'erreur
 */
int EcrireEntreeRepertoire(tRepertoire rep, char nomEntree[], unsigned int numeroInode) {
    if (rep == NULL) {
        return -1;
    }

    long nbMax = TailleMaxFichier() / sizeof(struct sEntreesRepertoire);
    int iLibre = -1, trouve = 0;

    for (int i = 0; i < nbMax && !trouve; i++) {
        if (rep->table[i] != NULL) {
            if (strcmp(rep->table[i]->nomEntree, nomEntree) == 0) {
                rep->table[i]->numeroInode = numeroInode;
                trouve = 1;
            }
        } else if (iLibre == -1) {
            iLibre = i;
        }
    }

    if (!trouve) {
        if (iLibre != -1) {
            rep->table[iLibre] = (tEntreesRepertoire)malloc(sizeof(struct sEntreesRepertoire));
            if (rep->table[iLibre] == NULL) {
                return -1;
            }
            memset(rep->table[iLibre], 0, sizeof(struct sEntreesRepertoire));
            strncpy(rep->table[iLibre]->nomEntree, nomEntree, TAILLE_NOM_FICHIER);
            rep->table[iLibre]->nomEntree[TAILLE_NOM_FICHIER] = '\0';
            rep->table[iLibre]->numeroInode = numeroInode;
        } else {
            return -1;
        }
    }
    return 0;
}
/* V4
 * Lit le contenu d'un répertoire depuis un inode.
 * Entrées : le répertoire mis à jour avec le contenu lu,
 *           l'inode source.
 * Retour : 0 si le répertoire est lu avec succès, -1 en cas d'erreur
 */
int LireRepertoireDepuisInode(tRepertoire *pRep, tInode inode) {
    if (pRep == NULL || inode == NULL) {
        return -1;
    }

    *pRep = CreerRepertoire();
    
    if (*pRep == NULL) {
        return -1;
    }

    struct sEntreesRepertoire entreeTemp;
    long decalage = 0;
    long tailleF = Taille(inode);
    long nbEntrees = tailleF / sizeof(struct sEntreesRepertoire);

    for (int i = 0; i < nbEntrees; i++) {
        if (LireDonneesInode(inode, (unsigned char *)&entreeTemp, sizeof(struct sEntreesRepertoire), decalage) != sizeof(struct sEntreesRepertoire)) {
            DetruireRepertoire(pRep);
            return -1;
        }
        EcrireEntreeRepertoire(*pRep, entreeTemp.nomEntree, entreeTemp.numeroInode);
        decalage += sizeof(struct sEntreesRepertoire);
    }
    return 0;
}

/* V4
 * Écrit un répertoire dans un inode.
 * Entrées : le répertoire source et l'inode destination
 * Sortie : 0 si le répertoire est écrit avec succès, -1 en cas d'erreur
 */
int EcrireRepertoireDansInode(tRepertoire rep, tInode inode) {
    if (rep == NULL || inode == NULL) {
        return -1;
    }

    long nbMax = TailleMaxFichier() / sizeof(struct sEntreesRepertoire);
    long decalage = 0;

    for (int i = 0; i < nbMax; i++) {
        if (rep->table[i] != NULL) {
            if (EcrireDonneesInode(inode, (unsigned char *)rep->table[i], sizeof(struct sEntreesRepertoire), decalage) != sizeof(struct sEntreesRepertoire)) {
                return -1;
            }
            decalage += sizeof(struct sEntreesRepertoire);
        }
    }
    return 0;
}
/* V4
 * Récupère les entrées contenues dans un répertoire.
 * Entrées : le répertoire source, un tableau récupérant les numéros d'inodes des entrées du rpertoire
 * Retour : le nombre d'entrées dans le répertoire
 */
int EntreesContenuesDansRepertoire(tRepertoire rep,  struct sEntreesRepertoire tabNumInodes[]){
    if (rep == NULL) {
        return 0;
    }

    int c = 0;
    long nbMax = TailleMaxFichier() / sizeof(struct sEntreesRepertoire);

    for (int i = 0; i < nbMax; i++) {
        if (rep->table[i] != NULL) {
            strcpy(tabNumInodes[c].nomEntree, rep->table[i]->nomEntree);
            tabNumInodes[c].numeroInode = rep->table[i]->numeroInode;
            c++;
        }
    }
    return c;
}
/* V4
 * Compte le nombre d'entrées d'un répertoire.
 * Entrée : le répertoire source
 * Retour : le nombre d'entrées du répertoire
 */
int NbEntreesRepertoire(tRepertoire rep) {
    if (rep == NULL) {
        return 0;
    }

    int c = 0;
    long nbMax = TailleMaxFichier() / sizeof(struct sEntreesRepertoire);

    for (int i = 0; i < nbMax; i++) {
        if (rep->table[i] != NULL) {
            c++;
        }
    }
    return c;
}

/* V4
 * Affiche le contenu d'un répertoire.
 * Entrée : le répertoire à afficher
 * Retour : aucun
 */
void AfficherRepertoire(tRepertoire rep) {
    if (rep == NULL) {
        return;
    }

    long nbMax = TailleMaxFichier() / sizeof(struct sEntreesRepertoire);
    
    for (int i = 0; i < nbMax; i++) {
        if (rep->table[i] != NULL) {
            printf("%s : %u\n", rep->table[i]->nomEntree, rep->table[i]->numeroInode);
        }
    }
}