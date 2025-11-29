/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 4
 * Fichier : sf.c
 * Module de gestion d'un systèmes de fichiers (simulé)
 **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "inode.h"      
#include "repertoire.h" 
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
    tSuperBloc sb = (tSuperBloc)malloc(sizeof(struct sSuperBloc));
    if (sb == NULL) {
        fprintf(stderr, "Creer Super Bloc probleme creation\n");
        return NULL;
    }
    memset(sb, 0, sizeof(struct sSuperBloc));
    strncpy(sb->nomDisque, nomDisque, TAILLE_NOM_DISQUE);
    sb->nomDisque[TAILLE_NOM_DISQUE] = '\0';
    sb->dateDerModif = time(NULL);
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
    tInode racineInode = CreerInode(0, REPERTOIRE);

    if (racineInode == NULL) {
        DetruireSuperBloc(&(sf->superBloc));
        free(sf);
        return NULL;
    }

    tRepertoire repRacine = CreerRepertoire();
    if (repRacine == NULL) {
        return NULL;
    }
    
    EcrireRepertoireDansInode(repRacine, racineInode);
    DetruireRepertoire(&repRacine);
    struct sListeInodesElement *elt = malloc(sizeof(struct sListeInodesElement));
    elt->inode = racineInode;
    elt->suivant = NULL;
    sf->listeInodes.premier = elt;
    sf->listeInodes.dernier = elt;
    sf->listeInodes.nbInodes = 1;

    return sf;
}

/* V2
 * Détruit un système de fichiers et libère la mémoire associée.
 * Entrée : le SF à détruire
 * Sortie : aucune
 */
