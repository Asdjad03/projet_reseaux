//BAKARY Asdjad 
// Projet Réseau gestion bancaire
// CLIENT_UDP 1 client 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <strings.h> 
#include <stdbool.h> 

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 2048
#define MAX_FIELD_SIZE 50

void afficher_menu() {
    printf("\n==================== MENU CLIENT ====================\n");
    printf("1. AJOUT\n");
    printf("2. RETRAIT\n");
    printf("3. SOLDE\n");
    printf("4. OPERATIONS\n");
    printf("5. QUITTER\n");
    printf("====================================================\n");
    printf("Choisissez une commande (1-5) : ");
}

void demander_authentification(char *id_client, char *id_compte, char *password) {
    printf("Entrez votre ID client : ");
    fgets(id_client, MAX_FIELD_SIZE, stdin);
    id_client[strcspn(id_client, "\n")] = '\0';

    printf("Entrez votre ID compte : ");
    fgets(id_compte, MAX_FIELD_SIZE, stdin);
    id_compte[strcspn(id_compte, "\n")] = '\0';

    printf("Entrez votre mot de passe : ");
    fgets(password, MAX_FIELD_SIZE, stdin);
    password[strcspn(password, "\n")] = '\0';
}

void demander_montant(float *montant) {
    printf("Entrez le montant : ");
    scanf("%f", montant);
    getchar(); // Consomme le caractère '\n' laissé par scanf
}

void communiquer_avec_serveur(int client_fd, struct sockaddr_in *server_addr, const char *commande) {
    char response[BUFFER_SIZE];
    socklen_t addr_len = sizeof(*server_addr);

    // Envoyer la commande au serveur
    if (sendto(client_fd, commande, strlen(commande), 0, (struct sockaddr *)server_addr, addr_len) < 0) {
        perror("Erreur lors de l'envoi de la commande");
        return;
    }

    // Réception de la réponse du serveur
    int n = recvfrom(client_fd, response, BUFFER_SIZE, 0, (struct sockaddr *)server_addr, &addr_len);
    if (n < 0) {
        perror("Erreur lors de la réception de la réponse");
        return;
    }
    response[n] = '\0';

    // Afficher la réponse du serveur
    printf("\n----------------- RÉPONSE DU SERVEUR ----------------\n");
    printf("%s\n", response);
    printf("----------------------------------------------------\n");
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char id_client[MAX_FIELD_SIZE], id_compte[MAX_FIELD_SIZE], password[MAX_FIELD_SIZE];
    float montant;

    // Création du socket
    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Adresse serveur invalide");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Client prêt. Connecté au serveur %s:%d\n", SERVER_IP, SERVER_PORT);

    while (true) {
        afficher_menu();

        // Lecture de la commande utilisateur
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Retire le caractère de nouvelle ligne

        // Vérification du contenu de la commande
        if (strcmp(buffer, "5") == 0) {
            printf("[INFO] Au revoir! À très bientôt chez BAK!.\n");
            break;
        }

        // Traiter les commandes
        if (strcmp(buffer, "1") == 0) { // AJOUT
            demander_authentification(id_client, id_compte, password);
            demander_montant(&montant);
            snprintf(buffer, BUFFER_SIZE, "AJOUT %s %s %s %.2f", id_client, id_compte, password, montant);
        } else if (strcmp(buffer, "2") == 0) { // RETRAIT
            demander_authentification(id_client, id_compte, password);
            demander_montant(&montant);
            snprintf(buffer, BUFFER_SIZE, "RETRAIT %s %s %s %.2f", id_client, id_compte, password, montant);
        } else if (strcmp(buffer, "3") == 0) { // SOLDE
            demander_authentification(id_client, id_compte, password);
            snprintf(buffer, BUFFER_SIZE, "SOLDE %s %s %s", id_client, id_compte, password);
        } else if (strcmp(buffer, "4") == 0) { // OPERATIONS
            demander_authentification(id_client, id_compte, password);
            snprintf(buffer, BUFFER_SIZE, "OPERATIONS %s %s %s", id_client, id_compte, password);
        } else {
            printf("[ERREUR] Commande invalide.\n");
            continue;
        }

        // Appeler la fonction de communication
        communiquer_avec_serveur(client_fd, &server_addr, buffer);
    }

    close(client_fd);
    return 0;
}
