#include "header_tcp.h"
#include "utils.c"

// Variables globales
Client clients[MAX_COMPTES];
int nb_clients = 0;

// Fonction pour ajouter une somme sur un compte
void ajouter_somme(Compte *compte, float somme, char *reponse) {
    afficher_log("Traitement de l'opération AJOUT");

    compte->solde += somme;

    // Ajouter une opération avec la date
    char date[50];
    obtenir_heure_actuelle(date, 50);
    snprintf(compte->operations[compte->op_index].type, sizeof(compte->operations[compte->op_index].type), "AJOUT");
    compte->operations[compte->op_index].montant = somme;
    snprintf(compte->operations[compte->op_index].date, sizeof(compte->operations[compte->op_index].date), "%s", date);
    compte->op_index = (compte->op_index + 1) % MAX_OPERATIONS;
    if (compte->op_index == 0) {
        afficher_log("L'historique des opérations a atteint sa capacité maximale. Écrasement des anciennes opérations.");
    }

    snprintf(reponse, BUFFER_SIZE, "OK : %.2f € ajouté, nouveau solde : %.2f € \n", somme, compte->solde);
    afficher_log(reponse);

}

// Fonction pour retirer une somme sur un compte
void retirer_somme(Compte *compte, float somme, char *reponse) {
    afficher_log("Traitement de l'opération RETRAIT");

    if (compte->solde >= somme) {
        compte->solde -= somme;

        // Ajouter une opération avec la date
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

// Fonction pour traiter l'authentification
int authentifier_client(const char *id_client, const char *id_compte, const char *password) {
    for (int i = 0; i < nb_clients; i++) {
        if (strcmp(clients[i].id_client, id_client) == 0) {
            for (int j = 0; j < clients[i].nb_comptes; j++) {
                if (strcmp(clients[i].comptes[j].id_compte, id_compte) == 0 &&
                    strcmp(clients[i].comptes[j].password, password) == 0) {
                    return 1; // Authentification réussie
                }
            }
        }
    }
    return 0; // Authentification échouée
}

// Fonction pour traiter les requêtes d'un client
void traiter_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    char reponse[BUFFER_SIZE] = {0};

    // Étape d'authentification
    char id_client[50], id_compte[50], password[50];
    int authenticated = 0;

    afficher_log("Début de l'authentification du client.");

    // Lire les identifiants envoyés par le client
    while (!authenticated) {  // Boucle tant que l'authentification n'est pas réussie
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(client_socket, buffer, BUFFER_SIZE);

        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                printf("[LOG] Client déconnecté avant authentification.\n");
            } else {
                perror("[ERREUR] Lecture des identifiants");
            }
            close(client_socket);
            return;
        }

        buffer[bytes_read] = '\0'; // Assurer que la chaîne est terminée
        printf("[LOG] Identifiants reçus : %s\n", buffer);

        // Analyse des identifiants
        if (sscanf(buffer, "%s %s %s", id_client, id_compte, password) != 3) {
            snprintf(reponse, BUFFER_SIZE, "KO : Format des identifiants incorrect. Veuillez réessayer.");
            send(client_socket, reponse, strlen(reponse), 0);
            continue;  // Recommencer la boucle pour redemander les identifiants
        }

        // Vérification des identifiants
        if (authentifier_client(id_client, id_compte, password)) {
            snprintf(reponse, BUFFER_SIZE, "OK : Authentification réussie. Bienvenue %s sur le compte %s !", id_client, id_compte);
            afficher_log("Authentification réussie pour le client.");
            send(client_socket, reponse, strlen(reponse), 0);
            authenticated = 1;  // Sortir de la boucle d'authentification
        } else {
            snprintf(reponse, BUFFER_SIZE, "KO : Identifiants incorrects. Veuillez réessayer !");
            afficher_log("Échec de l'authentification.");
            send(client_socket, reponse, strlen(reponse), 0);
        }
    }


    // Boucle pour traiter les commandes après authentification
    while (1) {
        // Nettoyer les tampons
        memset(buffer, 0, BUFFER_SIZE);
        memset(reponse, 0, BUFFER_SIZE);

        // Lire la commande du client
        int bytes_read = read(client_socket, buffer, BUFFER_SIZE);

        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                printf("[LOG] Client déconnecté proprement.\n");
            } else {
                perror("[ERREUR] Lecture depuis le client");
            }
            close(client_socket);
            break;
        }

        buffer[bytes_read] = '\0'; // Terminer correctement la chaîne
        printf("[LOG] Commande reçue du client: %s\n", buffer);

        char operation[20], cmd_id_client[50], cmd_id_compte[50], cmd_password[50];  
        float somme = 0;
       
       // Analyser la commande complète
        int args_count = sscanf(buffer, "%s %s %s %s %f", operation, cmd_id_client, cmd_id_compte, cmd_password, &somme);

        // Vérifier que les identifiants correspondent au client authentifié
        if (strcmp(id_client, cmd_id_client) != 0 || strcmp(id_compte, cmd_id_compte) != 0 || strcmp(password, cmd_password) != 0) {
            snprintf(reponse, BUFFER_SIZE, "KO : Identifiants non valides pour cette commande.");
            send(client_socket, reponse, strlen(reponse), 0);
            continue;
        }

        // Traiter les commandes
        if (strcmp(operation, "AJOUT") == 0 && args_count == 5) {
            ajouter_somme(&clients[0].comptes[0], somme, reponse);
        } else if (strcmp(operation, "RETRAIT") == 0 && args_count == 5) {
            retirer_somme(&clients[0].comptes[0], somme, reponse);
        } else if (strcmp(operation, "SOLDE") == 0 && args_count == 4) {
            obtenir_solde(&clients[0].comptes[0], reponse);
        } else if (strcmp(operation, "OPERATIONS") == 0 && args_count == 4) {
            obtenir_operations(&clients[0].comptes[0], reponse);
        } else {
            snprintf(reponse, BUFFER_SIZE, "KO : Commande inconnue ou mal formée.");
        }

        // Terminer la réponse avec un caractère nul avant l'envoi
        reponse[strlen(reponse)] = '\0';
        send(client_socket, reponse, strlen(reponse), 0);
        //afficher_log("Réponse envoyée au client.");
    }
}

// Fonction principale
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Initialisation des clients et des comptes
    strcpy(clients[0].id_client, "asdjad");
    clients[0].nb_comptes = 1;
    strcpy(clients[0].comptes[0].id_compte, "poly");
    strcpy(clients[0].comptes[0].password, "ei4");
    clients[0].comptes[0].solde = 1000.0;
    clients[0].comptes[0].op_index = 0;

    nb_clients = 1;

    // Création du socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Erreur de création du socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erreur lors de l'association");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Erreur d'écoute");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexions sur le port %d...\n", PORT);

    // Boucle principale pour accepter et traiter les connexions clients
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Erreur d'acceptation");
            continue;
        }
        // Gestion d'un seul client à la fois
        traiter_client(new_socket);
    }

    return 0;
}
