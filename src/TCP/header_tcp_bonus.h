//BAKARY Asdjad 
// Projet Réseau gestion bancaire
// HEADER_TCP 1 client 

#ifndef HEADER_H_BONUS
#define HEADER_H_BONUS

#include <stdio.h>      // Entrées/sorties standard
#include <stdlib.h>     // Fonctions utilitaires (malloc, free, exit ...)
#include <string.h>     // Gestion des chaînes de caractères
#include <unistd.h>     // gestion des fichiers et sockets
#include <arpa/inet.h>  // Fonctions et structures pour les communications réseau
#include <time.h>       // Gestion des dates et heures

#define PORT 8080             // Port d'écoute pour le serveur
#define BUFFER_SIZE 1024      // Taille max du tampon pour les communications
#define MAX_COMPTES 10        // Nombre maximum de comptes
#define MAX_OPERATIONS 10     // Nombre maximum d'opérations par compte

// Structure pour représenter une opération
typedef struct {
    char type[20];           // Type d'opération (AJOUT, RETRAIT, etc.)
    float montant;           // Montant de l'opération
    char date[50];           // Date de l'opération
} Operation;

// Structure pour représenter un compte
typedef struct {
    char id_compte[50];      // Identifiant unique du compte
    char password[50];       // Mot de passe du compte
    float solde;             // Solde actuel du compte
    Operation operations[MAX_OPERATIONS]; // Tableau des dernières opérations
    int op_index;            // Index pour insérer la prochaine opération
} Compte;

// Structure pour représenter un client
typedef struct {
    char id_client[50];      // Identifiant du client
    Compte comptes[MAX_COMPTES]; // Tableau de comptes du client
    int nb_comptes;          // Nombre de comptes associés au client
} Client;

// Prototypes des fonctions
// Ces fonctions seront définies dans les fichiers d'implémentation correspondants 

// Fonction pour ajouter une somme au compte
void ajouter_somme(int client_index, const char *id_compte, float somme, char *reponse);

// Fonction pour retirer une somme du compte
void retirer_somme(int client_index, const char *id_compte, float somme, char *reponse);

// Fonction pour obtenir le solde actuel du compte
void obtenir_solde(int client_index, const char *id_compte, char *reponse);

// Fonction pour récupérer l'historique des opérations
void obtenir_operations(int client_index, const char *id_compte, char *reponse);

// Fonction principale pour traiter les interactions avec un client
void traiter_client(int client_socket);

// Fonction pour afficher un log avec un message
void afficher_log(const char *message);

// Fonction pour obtenir l'heure actuelle sous forme de chaîne de caractères
void obtenir_heure_actuelle(char *buffer, int taille);

#endif
