#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/time.h>

#include "fonctionsTCP.h"
#include "protocolColonne.h"
#include "validation.h"

/* taille du buffer de reception */
#define TIME_MAX 6


/**
 * Fonction de traitement des demandes de connexion
 * param 	int sockConx   	Socket de connexion du serveur
 * return 	int 			Retourne la valeur du descripteur de fichier du joueur connectée
 */
int traitementDemandeConnex(int sockConx){
	int err = accept(sockConx,NULL,NULL);
    if (err < 0) {
        perror("serveurTCP:  erreur sur accept");
        return -5;
    }
			
	return err;
}

/**
 * Fonction de traitement des demandes de partie par les joueurs
 * param 	int sockJ1  		Socket de connexion du joueur 1
 * param 	int sockJ2   		Socket de connexion du joueur 2 
 * param 	TPartieReq* reqJ1  	Requete de demande de partie envoyer par le joueur
 * return 	int 				Retourne -1 si il y a une erreur, 0 sinon
 */
int traitementDemandePartie(int sockJ1,int sockJ2,TPartieReq* reqJ){
	//Reception de la demande du j1 /
	TPartieRep rep;
    int err = recv(sockJ1,reqJ,sizeof(TPartieReq), 0);
    if (err < 0) {
        perror("serveurTCP: erreur dans la reception");
        // envoie erreur aux joueur
        rep.err = ERR_PARTIE;
        send(sockJ1, &rep, sizeof(TPartieRep), 0);
        send(sockJ2, &rep, sizeof(TPartieRep), 0);
        shutdown(sockJ1, 2); close(sockJ1);
        shutdown(sockJ2, 2); close(sockJ2);
        return -1;
    }
	
    if (reqJ->idRequest != PARTIE) {       
        perror("serveurTCP: erreur dans la reception");
        // envoie erreur aux joueur
        rep.err = ERR_TYP;
        send(sockJ1, &rep, sizeof(TPartieRep), 0);
        send(sockJ2, &rep, sizeof(TPartieRep), 0);
        shutdown(sockJ1, 2); close(sockJ1);
        shutdown(sockJ2, 2); close(sockJ2);
        return -1;
    }
	
	return 0;
}

/**
 * Fonction de reponse à la suite des demandes de partie faites par les joueurs
 * param 	int sockJ1  		Socket de connexion du joueur 1
 * param 	int sockJ2   		Socket de connexion du joueur 2 
 * param 	int demande  		Joueur ayant envoyer la demande en premier 
 * param 	TPartieReq* reqJ1  	Requete de demande de partie envoyer par le joueur 1
 * param 	TPartieReq* reqJ1  	Requete de demande de partie envoyer par le joueur 2
 * return 	void
 */
void envoiCouleur(int sockJ1,int sockJ2,int demande,TPartieReq* reqJ1, TPartieReq* reqJ2){
	// Envoie des couleurs aux joueurs
	TPartieRep repJ1;
	TPartieRep repJ2;
	
    if (demande == 1) {
        
        repJ1.err = ERR_OK;
        repJ1.coul = BLANC;	
		memcpy(repJ1.nomAdvers,reqJ2->nomJoueur ,sizeof repJ1.nomAdvers);
        send(sockJ1, &repJ1, sizeof(TPartieRep), 0);
        
        repJ2.err = ERR_OK;
        repJ2.coul = NOIR;
		
		memcpy(repJ2.nomAdvers,reqJ1->nomJoueur ,sizeof repJ2.nomAdvers);
        send(sockJ2, &repJ2, sizeof(TPartieRep), 0);
    }
    if (demande == 2) {
        repJ2.err = ERR_OK;
        repJ2.coul = BLANC;		
		memcpy(repJ2.nomAdvers,reqJ1->nomJoueur ,sizeof repJ2.nomAdvers);
        send(sockJ2, &repJ2, sizeof(TPartieRep), 0);
		
        repJ1.err = ERR_OK;
        repJ1.coul = NOIR;

		memcpy(repJ1.nomAdvers,reqJ2->nomJoueur ,sizeof repJ1.nomAdvers);
        send(sockJ1, &repJ1, sizeof(TPartieRep), 0);   
    }
	
}

