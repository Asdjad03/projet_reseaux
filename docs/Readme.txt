#------ PROJET RESEAU BAKARY Asdjad -------

Ce fichier explique comment compiler et exécuter les différentes parties de l'application de gestion bancaire utilisant les protocoles TCP et UDP.

# Organisation du projet
.
├── Makefile_udp         # Compilation pour UDP uniquement
├── Makefile_tcp         # Compilation pour TCP uniquement
├── Makefile_BONUS       # Compilation pour les versions bonus
├── README.md            # Documentation principale
├── src/                 # Dossier contenant tous les codes sources
│   ├── TCP/             # Implémentations pour le protocole TCP
│   │   ├── client_tcp.c
│   │   ├── server_tcp.c
│   │   ├── header_tcp.h
│   │   ├── client_tcp_bonus.c
│   │   ├── server_tcp_bonus.c
│   │   ├── header_tcp_bonus.h
│   │   ├── utils.c
│   ├── UDP/             # Implémentations pour le protocole UDP
│   │   ├── client_udp.c
│   │   ├── server_udp.c
│   │   ├── header_udp.h
│   │   ├── utils_udp.c
├── docs/                # Documentation complémentaire
│   └── Readme.txt       # Documentation en texte simple

# Compilation
## Prérequis
Assurez-vous d'avoir GCC installé sur votre système. Sous Linux :
sudo apt update && sudo apt install build-essential

# Commandes de Compilation
## Compilation générale
Pour compiler tous les fichiers TCP et UDP, exécutez :
make 

## Compilation pour UDP uniquement
make -f Makefile_udp

## Compilation pour TCP uniquement
make -f Makefile_tcp

## Compilation pour les versions bonus
make -f Makefile_BONUS

## Nettoyage
Pour nettoyer les fichiers générés :
make clean 

# Exécution
Il faut ouvrir deux terminals différents après avoir fait l'instruction 'make'
Dans l'un, ouvrir le serveur lié et dans l'autre ouvrir le client adapté.

## Serveurs 
    ###Serveur TCP:
    ./server_tcp
    
    ###Serveur UDP:
    ./server_udp

    ###Serveur TCP Bonus:
    ./server_tcp_bonus

## Clients
    ###Client TCP:
    ./client_tcp

    ###Client UDP:
    ./client_udp

    ###Client TCP Bonus:
    ./client_tcp_bonus

# Fonctionnalités
## Authentification

###TCP
Dans le terminal où vous aurez ouvert le client, 
On demandera de s'authentifier avant de faire tout action, il faut mettre:
id_client id_compte mdp
asdjad poly ei4

###TCP bonus
Pareil que pour précédemment mais ici il sera possible de gerer differents clients en même temps:

id_client id_compte mdp
client1 compte1 pass1
client2 compte2 pass2
client3 compte3 pass3
client4 compte4 pass4
client5 compte5 pass5

###UDP
On demandera ici da faire une operation et après chaque choix l'authentification sera demandé avec ici:
id_client id_compte mdp
asdjad poly ei4

## Action disponible
Ajout Ajoutez une somme à votre compte
Retrait Retire une somme de votre compte
Solde Consultez le solde de votre compte
Operations Consultez l'historique des opérations
Quitter Quittez l'application depuis le client

#Notes
Assurez vous de toujours démarrer le serveur avant le client.
Si vous utilisez la version TCP bonus, plusieurs clients peuvent se connecter en même temps.

#Auteur 
Bakary Asdjad 
EI4 2024 2025
Polytech - Projet réseau