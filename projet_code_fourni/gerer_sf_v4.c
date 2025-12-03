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
    char fichier2[] = "grenouille.txt";
    char save[] = "systeme.dat";

    printf("--- TEST VERSION 4 ---\n");

    // 1. Création du SF (avec initialisation de la racine)
    printf("[1] Creation SF (avec racine)...\n");
    tSF sf = CreerSF("DisqueV4");
    
    if (sf == NULL) {
        remove(fichier1);
        remove(fichier2);
        return 1;
    }

    // 2. Création de fichiers réels pour le test
    printf("[2] Creation fichiers reels sur le disque...\n");
    FILE *f = fopen(fichier1, "w"); 
    if(f) { 
        fprintf(f, "Miaou"); 
        fclose(f); 
    }
    
    f = fopen(fichier2, "w"); 
    if(f) { 
        fprintf(f, "croicroi"); 
        fclose(f); 
    }

    // 3. Ajout de fichiers dans le SF (et dans le répertoire racine)
    printf("[3] Ajout fichiers...\n");
    EcrireFichierSF(sf, fichier1, ORDINAIRE);
    EcrireFichierSF(sf, fichier2, ORDINAIRE);

    // 4. Test de la commande Ls (mode simple)
    printf("\n[4] LS Simple :\n");
    Ls(sf, 0);

    // 5. Test de la commande Ls (mode détail)
    printf("\n[5] LS Detail :\n");
    Ls(sf, 1);

    /* 6. Afficher le système de fichiers */
    printf("\n[6] Affichage complet du SF :\n");
    AfficherSF(sf);

    // 7. Sauvegarde du SF
    printf("\n[7] Sauvegarde SF vers '%s'...\n", save);
    if (SauvegarderSF(sf, save) == 0) {
        printf("Sauvegarde reussie.\n");
    }

    // 8. Destruction du SF en mémoire
    printf("[8] Destruction SF en memoire...\n");
    DetruireSF(&sf);
    
    // 9. Restaurer le système de fichiers sauvegardé */
    printf("\n[9] Restauration depuis '%s'...\n", save);
    if (ChargerSF(&sf, save) == 0) {
        
        // 10. L'afficher
        printf("\n[10] Affichage complet du SF restaure :\n");
        AfficherSF(sf);

        // 11. Lister sans détail 
        printf("\n[11] Liste sans detail apres restauration :\n");
        Ls(sf, 0);

        // 12. Lister avec détails 
        printf("\n[12] Liste avec details apres restauration :\n");
        Ls(sf, 1);
        
        // 13. Le détruire 
        printf("\n[13] Destruction finale...\n");
        DetruireSF(&sf);
    } else {
        printf("Echec chargement.\n");
    }

    printf("\n--- Nettoyage fichiers temporaires ---\n");
    DetruireSF(&sf);
    remove(fichier1); 
    remove(fichier2); 
    remove(save);
    
    printf("\n--- FIN TEST VERSION 4 ---\n");
    return 0;
}