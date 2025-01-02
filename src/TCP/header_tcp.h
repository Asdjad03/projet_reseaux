#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080             // Port d'écoute pour le serveur
#define BUFFER_SIZE 1024      // Taille du tampon pour les communications
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
    char id_compte[50];      // Identifiant du compte
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
void ajouter_somme(Compte *compte, float somme, char *reponse);
void retirer_somme(Compte *compte, float somme, char *reponse);
void obtenir_solde(Compte *compte, char *reponse);
void obtenir_operations(Compte *compte, char *reponse);
void traiter_client(int client_socket);
void afficher_log(const char *message);
void obtenir_heure_actuelle(char *buffer, int taille);

#endif