void DetruireSF(tSF *pSF) {
    if (pSF != NULL && *pSF != NULL) {
        DetruireSuperBloc(&((*pSF)->superBloc));
        struct sListeInodesElement *courant = (*pSF)->listeInodes.premier;
        while (courant != NULL) {
            struct sListeInodesElement *suivant = courant->suivant;
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
void AfficherSF(tSF sf) {
    if (sf == NULL) {
        return;
    }

    AfficherSuperBloc(sf->superBloc);
    printf("Inodes :\n");
    struct sListeInodesElement *courant = sf->listeInodes.premier;

    while (courant != NULL) {
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
    tInode inode = CreerInode(sf->listeInodes.nbInodes, type);
    if (inode == NULL) {
        return -1;
    }

    FILE *f = fopen(nomFichier, "rb");
    if (f == NULL) {
        DetruireInode(&inode); 
        return -1;
    }

    unsigned char buffer[TAILLE_BLOC];
    long lus = fread(buffer, 1, TAILLE_BLOC, f);
    fclose(f);

    if (EcrireDonneesInode1bloc(inode, buffer, lus) == -1) {
        DetruireInode(&inode);
        return -1;
    }

    struct sListeInodesElement *elt = malloc(sizeof(struct sListeInodesElement));
    elt->inode = inode;
    elt->suivant = NULL;
    if (sf->listeInodes.premier == NULL) {
        sf->listeInodes.premier = elt;
    } else {
        sf->listeInodes.dernier->suivant = elt;
    }
    sf->listeInodes.dernier = elt;
    sf->listeInodes.nbInodes++;

    return lus;
}

/* V3
 * Ecrit un fichier (d'un nombre de blocs quelconque) dans le système de fichiers.
 * Si la taille du fichier à écrire dépasse la taille maximale d'un fichier dans le SF(10 x 64 octets),
 * seuls les 640 premiers octets seront écrits dans le système de fichiers.
 * Entrées : le système de fichiers, le nom du fichier (sur disque) et son type dans le SF (simulé)
 * Sortie : le nombre d'octets effectivement écrits, -1 en cas d'erreur.
 */
long EcrireFichierSF(tSF sf, char nomFichier[], natureFichier type) {
    if (sf == NULL) {
        return -1;
    }

    FILE *f = fopen(nomFichier, "rb");
    if (f == NULL) {
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long tailleFichier = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *buffer = malloc(tailleFichier);

    if (fread(buffer, 1, tailleFichier, f) != (size_t)tailleFichier) {
        free(buffer); 
        fclose(f); 
        return -1;
    }

    fclose(f);
    int id = sf->listeInodes.nbInodes;
    tInode inode = CreerInode(id, type);

    if (inode == NULL) {
        free(buffer); return -1;
    }

    long ecrits = EcrireDonneesInode(inode, buffer, tailleFichier, 0);
    free(buffer);

    if (ecrits == -1) {
        DetruireInode(&inode); return -1;
    }

    struct sListeInodesElement *elt = malloc(sizeof(struct sListeInodesElement));
    elt->inode = inode;
    elt->suivant = NULL;
    sf->listeInodes.dernier->suivant = elt;
    sf->listeInodes.dernier = elt;
    sf->listeInodes.nbInodes++;
    tInode racineInode = sf->listeInodes.premier->inode;
    tRepertoire repRacine;

    if (LireRepertoireDepuisInode(&repRacine, racineInode) == -1) {
        return -1;
    }
    
    EcrireEntreeRepertoire(repRacine, nomFichier, id);
    EcrireRepertoireDansInode(repRacine, racineInode);
    DetruireRepertoire(&repRacine);

    return ecrits;
}
/* V3
 * Sauvegarde un système de fichiers dans un fichier (sur disque).
 * Entrées : le système de fichiers, le nom du fichier sauvegarde (sur disque)
 * Sortie : 0 en cas de succèe, -1 en cas d'erreur
 */
int SauvegarderSF(tSF sf, char nomFichier[]) {
    if (sf == NULL) {
        return -1;
    }
    FILE *f = fopen(nomFichier, "wb");
    if (f == NULL) {
        return -1;
    }

    fwrite(sf->superBloc, sizeof(struct sSuperBloc), 1, f);
    fwrite(&(sf->listeInodes.nbInodes), sizeof(int), 1, f);
    struct sListeInodesElement *courant = sf->listeInodes.premier;
    while (courant != NULL) {
        if (SauvegarderInode(courant->inode, f) != 0) {
            fclose(f); return -1;
        }
        courant = courant->suivant;
    }

    fclose(f);
    return 0;
}

/* V3
 * Restaure le contenu d'un système de fichiers depuis un fichier sauvegarde (sur disque).
 * Entrées : le système de fichiers où restaurer, le nom du fichier sauvegarde (sur disque)
 * Sortie : 0 en cas de succèe, -1 en cas d'erreur
 */
int ChargerSF(tSF *pSF, char nomFichier[]) {
    FILE *f = fopen(nomFichier, "rb");
    if (f == NULL) {
        return -1;
    }

    *pSF = (tSF)malloc(sizeof(struct sSF));
    
    (*pSF)->superBloc = (tSuperBloc)malloc(sizeof(struct sSuperBloc));
    fread((*pSF)->superBloc, sizeof(struct sSuperBloc), 1, f);

    int nb;
    fread(&nb, sizeof(int), 1, f);
    (*pSF)->listeInodes.nbInodes = nb;
    (*pSF)->listeInodes.premier = NULL;
    (*pSF)->listeInodes.dernier = NULL;

    for (int i = 0; i < nb; i++) {
        tInode inode;
        if (ChargerInode(&inode, f) != 0) {
            fclose(f); return -1;
        }
        
        struct sListeInodesElement *elt = malloc(sizeof(struct sListeInodesElement));
        elt->inode = inode;
        elt->suivant = NULL;
        if ((*pSF)->listeInodes.premier == NULL) {
            (*pSF)->listeInodes.premier = elt;
        } else {
            (*pSF)->listeInodes.dernier->suivant = elt;
        }
        (*pSF)->listeInodes.dernier = elt;
    }
    fclose(f);
    return 0;
}

/* V4
 * Affiche le détail du répertoire racine d'un système de fichiers.
 * Entrées : le système de fichiers,
 * et un booléen indiquant si l'affichage doit être détaillé (true) ou non (false),
 * voir l'énoncé pour les consignes d'affichage à suivre
 * Sortie : 0 en cas de succès, -1 en cas d'erreur
 */
int Ls(tSF sf, bool detail) {
    if (sf == NULL) {
        return 1; 
    }

    tInode racineInode = sf->listeInodes.premier->inode; 
    tRepertoire rep;
    
    if (LireRepertoireDepuisInode(&rep, racineInode) == -1) {
        return 1;
    }

    int nbFichiers = NbEntreesRepertoire(rep);
    printf("Nombre de fichiers dans le répertoire racine : %d\n", nbFichiers);
    long maxEntrees = TailleMaxFichier() / sizeof(struct sEntreesRepertoire);
    struct sEntreesRepertoire *tab = malloc(maxEntrees * sizeof(struct sEntreesRepertoire));
    EntreesContenuesDansRepertoire(rep, tab);

    for (int i = 0; i < nbFichiers; i++) {
        if (detail) {
            tInode inodeCible = NULL;
            struct sListeInodesElement *courant = sf->listeInodes.premier;
            while(courant != NULL) {
                if (Numero(courant->inode) == tab[i].numeroInode) {
                    inodeCible = courant->inode;
                    break;
                }
                courant = courant->suivant;
            }

            if (inodeCible) {
                char *typeStr = "INCONNU";
                if (Type(inodeCible) == ORDINAIRE) {
                    typeStr = "ORDINAIRE";
                } else if (Type(inodeCible) == REPERTOIRE) {
                    typeStr = "REPERTOIRE";
                }
                
                char *date = ctime((const time_t []){DateDerModif(inodeCible)});
                date[strlen(date)-1] = '\0';

                printf("%-3u %-12s %6ld %s %s\n", 
                    Numero(inodeCible), 
                    typeStr, 
                    Taille(inodeCible), 
                    date, 
                    tab[i].nomEntree);
            }
        } else {
            printf("%s\n", tab[i].nomEntree);
        }
    }

    free(tab);
    DetruireRepertoire(&rep);
    return 0;
}


