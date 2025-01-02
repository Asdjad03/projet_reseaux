#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Fonction pour afficher le menu
void afficher_menu() {
    printf("\n========== Menu Client ===========\n");
    printf("1. AJOUT (Ajouter une somme)\n");
    printf("2. RETRAIT (Retirer une somme)\n");
    printf("3. SOLDE (Consulter le solde)\n");
    printf("4. OPERATIONS (Voir les dernières opérations)\n");
    printf("5. Quitter\n");
    printf("Choisissez une option : ");
}

//Traitement des reponses 
void traiter_reponse(const char *buffer) {
    if (strncmp(buffer, "OK", 2) == 0) {
        printf("Succès : %s\n", buffer + 3);
    } else if (strncmp(buffer, "KO", 2) == 0) {
        printf("Erreur : %s\n", buffer + 3);
    } else if (strncmp(buffer, "RES_SOLDE", 9) == 0) {
        printf("Solde et dernière opération : %s\n", buffer + 10);
    } else if (strncmp(buffer, "RES_OPERATIONS", 14) == 0) {
        printf("Historique des opérations :\n%s\n", buffer + 15);
    } else {
        printf("Réponse non reconnue : %s\n", buffer);
    }
}


// Fonction de communication avec le serveur
void communication(int sockfd) {
    char buffer[BUFFER_SIZE];
    char id_client[50], id_compte[50], password[50];

     // Boucle d'authentification
    while (1) {
        printf("Bienvenue chez votre banque BAK! Veuillez vous authentifier.\n");
        printf("Entrez votre: ID client, ID compte, mot de passe \n");
        scanf("%s %s %s", id_client, id_compte, password);
        getchar(); // Consommer le saut de ligne

        // Envoi des identifiants au serveur
        char identifiants[BUFFER_SIZE];
        snprintf(identifiants, sizeof(identifiants), "%s %s %s", id_client, id_compte, password);
        send(sockfd, identifiants, strlen(identifiants), 0);

        // Lecture de la réponse du serveur
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sockfd, buffer, BUFFER_SIZE);

        if (bytes_read <= 0) {
            printf("Connexion fermée par le serveur.\n");
            close(sockfd);
            return;
        }

        buffer[bytes_read] = '\0'; // Terminer correctement la chaîne
        traiter_reponse(buffer);

        // Si authentification réussie, sortir de la boucle
        if (strncmp(buffer, "OK", 2) == 0) {
            break;
        }

        // Sinon, redemander les identifiants
        //printf("Erreur : Identifiants incorrects. Veuillez réessayer.\n");
    }

    while (1) {
        afficher_menu();
        int choix;
        scanf("%d", &choix);
        getchar(); // Consommer le saut de ligne

        char commande[BUFFER_SIZE] = {0};
        float somme;

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

            default:
                printf("Option invalide. Veuillez réessayer.\n");
                continue;
        }

        // Envoi de la commande au serveur
        send(sockfd, commande, strlen(commande), 0);

        // Lecture de la réponse du serveur
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sockfd, buffer, BUFFER_SIZE);

        if (bytes_read <= 0) {
            printf("Connexion fermée par le serveur.\n");
            break;
        }

        traiter_reponse(buffer);
    }
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;

    // Création du socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur de création du socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Conversion de l'adresse IP
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
