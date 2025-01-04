//BAKARY Asdjad 
// Projet Réseau gestion bancaire
// UTILS_TCP 1 client 

#include <stdio.h>      // entrées/sorties
#include <time.h>       // gestion et manipulation des dates et heures
#include "header_tcp.h" //fichier d'en-tête contenant les définitions globales



// Fonction pour afficher un log avec un horodatage
// Cette fonction affiche un message de log précédé de la date et de l'heure actuelles.
// Paramètre : 
//   - message : Chaîne de caractères contenant le message à afficher.
void afficher_log(const char *message) {
    char timestamp[50];
    obtenir_heure_actuelle(timestamp, sizeof(timestamp));
    printf("[LOG] [%s] %s\n", timestamp, message);
}

// Fonction pour obtenir l'heure actuelle sous forme de chaîne
// Cette fonction génère une chaîne représentant l'heure actuelle dans le format "YYYY-MM-DD HH:MM:SS".
// Paramètres : 
//   - buffer : Tableau de caractères où l'heure sera stockée.
//   - taille : Taille maximale du tableau buffer (prévention des débordements).
void obtenir_heure_actuelle(char *buffer, int taille) {
    time_t maintenant = time(NULL);     // Récupération de l'heure actuelle en secondes depuis l'époque Unix
    strftime(buffer, taille, "%Y-%m-%d %H:%M:%S", localtime(&maintenant));
}
