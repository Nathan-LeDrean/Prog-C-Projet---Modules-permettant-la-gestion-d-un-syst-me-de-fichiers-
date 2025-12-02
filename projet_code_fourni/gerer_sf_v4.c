/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 4
 * Fichier : gerer_sf_v4.c
 * Programme de test pour la version 4.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sf.h"
#include "inode.h"

int main(void) {
    char fichier1[] = "chat.txt";
    char fichier2[] = "lion.txt";
    char save[] = "systeme.dat";

    printf("--- TEST VERSION 4 ---\n");

    // 1. Création de fichiers réels pour le test
    printf("[1] Creation fichiers reels sur le disque...\n");
    FILE *f = fopen(fichier1, "w"); 
    if(f) { 
        fprintf(f, "Miaou"); 
        fclose(f); 
    }
    
    f = fopen(fichier2, "w"); 
    if(f) { 
        fprintf(f, "Roar"); 
        fclose(f); 
    }

    // 2. Création du SF (avec initialisation de la racine)
    printf("[2] Creation SF (avec racine)...\n");
    tSF sf = CreerSF("DisqueV4");
    
    if (sf == NULL) {
        remove(fichier1);
        remove(fichier2);
        return 1;
    }

    // 3. Ajout de fichiers dans le SF (et dans le répertoire racine)
    printf("[3] Ajout fichiers...\n");
    EcrireFichierSF(sf, fichier1, ORDINAIRE);
    EcrireFichierSF(sf, fichier2, ORDINAIRE);

    // 4. Test de la commande Ls (mode simple)
    printf("\n[4] LS Simple :\n");
    Ls(sf, 1);

    // 5. Test de la commande Ls (mode détail)
    printf("\n[5] LS Detail :\n");
    Ls(sf, 0);

    // 6. Sauvegarde du SF
    printf("\n[6] Sauvegarde SF vers '%s'...\n", save);
    if (SauvegarderSF(sf, save) == 0) {
        printf("Sauvegarde reussie.\n");
    }

    // 7. Destruction du SF en mémoire
    printf("[7] Destruction SF en memoire...\n");
    DetruireSF(&sf);
    
    // 8. Chargement du SF depuis le fichier de sauvegarde
    printf("[8] Chargement SF depuis '%s'...\n", save);
    if (ChargerSF(&sf, save) == 0) {
        printf("Chargement reussi.\n");
        printf("\n[9] LS Detail apres rechargement :\n");
        Ls(sf, 0);
        
        DetruireSF(&sf);
    } else {
        printf("Echec chargement.\n");
    }

    DetruireSF(&sf);
    remove(fichier1); 
    remove(fichier2); 
    remove(save);
    
    printf("\n--- FIN TEST VERSION 4 ---\n");
    return 0;
}