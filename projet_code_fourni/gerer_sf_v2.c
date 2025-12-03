/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 2
 * Fichier : gerer_sf_v2.c
 * Programme de test pour la version 2.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sf.h"     
#include "inode.h"  

int main(void) {
    char nomFichier[] = "test_v2.txt";
    char contenu[] = "Ceci est un fichier pour tester la V2 du projet.";

    printf("--- DEBUT TEST VERSION 2 ---\n");

    // Etape 0 : Création d'un vrai fichier sur le disque pour le test
    printf("[0] Creation du fichier reel '%s' sur le disque...\n", nomFichier);
    FILE *f = fopen(nomFichier, "w"); 
    if (f == NULL) {
        fprintf(stderr, "Erreur creation fichier test\n");
        return 1;
    }

    fprintf(f, "%s", contenu);
    fclose(f);

    // Etape 1 : Création du Système de Fichiers
    printf("\n[1] Creation du Systeme de Fichiers 'DisqueDur'...\n");
    tSF SF = CreerSF("DisqueDur");

    if (SF == NULL) {
        fprintf(stderr, "Erreur fatale : Echec creation SF.\n");
        return 1;
    }

    AfficherSF(SF);

    // Etape 2 : Ajout d'un fichier dans le SF
    printf("\n[2] Ajout du fichier '%s' dans le SF...\n", nomFichier);
    
    long octetsEcrits = Ecrire1BlocFichierSF(SF, nomFichier, ORDINAIRE);

    if (octetsEcrits == -1) {
        fprintf(stderr, "Erreur lors de l'ajout du fichier dans le SF.\n");
    } else {
        printf("Succes : %ld octets importes dans le SF.\n", octetsEcrits);
    }

    /* Etape 3 : Affichage du SF après ajout
     * On doit voir le Super-Bloc à jour et un Inode dans la liste
     */
    printf("\n[3] Etat du SF apres ajout :\n");
    AfficherSF(SF);

    // Etape 4 : Destruction
    printf("\n[4] Destruction du SF...\n");
    DetruireSF(&SF);

    if (SF == NULL) {
        printf("SF detruit correctement.\n");
    }

    remove(nomFichier); 

    printf("\n--- FIN TEST VERSION 2 ---\n");
    return 0;
}