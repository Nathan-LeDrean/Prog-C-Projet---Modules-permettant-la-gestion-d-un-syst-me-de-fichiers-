#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sf.h"
#include "inode.h"

int main(void) {
    char fichierSource[] = "gros_fichier.txt";
    char fichierSauvegarde[] = "systeme.dat";

    printf("--- TEST VERSION 3 ---\n");

    // 1. Création fichier > 1 bloc
    printf("[1] Creation fichier > 1 bloc...\n");
    FILE *f = fopen(fichierSource, "w");
    if (f) {
        for(int i = 0; i < 200; i++){
            fprintf(f, "A"); 
        }
        fprintf(f, "FIN");
        fclose(f);
    }

    // 2. Création SF
    printf("[2] Creation SF...\n");
    tSF sf = CreerSF("DisqueV3");
    
    // 3. Ajout fichier (multi-blocs)
    printf("[3] Ajout fichier (multi-blocs)...\n");
    if (EcrireFichierSF(sf, fichierSource, ORDINAIRE) > 0) {
        printf("Fichier ajoute.\n");
    }

    // 4. Affichage SF avant sauvegarde
    printf("[4] Affichage SF avant sauvegarde :\n");
    AfficherSF(sf);

    // 5. Sauvegarde SF
    printf("[5] Sauvegarde SF vers '%s'...\n", fichierSauvegarde);
    if (SauvegarderSF(sf, fichierSauvegarde) == 0) {
        printf("Sauvegarde reussie.\n");
    }

    // 6. Destruction SF en memoire
    printf("[6] Destruction SF en memoire...\n");
    DetruireSF(&sf);

    // 7. Chargement SF
    printf("[7] Chargement SF depuis '%s'...\n", fichierSauvegarde);
    if (ChargerSF(&sf, fichierSauvegarde) == 0) {
        printf("Chargement reussi.\n");
        printf("[8] Affichage SF restaure :\n");
        AfficherSF(sf);
        DetruireSF(&sf);
    } else {
        printf("Echec chargement.\n");
    }

    remove(fichierSource);
    remove(fichierSauvegarde);
    
    printf("\n--- FIN TEST VERSION 3 ---\n");
    return EXIT_SUCCESS;
}