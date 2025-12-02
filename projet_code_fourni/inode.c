/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 4
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
    tInode inode = (tInode)malloc(sizeof(struct sInode));

    if (inode == NULL) {
        fprintf(stderr, "CreerInode: probleme creation\n");
        return NULL;
    }

    memset(inode, 0, sizeof(struct sInode));
    
    inode->numero = numInode;
    inode->type = type;
    inode->taille = 0;

    for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {
        inode->blocDonnees[i] = NULL;
    }

    time_t t = time(NULL);
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

    printf("-------Inode------[%u] :\n", inode->numero);
    printf("type : ");

    if (inode->type == ORDINAIRE) {
        printf("Ordinaire\n");
    } else if (inode->type == REPERTOIRE) {
        printf("Repertoire\n"); 
    } else if (inode->type == AUTRE) {
        printf("Autre\n");
    } else {
        printf("Inconnu\n");
    }

    printf("  taille : %ld octets\n", inode->taille);
    printf("  date dernier accès : %s", ctime(&(inode->dateDerAcces)));
    printf("  date dernière modification : %s", ctime(&(inode->dateDerModif)));
    printf("  date dernière modification inode : %s", ctime(&(inode->dateDerModifInode)));
    printf("Donnees :\n");
    unsigned char buffer[TAILLE_BLOC];

    for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {
        if (inode->blocDonnees[i] != NULL) {
            LireContenuBloc(inode->blocDonnees[i], buffer, TAILLE_BLOC);
            fwrite(buffer, 1, TAILLE_BLOC, stdout);
        }
    }
    printf("\n");
}

/* V1
 * Copie à l'adresse contenu les taille octets stockés dans un inode.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : l'inode, l'adresse de la zone où recopier et la taille en octets de l'inode
 * Retour : le nombre d'octets effectivement lus dans l'inode ou -1 en cas d'erreur
 */
long LireDonneesInode1bloc(tInode inode, unsigned char *contenu, long taille) {
    return LireDonneesInode(inode, contenu, taille, 0);
}

/* V1
 * Copie dans un inode les taille octets situés à l’adresse contenu.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : l'inode, l'adresse de la zone à recopier et sa taille en octets
 * Retour : le nombre d'octets effectivement écrits dans l'inode ou -1 en cas d'erreur
 */
long EcrireDonneesInode1bloc(tInode inode, unsigned char *contenu, long taille) {
    return EcrireDonneesInode(inode, contenu, taille, 0);
}
/* V3
 * Lit les données d'un inode avec décalage, et les stocke à une adresse donnée
 * Entrées : l'inode d'où les données sont lues, la zone où recopier ces données, la taille en octets
 * des données à lire et le décalage à appliquer (voir énoncé)
 * Sortie : le nombre d'octets effectivement lus, 0 si le décalage est au-delà de la taille
 */
long LireDonneesInode(tInode inode, unsigned char *contenu, long taille, long decalage) {
    long nbOctetsLus = 0;
    long nbOctetsALire;
    unsigned char buffer[TAILLE_BLOC];
    int continuer = 1; 

    if (inode == NULL || decalage >= inode->taille) {
        return 0;
    }

    int iBloc = decalage / TAILLE_BLOC, decalageBloc = decalage % TAILLE_BLOC;

    while (nbOctetsLus < taille && iBloc < NB_BLOCS_DIRECTS && continuer == 1) {
        
        if (inode->blocDonnees[iBloc] == NULL) {
            continuer = 0;
        } else {
            nbOctetsALire = TAILLE_BLOC - decalageBloc;
            
            if (nbOctetsALire > (taille - nbOctetsLus)) {
                nbOctetsALire = taille - nbOctetsLus;
            }
            
            if ((decalage + nbOctetsLus + nbOctetsALire) > inode->taille) {
                nbOctetsALire = inode->taille - (decalage + nbOctetsLus);
            }

            if (nbOctetsALire <= 0) {
                continuer = 0; 
            } else {
                LireContenuBloc(inode->blocDonnees[iBloc], buffer, TAILLE_BLOC);

                for (int i = 0; i < nbOctetsALire; i++) {
                    contenu[nbOctetsLus + i] = buffer[decalageBloc + i];
                }

                nbOctetsLus += nbOctetsALire;
                iBloc++;
                decalageBloc = 0;
            }
        }
    }

    inode->dateDerAcces = time(NULL);
    return nbOctetsLus;
}