/**
 * Fonction de traitement des coups envoyer par les joueurs
 * Gestion du delai maximale de reponse (TIMEOUT)
 * Envoie des reponses au joueurs, affiche le resultat des parties
 * param 	int sockJ1  		Socket de connexion du joueur qui envoie le coup
 * param 	int sockJ2   		Socket de connexion du joueur qui attent le coup 
 * param 	int coul 			Couleur du joueur qui envoie le coup
 * param 	bool timeout		Indique si le joueur qui envoie le coup a mis trop de temps a depasser le temps autorisé
 * param 	TPropCoup* pc		Parametre de sortie du resultat de la partie(utiliser pour calculer le score)
 * return 	bool 				Retourne vrai si la partie courrente est terminer
 */
bool traitementCoup(int sockJ1,int sockJ2,int coul,bool timeout,TCoupRep* pc){
	int err;
	TCoupReq reqJ1;
	TPropCoup propCoup;
	TCoupRep rep;
	bool fini = false;			
	bool res;

	err = recv(sockJ1,&reqJ1,sizeof(TCoupReq), 0);
	printf("Coup du Joueur %d reçu \n",coul);
				
	if (err < 0) {
	  perror("serveurTCP: erreur dans la reception");
	  TPartieRep rep;
	  rep.err = ERR_TYP;
	  send(sockJ1, &rep, sizeof(TPartieRep), 0);
	  send(sockJ2, &rep, sizeof(TPartieRep), 0);
	  shutdown(sockJ1, 2); close(sockJ1);
	  shutdown(sockJ2, 2); close(sockJ2);
	  return -1;
	}
				
	// Traitement
	
	rep.err = ERR_OK;
	
	if(timeout){
		// On fait perdre le joueur qui a fait un timeout
		// pour réinitialiser validationCoup afin de jouer la partie suivante
		// sans que la fonction attende la suite de la partie
		reqJ1.action.posPion.lg = 4;
		res = validationCoup(coul,reqJ1,&propCoup);
		printf("TIMEOUT : Le joueur %d n'a pas répondue à temps : Il a perdu la partie\n",coul);
		rep.validCoup = TIMEOUT; rep.propCoup = PERDU; fini = true;
		pc->propCoup = PERDU;
	}
	else{
		res = validationCoup(coul,reqJ1,&propCoup);

		if (res) { 
			rep.validCoup = VALID; 
			printf("Coup du Joueur %d Validé \n",coul);
			if(propCoup == GAGNE){
				pc->propCoup = GAGNE;
				printf("Le Joueur %d à GAGNÉ \n",coul);
			}
			if(propCoup == NULLE){
				pc->propCoup = NULLE;
				printf("Il y a match nulle !\n");
			}
		}
		if (!res) { 
			rep.validCoup = TRICHE; 
			printf("Coup du Joueur %d NON Validé \n",coul);
			if(propCoup == PERDU){
				pc->propCoup = PERDU;
				printf("Le Joueur %d à PERDU \n",coul);
			}
		}
	}
	if(propCoup != CONT){
		fini = true;
	}
				

	// Envoie des Reponses
	rep.propCoup = propCoup;
	send(sockJ1, &rep, sizeof(TCoupRep), 0);
	send(sockJ2, &rep, sizeof(TCoupRep), 0);
	if(!fini || timeout){send(sockJ2, &reqJ1, sizeof(TCoupReq), 0);}  
	printf("Réponse envoyer aux Joueurs\n");
	
	return fini;
	
}

