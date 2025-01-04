//BAKARY Asdjad 
// Projet Réseau gestion bancaire
// SERVEUR_TCP : Gestion d'un seul client

#include "header_tcp_bonus.h"     //en-tête contenant les structures et prototypes
#include "utils.c"          //fonctions utilitaires

#include <pthread.h>  // Pour la gestion des threads

#define MAX_CLIENTS 10  // Nombre maximum de clients simultanés

// Variables globales
Client clients[MAX_CLIENTS]; // Tableau pour stocker les clients
int nb_clients = 5;         // Nombre actuel de clients dans le système
int client_count = 0;

void initialiser_clients() {
    for (int i = 0; i < nb_clients; i++) {
        snprintf(clients[i].id_client, sizeof(clients[i].id_client), "client%d", i + 1);
        clients[i].nb_comptes = 1; // Chaque client a un compte par défaut
        snprintf(clients[i].comptes[0].id_compte, sizeof(clients[i].comptes[0].id_compte), "compte%d", i + 1);
        snprintf(clients[i].comptes[0].password, sizeof(clients[i].comptes[0].password), "pass%d", i + 1);
        clients[i].comptes[0].solde = 100.0 * (i + 1); // Exemple : solde initial
        clients[i].comptes[0].op_index = 0; // Pas d'opérations initiales
    }
    printf("Clients initialisés avec succès.\n");
}

// Fonction pour gérer un client (exécutée dans un thread)
void *gerer_client(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    free(client_socket_ptr);  // Libérer la mémoire allouée pour le socket
    traiter_client(client_socket);  // Appel à la fonction qui gère les interactions avec le client
    close(client_socket);  // Fermer le socket une fois terminé
    client_count--;  // Décrémenter le compteur de connexions actives
    pthread_exit(NULL);    // Terminer le thread proprement
}

// Fonction pour ajouter une somme sur un compte
void ajouter_somme(int client_index, const char *id_compte, float somme, char *reponse) {
    afficher_log("Traitement de l'opération RAJOUT");
    for (int j = 0; j < clients[client_index].nb_comptes; j++) {
        if (strcmp(clients[client_index].comptes[j].id_compte, id_compte) == 0) {
            clients[client_index].comptes[j].solde += somme;

            // Ajouter une opération
            char date[50];
            obtenir_heure_actuelle(date, sizeof(date));
            snprintf(clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].type,
                     sizeof(clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].type),
                     "AJOUT");
            clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].montant = somme;
            snprintf(clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].date,
                     sizeof(clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].date),
                     "%s", date);
            clients[client_index].comptes[j].op_index = (clients[client_index].comptes[j].op_index + 1) % MAX_OPERATIONS;

            snprintf(reponse, BUFFER_SIZE, "OK : %.2f € ajouté, nouveau solde : %.2f €",
                     somme, clients[client_index].comptes[j].solde);
            return;
        }
    }
    snprintf(reponse, BUFFER_SIZE, "KO : Compte non trouvé.");
}


// Fonction pour retirer une somme sur un compte
void retirer_somme(int client_index, const char *id_compte, float somme, char *reponse) {
    afficher_log("Traitement de l'opération RETRAIT");
    // Parcourir les comptes du client pour trouver le compte correspondant
    for (int j = 0; j < clients[client_index].nb_comptes; j++) {
        if (strcmp(clients[client_index].comptes[j].id_compte, id_compte) == 0) {
            // Vérifier si le solde est suffisant
            if (clients[client_index].comptes[j].solde >= somme) {
                clients[client_index].comptes[j].solde -= somme;

                // Enregistrer l'opération
                char date[50];
                obtenir_heure_actuelle(date, sizeof(date));
                snprintf(clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].type,
                         sizeof(clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].type),
                         "RETRAIT");
                clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].montant = somme;
                snprintf(clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].date,
                         sizeof(clients[client_index].comptes[j].operations[clients[client_index].comptes[j].op_index].date),
                         "%s", date);
                clients[client_index].comptes[j].op_index = (clients[client_index].comptes[j].op_index + 1) % MAX_OPERATIONS;

                // Préparer la réponse
                snprintf(reponse, BUFFER_SIZE, "OK : %.2f € retiré, nouveau solde : %.2f €",
                         somme, clients[client_index].comptes[j].solde);
                return;
            } else {
                snprintf(reponse, BUFFER_SIZE, "KO : Solde insuffisant");
                return;
            }
        }
    }

    // Si aucun compte correspondant n'a été trouvé
    snprintf(reponse, BUFFER_SIZE, "KO : Compte non trouvé.");
}

// Fonction pour obtenir le solde d'un compte
void obtenir_solde(int client_index, const char *id_compte, char *reponse) {
    afficher_log("Traitement de l'opération SOLDE");

    // Parcourir les comptes du client
    for (int j = 0; j < clients[client_index].nb_comptes; j++) {
        if (strcmp(clients[client_index].comptes[j].id_compte, id_compte) == 0) {
            // Compte trouvé, récupérer le solde et la dernière opération
            char date_derniere_op[50] = "Aucune opération"; // Par défaut
            if (clients[client_index].comptes[j].op_index > 0) {
                snprintf(date_derniere_op, sizeof(date_derniere_op), "%s", 
                         clients[client_index].comptes[j].operations[(clients[client_index].comptes[j].op_index - 1) % MAX_OPERATIONS].date);
            }

            snprintf(reponse, BUFFER_SIZE, "RES_SOLDE : Solde : %.2f €, Dernière opération : %s",
                     clients[client_index].comptes[j].solde, date_derniere_op);
            afficher_log(reponse);
            return;
        }
    }

    // Si aucun compte correspondant n'est trouvé
    snprintf(reponse, BUFFER_SIZE, "KO : Compte non trouvé.");
    afficher_log(reponse);
}


