/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 1
 * Fichier : gerer_sf_v1.c
 * Programme de test pour la version 1.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inode.h"  
#include "bloc.h"

int main(void) {
    printf("--- DEBUT TEST VERSION 1 ---\n");

    // 1. Création d'un inode (numéro 1, type ORDINAIRE)
    printf("\n[1] Creation de l'inode n°1...\n");
    tInode monInode = CreerInode(1, ORDINAIRE);

    if (monInode == NULL) {
        return 1;
    }

    // 2. Affichage avant écriture
    printf("--- Etat initial ---\n");
    AfficherInode(monInode);

    // 3. Écriture de données
    char message[] = "Bonjour, ceci est un test d'ecriture dans le bloc 0.";
    long tailleMsg = strlen(message) + 1; 

    printf("\n[2] Ecriture de %ld octets : \"%s\"\n", tailleMsg, message);
    long res = EcrireDonneesInode1bloc(monInode, (unsigned char*)message, tailleMsg);
    
    if (res != -1) {
        printf("Succes : %ld octets ecrits.\n", res);
    } else {
        fprintf(stderr, "Erreur ecriture.\n");
    }

    // 4. Affichage après écriture (vérification taille et dates)
    printf("\n--- Etat apres ecriture ---\n");
    AfficherInode(monInode);

    // 5. Lecture pour vérification
    printf("\n[3] Lecture des donnees...\n");
    unsigned char buffer[TAILLE_BLOC]; 
    long lus = LireDonneesInode1bloc(monInode, buffer, tailleMsg);
    
    if (lus > 0) {
        printf("Lu : \"%s\"\n", buffer);
    }

    // 6. Destruction
    printf("\n[4] Destruction de l'inode...\n");
    DetruireInode(&monInode); 

    if (monInode == NULL) {
        printf("Inode detruit avec succes (pointeur NULL).\n");
    }

    printf("\n--- FIN TEST VERSION 1 ---\n");
    return 0;
}