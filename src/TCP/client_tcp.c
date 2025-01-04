//BAKARY Asdjad 
// Projet Réseau gestion bancaire
// CLIENT_TCP : Client TCP permettant de se connecter au serveur bancaire

#include <stdio.h>      // Entrées/sorties standard
#include <stdlib.h>     // Fonctions utilitaires (malloc, exit, etc.)
#include <string.h>     // Gestion des chaînes de caractères
#include <unistd.h>     // Fonctions liées aux fichiers et sockets
#include <arpa/inet.h>  // Structures et fonctions pour les communications réseau


#define PORT 8080           // Port de connexion au serveur
#define BUFFER_SIZE 1024    // Taille maximale du tampon pour les messages


// Fonction pour afficher le menu d'options au client
void afficher_menu() {
    printf("\n========== Menu Client ===========\n");
    printf("1. AJOUT (Ajouter une somme)\n");
    printf("2. RETRAIT (Retirer une somme)\n");
    printf("3. SOLDE (Consulter le solde)\n");
    printf("4. OPERATIONS (Voir les dernières opérations)\n");
    printf("5. Quitter\n");
    printf("Choisissez une option : ");
}

// Fonction pour traiter la réponse reçue du serveur
// Paramètre : buffer - Chaîne contenant la réponse du serveur
void traiter_reponse(const char *buffer) {
    if (strncmp(buffer, "OK", 2) == 0) {    // Réponse positive du serveur
        printf("Succès : %s\n", buffer + 3);    // Affiche le message après "OK"
    } else if (strncmp(buffer, "KO", 2) == 0) { // Réponse négative du serveur
        printf("Erreur : %s\n", buffer + 3);    // Affiche le message après "KO"
    } else if (strncmp(buffer, "RES_SOLDE", 9) == 0) {  // Réponse contenant le solde
        printf("Solde et dernière opération : %s\n", buffer + 10);
    } else if (strncmp(buffer, "RES_OPERATIONS", 14) == 0) {    // Réponse contenant les opérations
        printf("Historique des opérations :\n%s\n", buffer + 15);
    } else {         // Réponse non reconnu
        printf("Réponse non reconnue : %s\n", buffer);
    }
}


// Fonction de communication avec le serveur
// Paramètre : sockfd - Descripteur de socket pour la connexion au serveur
void communication(int sockfd) {
    char buffer[BUFFER_SIZE];   // Tampon pour les messages échangés
    char id_client[50], id_compte[50], password[50];    // Identifiants du client

    // Boucle pour l'authentification du client
    while (1) {
        printf("Bienvenue chez votre banque BAK! Veuillez vous authentifier.\n");
        printf("Entrez votre: ID client, ID compte, mot de passe \n");
        scanf("%s %s %s", id_client, id_compte, password);  // Lecture des identifiants
        getchar(); // Consomme le caractère de nouvelle ligne

        // Envoi des identifiants au serveur
        char identifiants[BUFFER_SIZE];
        snprintf(identifiants, sizeof(identifiants), "%s %s %s", id_client, id_compte, password);
        send(sockfd, identifiants, strlen(identifiants), 0);    // Envoi des identifiants au serveur

        // Lecture de la réponse du serveur
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sockfd, buffer, BUFFER_SIZE);

        if (bytes_read <= 0) {  // Si la connexion est fermée ou qu'une erreur survient
            printf("Connexion fermée par le serveur.\n");
            close(sockfd);
            return;
        }

        buffer[bytes_read] = '\0';  // Terminer correctement la chaîne
        traiter_reponse(buffer);    // Traitement de la réponse

        // Si authentification réussie, sortir de la boucle
        if (strncmp(buffer, "OK", 2) == 0) {
            break;
        }
    }

    // Boucle pour traiter les commandes après authentification
    while (1) {
        afficher_menu();    // Affiche le menu au client
        int choix;
        scanf("%d", &choix);    // Lecture de l'option choisie
        getchar(); // Consomme le caractère de nouvelle ligne

        char commande[BUFFER_SIZE] = {0};
        float somme;

        // Traitement des options du menu
        switch (choix) {
            case 1: // AJOUT
                printf("Entrez la somme à ajouter : \n");
                scanf("%f", &somme);
                snprintf(commande, sizeof(commande), "AJOUT %s %s %s %.2f", id_client, id_compte, password, somme);
                break;

            case 2: // RETRAIT
                printf("Entrez la somme à retirer : \n");
                scanf("%f", &somme);
                snprintf(commande, sizeof(commande), "RETRAIT %s %s %s %.2f", id_client, id_compte, password, somme);
                break;

            case 3: // SOLDE
                snprintf(commande, sizeof(commande), "SOLDE %s %s %s", id_client, id_compte, password);
                break;

            case 4: // OPERATIONS
                snprintf(commande, sizeof(commande), "OPERATIONS %s %s %s", id_client, id_compte, password);
                break;

            case 5: // Quitter
                printf("Au revoir %s ! Revenez quand vous voulez! \n", id_client);
                close(sockfd);
                return;

            default: // Option invalide
                printf("Option invalide. Veuillez réessayer.\n");
                continue;
        }

        // Envoi de la commande au serveur
        send(sockfd, commande, strlen(commande), 0);

        // Lecture de la réponse du serveur
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sockfd, buffer, BUFFER_SIZE);

        if (bytes_read <= 0) {  // Si la connexion est fermée ou qu'une erreur survient
            printf("Connexion fermée par le serveur.\n");
            break;
        }

        traiter_reponse(buffer);    // Traitement de la réponse
    }
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;   // Structure pour l'adresse du serveur

    // Création du socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur de création du socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_port = htons(PORT);   // Port du serveur

    // Conversion de l'adresse IP en format reseau
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Adresse IP invalide");
        exit(EXIT_FAILURE);
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erreur de connexion au serveur");
        exit(EXIT_FAILURE);
    }

    printf("Connexion au serveur réussie !\n");

    // Communication avec le serveur
    communication(sock);

    return 0;
}