// Fonction pour obtenir les dernières opérations
void obtenir_operations(int client_index, const char *id_compte, char *reponse) {
    afficher_log("Récupération des dernières opérations");

    // Parcourir les comptes du client
    for (int j = 0; j < clients[client_index].nb_comptes; j++) {
        if (strcmp(clients[client_index].comptes[j].id_compte, id_compte) == 0) {
            // Compte trouvé, récupérer les opérations
            snprintf(reponse, BUFFER_SIZE, "RES_OPERATIONS :\n");

            for (int i = 0; i < MAX_OPERATIONS; i++) {
                if (strlen(clients[client_index].comptes[j].operations[i].type) > 0) {
                    char ligne[100];
                    snprintf(ligne, sizeof(ligne), "%s %s %.2f\n",
                             clients[client_index].comptes[j].operations[i].type,
                             clients[client_index].comptes[j].operations[i].date,
                             clients[client_index].comptes[j].operations[i].montant);
                    strncat(reponse, ligne, BUFFER_SIZE - strlen(reponse) - 1);
                }
            }

            // Vérifier si aucune opération n'a été enregistrée
            if (strlen(reponse) == strlen("RES_OPERATIONS :\n")) {
                strncat(reponse, "Aucune opération enregistrée\n", BUFFER_SIZE - strlen(reponse) - 1);
            }

            afficher_log(reponse);
            return;
        }
    }

    // Si aucun compte correspondant n'est trouvé
    snprintf(reponse, BUFFER_SIZE, "KO : Compte non trouvé.");
    afficher_log(reponse);
}


// Fonction pour authentifier un client
int authentifier_client(const char *id_client, const char *id_compte, const char *password) {
    for (int i = 0; i < nb_clients; i++) {
        if (strcmp(clients[i].id_client, id_client) == 0) {
            for (int j = 0; j < clients[i].nb_comptes; j++) {
                if (strcmp(clients[i].comptes[j].id_compte, id_compte) == 0 &&
                    strcmp(clients[i].comptes[j].password, password) == 0) {
                    return i; // Retourne l'index du client
                }
            }
        }
    }
    return -1; // Authentification échouée
}

// Fonction pour gérer les interactions avec un client
void traiter_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    char reponse[BUFFER_SIZE] = {0};

    // Étape d'authentification
    char id_client[50], id_compte[50], password[50];
    int client_index = -1;
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
            return;     //Deconnexion ou erreur
        }

        buffer[bytes_read] = '\0'; // Assurer que la chaîne est terminée
        printf("[LOG] Identifiants reçus : %s\n", buffer);

        // Analyse des identifiants
        if (sscanf(buffer, "%s %s %s", id_client, id_compte, password) == 3) {
            client_index = authentifier_client(id_client, id_compte, password);
            if (client_index >= 0) {
                snprintf(reponse, BUFFER_SIZE, "OK : Authentification réussie !");
                authenticated = 1;
            } else {
                snprintf(reponse, BUFFER_SIZE, "KO : Authentification échouée.");
            }
        } else {
            snprintf(reponse, BUFFER_SIZE, "KO : Format invalide.");
        }
        send(client_socket, reponse, strlen(reponse), 0);
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
            ajouter_somme(client_index, id_compte, somme, reponse);        } else if (strcmp(operation, "RETRAIT") == 0 && args_count == 5) {
            retirer_somme(client_index, cmd_id_compte, somme, reponse);
        } else if (strcmp(operation, "SOLDE") == 0 && args_count == 4) {
            obtenir_solde(client_index, cmd_id_compte, reponse);
        } else if (strcmp(operation, "OPERATIONS") == 0 && args_count == 4) {
            obtenir_operations(client_index, cmd_id_compte, reponse);
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
    initialiser_clients();
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    //pthread_t threads[MAX_CLIENTS];  // Tableau pour stocker les threads

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

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Erreur d'écoute");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexions sur le port %d...\n", PORT);

    // Boucle principale pour accepter et traiter les connexions clients
    while (1) {
        // Accepter une nouvelle connexion
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Erreur d'acceptation");
            continue;
        }
        printf("[INFO] Nouvelle connexion acceptée (Socket : %d)\n", new_socket);

        // Vérification du nombre de connexions actives
        if (client_count >= MAX_CLIENTS) {
            printf("[ERREUR] Trop de connexions actives. Connexion rejetée.\n");
            close(new_socket);
            continue;  // Passer à la prochaine itération
        }
        client_count++;  // Incrémenter le compteur de connexions actives
        
        // Allouer dynamiquement de la mémoire pour le descripteur du socket
        int *client_socket_ptr = malloc(sizeof(int));
        if (client_socket_ptr == NULL) {
            perror("Erreur d'allocation de mémoire pour le socket");
            close(new_socket);
            continue;
        }
        *client_socket_ptr = new_socket;

        // Créer un thread pour gérer le client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, gerer_client, client_socket_ptr) != 0) {
            perror("Erreur lors de la création du thread");
            free(client_socket_ptr);
            close(new_socket);
            client_count--;  // Décrémenter si le thread n'a pas pu être créé
            continue;
        }

        pthread_detach(thread_id);  // Laisser le thread se nettoyer automatiquement
    }

    return 0;
}
