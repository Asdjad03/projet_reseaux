#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080   // Port de connexion

// Fonction pour afficher le menu
void afficher_menu() {
    printf("\n=== Menu ===\n");
    printf("1. AJOUT (Ajouter une somme)\n");
    printf("2. RETRAIT (Retirer une somme)\n");
    printf("3. SOLDE (Obtenir le solde du compte)\n");
    printf("4. OPERATIONS (Voir les 10 dernières opérations)\n");
    printf("5. Quitter\n");
    printf("Choisissez une option : ");
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Création du socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Erreur de création du socket\n");
        return -1;
    }

    // Configuration de l'adresse du serveur
    serv_addr.sin_family = AF_INET;     // IPv4
    serv_addr.sin_port = htons(PORT);   // Convertir le port en format réseau

    // Convertir l'adresse IP en format réseau
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Adresse IP invalide\n");
        return -1;
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connexion au serveur échouée\n");
        return -1;
    }

    int choix;
    do {
        afficher_menu();
        scanf("%d", &choix);

        char commande[1024];
        char id_client[50], id_compte[50], password[50];
        float somme;

        switch (choix) {
            case 1: // AJOUT
                printf("Entrez : id_client id_compte password somme\n");
                scanf("%s %s %s %f", id_client, id_compte, password, &somme);
                if (strlen(id_client) == 0 || strlen(id_compte) == 0 || strlen(password) == 0) {
                    printf("Erreur : Tous les champs doivent être remplis.\n");
                    continue;
                }
                snprintf(commande, sizeof(commande), "AJOUT %s %s %s %.2f", id_client, id_compte, password, somme);
                break;

            case 2: // RETRAIT
                printf("Entrez : id_client id_compte password somme\n");
                scanf("%s %s %s %f", id_client, id_compte, password, &somme);
                if (strlen(id_client) == 0 || strlen(id_compte) == 0 || strlen(password) == 0) {
                    printf("Erreur : Tous les champs doivent être remplis.\n");
                    continue;
                }
                snprintf(commande, sizeof(commande), "RETRAIT %s %s %s %.2f", id_client, id_compte, password, somme);
                break;

            case 3: // SOLDE
                printf("Entrez : id_client id_compte password\n");
                scanf("%s %s %s", id_client, id_compte, password);
                if (strlen(id_client) == 0 || strlen(id_compte) == 0 || strlen(password) == 0) {
                    printf("Erreur : Tous les champs doivent être remplis.\n");
                    continue;
                }
                snprintf(commande, sizeof(commande), "SOLDE %s %s %s", id_client, id_compte, password);
                break;

            case 4: // OPERATIONS
                printf("Entrez : id_client id_compte password\n");
                scanf("%s %s %s", id_client, id_compte, password);
                if (strlen(id_client) == 0 || strlen(id_compte) == 0 || strlen(password) == 0) {
                    printf("Erreur : Tous les champs doivent être remplis.\n");
                    continue;
                }
                snprintf(commande, sizeof(commande), "OPERATIONS %s %s %s", id_client, id_compte, password);
                printf("Commande envoyée : %s\n", commande);
                break;

            case 5: // Quitter
                printf("Au revoir !\n");
                close(sock);
                return 0;

            default:
                printf("Choix invalide, veuillez réessayer.\n");
                continue;
        }

        // Envoyer la commande au serveur
        if (send(sock, commande, strlen(commande), 0) < 0) {
            printf("Erreur lors de l'envoi de la commande.\n");
            continue;
        }

        // Lire la réponse du serveur
        printf("En attente de la réponse du serveur...\n");
        memset(buffer, 0, sizeof(buffer)); // Nettoyer le tampon
        int bytes_received = read(sock, buffer, 1024);
        if (bytes_received <= 0) {
            printf("Erreur : Aucune réponse reçue du serveur.\n");
            continue;
        }
        buffer[bytes_received] = '\0'; // Assurez que la chaîne est terminée
        printf("Réponse du serveur : %s\n", buffer);

    } while (choix != 5);

    // Fermer la connexion
    close(sock);
    return 0;
}