/* V3
 * Ecrit dans un inode, avec décalage, ls données stockées à une adresse donnée
 * Entrées : l'inode où écrire le contenu, l'adesse de la zone depuis laquelle lire les données, la taille en octets
 * de ces données et le décalage à appliquer (voir énoncé)
 * Sortie : le nombre d'octets effectivement écrits, ou -1 en cas d'erreur
 */
long EcrireDonneesInode(tInode inode, unsigned char *contenu, long taille, long decalage) {
    long nbOctetsEcrits = 0;
    long nbOctetsAEcrire;
    unsigned char buffer[TAILLE_BLOC];


    if (inode == NULL){
        return -1;
    }
    
    if (decalage > inode->taille) {
        decalage = inode->taille; 
    }

    int iBloc = decalage / TAILLE_BLOC, decalageBloc = decalage % TAILLE_BLOC;

    while (nbOctetsEcrits < taille && iBloc < NB_BLOCS_DIRECTS) {
        if (inode->blocDonnees[iBloc] == NULL) {
            inode->blocDonnees[iBloc] = CreerBloc();
            if (inode->blocDonnees[iBloc] == NULL){
                return -1;
            }
        }

        LireContenuBloc(inode->blocDonnees[iBloc], buffer, TAILLE_BLOC);
        nbOctetsAEcrire = TAILLE_BLOC - decalageBloc;

        if (nbOctetsAEcrire > (taille - nbOctetsEcrits)) {
            nbOctetsAEcrire = taille - nbOctetsEcrits;
        }

        for (int i = 0; i < nbOctetsAEcrire; i++) {
            buffer[decalageBloc + i] = contenu[nbOctetsEcrits + i];
        }

        EcrireContenuBloc(inode->blocDonnees[iBloc], buffer, TAILLE_BLOC);
        nbOctetsEcrits += nbOctetsAEcrire;
        iBloc++;
        decalageBloc = 0;
    }

    if ((decalage + nbOctetsEcrits) > inode->taille) {
        inode->taille = decalage + nbOctetsEcrits;
    }

    time_t t = time(NULL);
    inode->dateDerModif = t;
    inode->dateDerModifInode = t;

    return nbOctetsEcrits;
}

/* V3
 * Sauvegarde toutes les informations contenues dans un inode dans un fichier (sur disque,
 * et préalablement ouvert en écriture et en mode binaire)
 * Entrées : l'inode concerné, l'identificateur du fichier
 * Sortie : 0 en cas de succès, -1 en cas d'erreur
 */
int SauvegarderInode(tInode inode, FILE *fichier) {
    if (inode == NULL || fichier == NULL) {
        return -1;
    }

    if (fwrite(inode, sizeof(struct sInode), 1, fichier) != 1) {
        return -1;
    }

    for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {
        int existe;
        if (inode->blocDonnees[i] != NULL) {
            existe = 1;
        } else {
            existe = 0;
        }

        if (fwrite(&existe, sizeof(int), 1, fichier) != 1) {
            return -1;
        }
        
        if (existe) {
            if (SauvegarderBloc(inode->blocDonnees[i], TAILLE_BLOC, fichier) != 0) {
                return -1;
            }
        }
    }
    return 0;
}

/* V3
 * Charge toutes les informations d'un inode à partir d'un fichier (sur disque,
 * et préalablement ouvert en lecture et en mode binaire)
 * Entrées : l'inode concerné, l'identificateur du fichier
 * Sortie : 0 en cas de succès, -1 en cas d'erreur
 */
int ChargerInode(tInode *pInode, FILE *fichier) {
    if (fichier == NULL) {
        return -1;
    }

    *pInode = (tInode)malloc(sizeof(struct sInode));
    if (*pInode == NULL) {
        return -1;
    }

    if (fread(*pInode, sizeof(struct sInode), 1, fichier) != 1) {
        free(*pInode);
        return -1;
    }

    for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {
        int existe;
        if (fread(&existe, sizeof(int), 1, fichier) != 1) {
            return -1;
        }

        if (existe) {
            (*pInode)->blocDonnees[i] = CreerBloc();
            if (ChargerBloc((*pInode)->blocDonnees[i], TAILLE_BLOC, fichier) != 0) {
                return -1;
            }
        } else {
            (*pInode)->blocDonnees[i] = NULL;
        }
    }
    
    return 0;
}

/* V4
 * Retourne la taille maximale d'un fichier contenu dans un inode.
 * Entrée : l'inode concerné,
 * Sortie : la taille du plus grand fichier de cet inode
 */
long TailleMaxFichier(void) {
    return (long)NB_BLOCS_DIRECTS * TAILLE_BLOC;
}