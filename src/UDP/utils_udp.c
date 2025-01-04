//BAKARY Asdjad 
// Projet Réseau gestion bancaire
// UTILS_TCP 1 client 

#include <stdio.h>
#include <time.h>
#include "header_udp.h"

// Fonction pour afficher un log avec l'heure actuelle
void afficher_log(const char *message) {
    char timestamp[50];
    obtenir_heure_actuelle(timestamp, sizeof(timestamp));
    printf("[LOG] [%s] %s\n", timestamp, message);
}

// Fonction pour obtenir l'heure actuelle sous forme de chaîne
void obtenir_heure_actuelle(char *buffer, size_t taille) {
    time_t maintenant = time(NULL);
    strftime(buffer, taille, "%Y-%m-%d %H:%M:%S", localtime(&maintenant));
}