/**
 * Fonction qui affiche les scores des joueurs
 * param 	int scoreJ1  		Score du joueur 1
 * param 	int scoreJ2   		Score du joueur 2 
 * param 	int demande  		Joueur ayant envoyer la demande en premier 
 * param 	TPartieReq* reqJ1  	Requete de demande de partie envoyer par le joueur 1
 * param 	TPartieReq* reqJ1  	Requete de demande de partie envoyer par le joueur 2
 * return 	void
 */
void affichageScore(int scoreJ1,int scoreJ2,int demande,TPartieReq* reqJ1, TPartieReq* reqJ2){
	if(demande == 1){
		printf("Score joueur %s : %d partie(s) gagné\n",reqJ1->nomJoueur,scoreJ1);
		printf("Score joueur %s : %d partie(s) gagné\n",reqJ2->nomJoueur,scoreJ2);
	}
	if(demande == 2){
		printf("Score joueur %s : %d partie(s) gagné\n",reqJ1->nomJoueur,scoreJ2);
		printf("Score joueur %s : %d partie(s) gagné\n",reqJ2->nomJoueur,scoreJ1);
	}
	
}

int main(int argc, char** argv) {
    int  sockConx,       /* descripteur socket connexion */
        sockJ1=0,		 /* descripteur socket transmission du premier connecté*/
        sockJ2=0,		 /* descripteur socket transmission du deuxieme connecté*/
        port,            /* numero de port */
        sizeAddr,        /* taille de l'adresse d'une socket */
        err;             /* code d'erreur */

    TPartieReq reqJ1; 	 /* Requete de demande du joueur 1 */
    TPartieReq reqJ2; 	 /* Requete de demande du joueur 2 */

    int nfsd = 3;		 /* Nombre de descripteur déjà utilisé */
	struct timeval tv = { TIME_MAX, 0 }; /* Parametre Timeout du select */
	int nbPartie = 0;    /* Compteur du nombre de partie joué */
	
	int coulJ1 = 0;		 /* Couleur du joueur 1 */
	int coulJ2 = 0;		 /* Couleur du joueur 2 */
	int scoreJ1 = 0;	 /* Score du joueur 1 */
	int scoreJ2 = 0;	 /* Score du joueur 2 */
    
    // Vérification des arguments ( Numero de port )
    if (argc != 2) {
        printf("usage : serveurTCP no_port\n");
        return -1;
    }

    port = atoi(argv[1]);
    sizeAddr = sizeof(struct sockaddr_in);

    err = socketServeur(port);
    if (err < 0) {
        printf("Erreur\n");
        exit(1);
    }
    sockConx = err;

    // Reception des demandes de connexion

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sockConx, &readSet);
    nfsd ++;
	int cpt = 0; /* Compteur du nombre de joueur connecté */
    int demande = 0; /* Joueur qui a demandé la partie en premier*/
    int num = 0; /* Compteur du nombre de demande de partie reçu */
	err = select(nfsd, &readSet, NULL, NULL, NULL);
	if (err < 0) {
        perror("serveurTCP:  erreur sur select");
        return -5;
    }
	printf("\n");
	printf("Attente des demande de connexions\n");
	printf("\n");
	
    while (cpt < 2) { // Boucle de connexion

        if (FD_ISSET(sockConx, &readSet) && num<2) { // On accepte que deux accept
		
            
			num++; // Increment du nbr d'accept
			if (num == 1) {
				sockJ1 = traitementDemandeConnex(sockConx);
				nfsd++;
				printf("J1 Connecté numero de descripteur : %d\n",sockJ1);
			}
			
			if (num == 2) {
				sockJ2 = traitementDemandeConnex(sockConx);
				nfsd++;
				printf("J2 Connecté numero de descripteur : %d\n",sockJ2);
			}
			
        }
		if(num == 1){
			FD_SET(sockConx, &readSet);
			FD_SET(sockJ1, &readSet);
		}
		if(num == 2){
			FD_SET(sockConx, &readSet);
			FD_SET(sockJ2, &readSet);
		}
		err = select(nfsd, &readSet, NULL, NULL, NULL);
		if (err < 0) {
			perror("serveurTCP:  erreur sur select");
			return -5;
		}		
		
		// Reception des demande de partie
        if(FD_ISSET(sockJ1, &readSet)){
            //Reception de la demande du J1 
            
			traitementDemandePartie(sockJ1,sockJ2,&reqJ1);
			cpt++;
			if(demande != 2){
				demande = 1;
			}
			printf("J1 partie demandee\n");
        }
		
        if (FD_ISSET(sockJ2, &readSet)) {
            // Reception de la demande du J2 
            
			traitementDemandePartie(sockJ2,sockJ1,&reqJ2);
			cpt++;
			if(demande != 1){
				demande = 2;
			}
			printf("J2 partie demandee\n");
        }
			
	}

    // Envoie des couleurs aux joueurs
    envoiCouleur(sockJ1,sockJ2,demande,&reqJ1,&reqJ2);
	
	if(demande == 1){
		coulJ1 = 1;
		coulJ2 = 2;
	}
	if(demande == 2){
		printf("demande = %d",demande);
		int tmp = sockJ1;
		sockJ1 = sockJ2;
		sockJ2 = tmp;
		coulJ1 = 2;
		coulJ2 = 1;
	}

    // Debut de la partie
	printf("\n");
	printf("Debut de la partie\n");
	printf("\n");
    
	
	while(nbPartie < 2){
		bool fini = false;
		bool timeout = false;
		int nbCoup = 0;
		while (nbCoup < 40 && !fini) {

			// Initialisation du select avec un timeout de 6 secondes. 
			tv.tv_sec = TIME_MAX;
			tv.tv_usec = 0;
			
			FD_SET(sockJ1, &readSet);
			FD_SET(sockJ2, &readSet);
			err = select(nfsd, &readSet, NULL, NULL, &tv);
			if (err <= 0) {
				// Vérification si il y a eu un timeout
				timeout = true;
			}
			

			if (FD_ISSET(sockJ1, &readSet) && !fini) {
				// Reception de coup pour le j1
				TCoupRep pc;
				pc.propCoup = CONT;
				printf("\t \t \t \tCoup J%d\n",coulJ1);  
				fini = traitementCoup(sockJ1,sockJ2,coulJ1,timeout,&pc);
				timeout = false;
				nbCoup++;
				if(pc.propCoup == GAGNE){scoreJ1++;}
				if(pc.propCoup == PERDU){scoreJ2++;}
			}
			if (FD_ISSET(sockJ2, &readSet) && !fini) {
				// Reception de coup pour le j2 
				TCoupRep pc;
				pc.propCoup = CONT;
				printf("\t \t \t \tCoup J%d\n",coulJ2);
				fini = traitementCoup(sockJ2,sockJ1,coulJ2,timeout,&pc);
				timeout = false;
				nbCoup++;
				if(pc.propCoup == GAGNE){scoreJ2++;}
				if(pc.propCoup == PERDU){scoreJ1++;}
			}
			
		}
			
		nbPartie++;
		if(nbPartie == 1){
			printf("\n");
			printf("Partie 1 FINI \n");
			printf("\n");
			printf("Debut partie 2 \n");
			printf("\n");
			// Le deuxieme de la premiere partie commence en premier
			// Echange des couleurs
			
			if(demande == 1){
				coulJ1 = 2;
				coulJ2 = 1;
			}
			
			if(demande == 2){
				coulJ1 = 1;
				coulJ2 = 2;
			}
			
		}
		
	}

    affichageScore(scoreJ1,scoreJ2,demande,&reqJ1,&reqJ2);
    // arret et fermeture
    shutdown(sockJ1, 2); close(sockJ1);
    shutdown(sockJ2, 2); close(sockJ2);
    close(sockConx);

    return 0;
}

