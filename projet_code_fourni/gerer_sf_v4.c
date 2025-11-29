#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sf.h"
#include "inode.h"

int main(void) {
    char fichier1[] = "chat.txt";
    char fichier2[] = "lion.txt";
    char save[] = "systeme.dat";
    FILE *f;
    tSF sf;

    printf("--- TEST VERSION 4 ---\n");

    f = fopen(fichier1, "w"); 
    if(f) { 
        fprintf(f, "Miaou"); 
        fclose(f); 
    }
    
    f = fopen(fichier2, "w"); 
    if(f) { 
        fprintf(f, "Roar"); 
        fclose(f); 
    }

    printf("[1] Creation SF (avec racine)...\n");
    sf = CreerSF("DisqueV4");
    if (sf == NULL) return 1;

    printf("[2] Ajout fichiers...\n");
    EcrireFichierSF(sf, fichier1, ORDINAIRE);
    EcrireFichierSF(sf, fichier2, ORDINAIRE);

    printf("\n[3] LS Simple :\n");
    Ls(sf, false);

    printf("\n[4] LS Detail :\n");
    Ls(sf, true);

    printf("\n[5] Sauvegarde et rechargement...\n");
    SauvegarderSF(sf, save);
    DetruireSF(&sf);
    
    ChargerSF(&sf, save);

    printf("\n[6] LS Detail apres rechargement :\n");
    Ls(sf, true);

    DetruireSF(&sf);
    remove(fichier1); 
    remove(fichier2); 
    remove(save);
    
    printf("\n--- FIN TEST VERSION 4 ---\n");
    return 0;
}