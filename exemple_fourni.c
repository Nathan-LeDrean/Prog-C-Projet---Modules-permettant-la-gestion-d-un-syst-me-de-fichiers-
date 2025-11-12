#include <stdio.h>
#include <string.h>
#include "inode.h"

#define TAILLE_BUFFER 100

int main(void) {
    tInode inode;
    // Créer un inode (vide) de numéro 0 et dont le type de fichier est ordinaire
    inode = CreerInode( /* A COMPLETER  */ );
    printf("** Inode cree\n");
    AfficherInode( /* A COMPLETER */ );

    // Ecrire/stocker dans cet inode (dans le seul bloc 0 pour cette V1) un "fichier"
    // dont le contenu est lu dans unContenu
    char unContenu[TAILLE_BUFFER];
    strcpy(unContenu, "Bonjour, Monde !");
    long tailleOctetsContenu = sizeof(char)*(strlen(unContenu)+1);
    long nb_ecrits = EcrireDonneesInode1bloc(inode, (unsigned char *)unContenu, tailleOctetsContenu);
    printf("\nNombre d'octets écrits %ld\n", nb_ecrits);

    printf("\n*** Inode rempli\n");
    AfficherInode(inode);

    // Lire le contenu du fichier stocké dans l'inode dans buffer
    char buffer[TAILLE_BUFFER];
    long nb_lus = LireDonneesInode1bloc(inode, (unsigned char *)buffer, sizeof(char)*(TAILLE_BUFFER - 1));
    buffer[nb_lus] = '\0';

    printf("\n*** Lecture du contenu de l'inode :\n");
    printf("Nombre d'octets lus : %ld\n", nb_lus);
    printf("Contenu lu : %s\n", buffer);

    DetruireInode( /* A COMPLETER */ );
    printf("\n*** Inode detruit : tests OK!\n");
    return 0;
}
