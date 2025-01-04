#ifndef HEADER_UDP_H
#define HEADER_UDP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_OPERATIONS 10  // Nombre maximal d'opérations à enregistrer
#define BUFFER_SIZE 1024   // Taille du buffer pour les communications
#define PORT 8080          // Port du serveur

// Structure pour une opération
typedef struct {
    char type[20];    // Type d'opération : AJOUT ou RETRAIT
    char date[50];    // Date de l'opération
    float montant;    // Montant de l'opération
} Operation;

// Structure pour un compte unique
typedef struct {
    char id_client[50];     // ID client
    char id_compte[50];     // ID compte
    char password[50];      // Mot de passe
    float solde;            // Solde du compte
    Operation operations[MAX_OPERATIONS]; // Historique des opérations
    int op_index;           // Index de l'opération courante
} Compte;

// Prototypes des fonctions
void afficher_log(const char *message);                      // Fonction pour afficher des logs
void obtenir_heure_actuelle(char *buffer, size_t taille);       // Fonction pour obtenir l'heure actuelle
void ajouter_somme(Compte *compte, float somme, char *reponse); // Ajouter une somme au compte
void retirer_somme(Compte *compte, float somme, char *reponse); // Retirer une somme du compte
void obtenir_solde(Compte *compte, char *reponse);           // Obtenir le solde du compte
void obtenir_operations(Compte *compte, char *reponse);      // Obtenir l'historique des opérations

#endif
