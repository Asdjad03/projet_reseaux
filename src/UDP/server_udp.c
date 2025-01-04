//BAKARY Asdjad 
// Projet Réseau gestion bancaire
// SERVEUR_UDP 1 client

#include "header_udp.h"
#include "utils_udp.c"

#include <sys/socket.h>   // Bibliothèque pour les sockets
#include <netinet/in.h>   // Structures pour les adresses réseau
#include <arpa/inet.h>    // Fonctions utilitaires pour les adresses IP
#include <unistd.h>       // Fonctions liées au système 

// Déclaration d'un compte unique utilisé pour simplifier les tests
Compte compte_unique;

// Fonction pour ajouter une somme sur un compte
void ajouter_somme(Compte *compte, float somme, char *reponse) {
    afficher_log("Traitement de l'opération AJOUT");

    compte->solde += somme;         // Mise à jour du solde

    // Enregistrement de l'operation
    char date[50];
    obtenir_heure_actuelle(date, 50);
    snprintf(compte->operations[compte->op_index].type, sizeof(compte->operations[compte->op_index].type), "AJOUT");
    compte->operations[compte->op_index].montant = somme;
    snprintf(compte->operations[compte->op_index].date, sizeof(compte->operations[compte->op_index].date), "%s", date);
    compte->op_index = (compte->op_index + 1) % MAX_OPERATIONS;      // Gestion circulaire des opérations
    if (compte->op_index == 0) {
        afficher_log("L'historique des opérations a atteint sa capacité maximale. Écrasement des anciennes opérations.");
    }

    snprintf(reponse, BUFFER_SIZE, "OK : %.2f € ajouté, nouveau solde : %.2f € \n", somme, compte->solde);
    afficher_log(reponse);      //log de confirmation

}

// Fonction pour retirer une somme sur un compte
void retirer_somme(Compte *compte, float somme, char *reponse) {
    afficher_log("Traitement de l'opération RETRAIT");

    if (compte->solde >= somme) {
        compte->solde -= somme;         //Mise a jour solde

        // Enregistrement de l'operation
        char date[50];
        obtenir_heure_actuelle(date, 50);
        snprintf(compte->operations[compte->op_index].type, sizeof(compte->operations[compte->op_index].type), "RETRAIT");
        compte->operations[compte->op_index].montant = somme;
        snprintf(compte->operations[compte->op_index].date, sizeof(compte->operations[compte->op_index].date), "%s", date);
        compte->op_index = (compte->op_index + 1) % MAX_OPERATIONS;

        snprintf(reponse, BUFFER_SIZE, "OK : %.2f € retiré, nouveau solde : %.2f € \n", somme, compte->solde);
    } else {
        snprintf(reponse, BUFFER_SIZE, "KO : Solde insuffisant");
    }
    afficher_log(reponse);      
}

// Fonction pour obtenir le solde d'un compte
void obtenir_solde(Compte *compte, char *reponse) {
    char date_derniere_op[50] = "Aucune opération"; // Par défaut
    if (compte->op_index > 0) {
        snprintf(date_derniere_op, sizeof(date_derniere_op), "%s", compte->operations[(compte->op_index - 1) % MAX_OPERATIONS].date);
    }
    snprintf(reponse, BUFFER_SIZE, "RES_SOLDE : Solde : %.2f €, Dernière opération : %s \n", compte->solde, date_derniere_op);
    afficher_log(reponse);
}


// Fonction pour obtenir les dernières opérations
void obtenir_operations(Compte *compte, char *reponse) {
    afficher_log("Récupération des dernières opérations");
    snprintf(reponse, BUFFER_SIZE, "RES_OPERATIONS :\n");

    for (int i = 0; i < MAX_OPERATIONS; i++) {
        if (strlen(compte->operations[i].type) > 0) {
            char ligne[100];
            snprintf(ligne, sizeof(ligne), "%s %s %.2f\n", compte->operations[i].type, compte->operations[i].date, compte->operations[i].montant);
            strncat(reponse, ligne, BUFFER_SIZE - strlen(reponse) - 1);
        }
    }

    if (strlen(reponse) == strlen("RES_OPERATIONS :\n")) {
        strncat(reponse, "Aucune opération enregistrée\n", BUFFER_SIZE - strlen(reponse) - 1);
    }
    afficher_log(reponse);
}

// Initialisation du compte unique avec des valeurs par défaut
void initialiser_compte() {
    strcpy(compte_unique.id_client, "asdjad");
    strcpy(compte_unique.id_compte, "poly");
    strcpy(compte_unique.password, "ei4");
    compte_unique.solde = 100.0; // Solde initial
    compte_unique.op_index = 0;  // Aucun historique pour l'instant
}

// Fonction pour authentifier un compte
int authentifier_compte(const char *id_client, const char *id_compte, const char *password) {
    if (strcmp(compte_unique.id_client, id_client) == 0 &&
        strcmp(compte_unique.id_compte, id_compte) == 0 &&
        strcmp(compte_unique.password, password) == 0) {
        return 1; // Authentification réussie
    }
    return 0; // Authentification échouée
}

// Fonction pour traiter une requête UDP du client
void traiter_requete_udp(int server_fd, struct sockaddr_in *client_addr, socklen_t client_addr_len) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Réception de la requête du client
    int n = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)client_addr, &client_addr_len);
    if (n < 0) {
        perror("Erreur de réception");
        return;
    }
    buffer[n] = '\0';

    printf("[LOG] Requête reçue : %s\n", buffer);

    // Extraction des paramètres de la requête
    char commande[20], id_client[50], id_compte[50], password[50];
    float somme = 0.0;

    if (sscanf(buffer, "%s %s %s %s %f", commande, id_client, id_compte, password, &somme) >= 3) {
        // Authentification
        if (authentifier_compte(id_client, id_compte, password)) {
            // Gestion des commandes
            if (strcmp(commande, "AJOUT") == 0) {
                if (somme > 0) {
                    ajouter_somme(&compte_unique, somme, response);
                } else {
                    snprintf(response, BUFFER_SIZE, "ERREUR : Montant invalide pour AJOUT");
                }
            } else if (strcmp(commande, "RETRAIT") == 0) {
                if (somme > 0) {
                    retirer_somme(&compte_unique, somme, response);
                } else {
                    snprintf(response, BUFFER_SIZE, "ERREUR : Montant invalide pour RETRAIT");
                }
            } else if (strcmp(commande, "SOLDE") == 0) {
                obtenir_solde(&compte_unique, response);
            } else if (strcmp(commande, "OPERATIONS") == 0) {
                obtenir_operations(&compte_unique, response);
            } else {
                snprintf(response, BUFFER_SIZE, "ERREUR : Commande inconnue");
            }
        } else {
            snprintf(response, BUFFER_SIZE, "ERREUR : Authentification échouée");
        }
    } else {
        snprintf(response, BUFFER_SIZE, "ERREUR : Format de commande invalide");
    }

    // Envoi de la réponse au client
    sendto(server_fd, response, strlen(response), 0, (struct sockaddr *)client_addr, client_addr_len);
    printf("[LOG] Réponse envoyée : %s\n", response);
}

// Fonction principale
int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);


    // Initialisation du compte unique
    initialiser_compte();

    // Création du socket UDP
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur de création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Liaison du socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur de liaison");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Serveur en attente de requêtes sur le port %d...\n", PORT);

    while (1) {
        traiter_requete_udp(server_fd, &client_addr, client_addr_len);
    }

    close(server_fd);
    return 0;
}
