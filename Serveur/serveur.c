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
#define TAIL_BUF 500
#define MAX_CL 1020
#define TIME_MAX 6


void transformeNom(char* p, TPartieRep* rep) {
    int i = 0;
    //for (i = 0; i < TNOM; i++) {
        //rep->nomAdvers = p;
		//printf("%c\n",*p);
        //p++;
        //np++;
    //}
	/*
	while(i< TNOM){
		printf("%c\n",p[i]);
		i++;
		p++;
	}*/
	//rep->nomAdvers = p;

}


int traitementDemandeConnex(int sockConx){
	int err = accept(sockConx,NULL,NULL);
    if (err < 0) {
        perror("serveurTCP:  erreur sur accept");
        return -5;
    }
			
	return err;
}

int traitementDemandePartie(int sockJ1,int sockJ2,TPartieReq* reqJ1){
	//Reception de la demande du j1 /
    int err = recv(sockJ1,reqJ1,sizeof(TPartieReq), 0);
    if (err < 0) {
        perror("serveurTCP: erreur dans la reception");
        // envoie erreur aux joueur
        TPartieRep rep;
        rep.err = ERR_PARTIE;
        send(sockJ1, &rep, sizeof(TPartieRep), 0);
        send(sockJ2, &rep, sizeof(TPartieRep), 0);
        shutdown(sockJ1, 2); close(sockJ1);
        shutdown(sockJ2, 2); close(sockJ2);
        return -1;
    }
	
	//printf("%s\n",reqJ1->nomJoueur);

    if (reqJ1->idRequest != PARTIE) {
                
        perror("serveurTCP: erreur dans la reception");
        // envoie erreur aux joueur
        TPartieRep rep;
        rep.err = ERR_TYP;
        send(sockJ1, &rep, sizeof(TPartieRep), 0);
        send(sockJ2, &rep, sizeof(TPartieRep), 0);
        shutdown(sockJ1, 2); close(sockJ1);
        shutdown(sockJ2, 2); close(sockJ2);
        return -1;
    }
	
	return 0;
}

void envoiCouleur(int sockJ1,int sockJ2,int demande,TPartieReq* reqJ1, TPartieReq* reqJ2){
	// Envoie des couleurs aux joueurs

    if (demande == 1) {
        TPartieRep repJ1;
        repJ1.err = ERR_OK;
        repJ1.coul = BLANC;
		
		memcpy(repJ1.nomAdvers,reqJ2->nomJoueur ,sizeof repJ1.nomAdvers);
	
        send(sockJ1, &repJ1, sizeof(TPartieRep), 0);

        TPartieRep repJ2;
        repJ2.err = ERR_OK;
        repJ2.coul = NOIR;
		
		memcpy(repJ2.nomAdvers,reqJ1->nomJoueur ,sizeof repJ2.nomAdvers);
        send(sockJ2, &repJ2, sizeof(TPartieRep), 0);
    }
    if (demande == 2) {
		TPartieRep repJ2;
        repJ2.err = ERR_OK;
        repJ2.coul = BLANC;
		
		memcpy(repJ2.nomAdvers,reqJ1->nomJoueur ,sizeof repJ2.nomAdvers);

        send(sockJ2, &repJ2, sizeof(TPartieRep), 0);
		
        TPartieRep repJ1;
        repJ1.err = ERR_OK;
        repJ1.coul = NOIR;

		memcpy(repJ1.nomAdvers,reqJ2->nomJoueur ,sizeof repJ1.nomAdvers);

        send(sockJ1, &repJ1, sizeof(TPartieRep), 0);   
    }
	
}

bool traitementCoup(int sockJ1,int sockJ2,int coul,bool timeout){
	//printf(" Activité SockJ1 \n");
	//printf(" Partie %d\n",nbPartie+1);
				
	int err;
	TCoupReq reqJ1;

	err = recv(sockJ1,&reqJ1,sizeof(TCoupReq), 0);
	//printf("\t Coup Joueur 1 reçu \n");
				
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
	TPropCoup propCoup;
	TCoupRep rep;
	rep.err = ERR_OK;
	bool fini = false;
			
	bool res;
	printf("Joueuer : %d\n",coul);
	res = validationCoup(coul,reqJ1,&propCoup);
	if(propCoup != CONT){
		fini = true;
	}
				

	// Reponse
	
	// Test TIMEOUT , VALIDITER
	if (res) { rep.validCoup = VALID; }
	if (!res) { rep.validCoup = TRICHE; }
	rep.propCoup = propCoup;
	if (timeout){ rep.validCoup = TIMEOUT; rep.propCoup = PERDU; fini = true;}
	
	
	send(sockJ1, &rep, sizeof(TCoupRep), 0);
	send(sockJ2, &rep, sizeof(TCoupRep), 0);
	if(!fini || timeout){send(sockJ2, &reqJ1, sizeof(TCoupReq), 0);}  
	
	return fini;
	
}

int main(int argc, char** argv) {
    int  sockConx,        /* descripteur socket connexion */
        sockTrans,       /* descripteur socket transmission */
        sockJ1=0,
        sockJ2=0,
        port,            /* numero de port */
        sizeAddr,        /* taille de l'adresse d'une socket */
        err;            /* code d'erreur */

    char buffer[TAIL_BUF]; /* buffer de reception */


    struct sockaddr_in nomTransmis;    /* adresse socket de transmission */
    struct sockaddr_in nomConnecte;    /* adresse de la socket connectee */

    
    
    char nomJ1[TNOM];
    char nomJ2[TNOM];

    TPartieReq reqJ1;
    TPartieReq reqJ2;

    int scl[MAX_CL];
    int nbsc = 0;
    int nfsd = 3;

    /*
    * verification des arguments
    */
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
	int cpt = 0;
    int demande = 0;
    int num = 0;
	err = select(nfsd, &readSet, NULL, NULL, NULL);
	if (err < 0) {
        perror("serveurTCP:  erreur sur select");
        return -5;
    }
	printf("\n");
	printf("\n");
	printf("Attente des demande de connexions\n");
	printf("\n");
	
    while (cpt < 2) { // Boucle de connecion

        if (FD_ISSET(sockConx, &readSet) && num<2) { // On accepte que deux accept
		
            
			num++; // Increment du nbr d'accept
			if (num == 1) {
				sockJ1 = traitementDemandeConnex(sockConx);
				nfsd++;
				printf("J1 Connecté %d\n",sockJ1);
			}
			
			if (num == 2) {
				sockJ2 = traitementDemandeConnex(sockConx);
				nfsd++;
				printf("J2 Connecté %d\n",sockJ2);
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
		//printf("%d\n",num);
		
		
		
		// Reception des demande de partie
        if(FD_ISSET(sockJ1, &readSet)){
            //Reception de la demande du j1 /
            
			traitementDemandePartie(sockJ1,sockJ2,&reqJ1);
			
			cpt++;
			if(demande != 2){
				demande = 1;
			}
			printf("J1 partie demandee\n");
        }
		
        if (FD_ISSET(sockJ2, &readSet)) {
            // Reception de la demande du j2 /
            
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
	int coulJ1 = -1;
	int coulJ2 = -1;
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
	printf("\n");
	printf("Debut de la partie\n");
	printf("\n");
    
	struct timeval tv = { 6, 0 }; // Parametre Timeout du select
	
	
	int nbPartie = 0;
	while(nbPartie < 2){
	
		bool fini = false;
		bool timeout = false;
		int nbCoup = 0;
		while (nbCoup < 40 && !fini) {

			// Faire plus de fonction pour alleger le main().
			// Init select avec timeout dans un timeval. // Init struct timeval tv entree/sortie  à 6s, dans le retour il nous dit si le temps est dépasser (Pas besoin de cst: TIME_MAX). 
			// Si TIMEOUT -> recup quand meme les infos de connex mais on renvoi TIMEOUT ( il a perdu ).
			tv.tv_sec = 6;
			tv.tv_usec = 0;
			
			FD_SET(sockJ1, &readSet);
			FD_SET(sockJ2, &readSet);
			err = select(nfsd, &readSet, NULL, NULL, &tv);
			if (err <= 0) {
				printf("TIMEOUT\n");
				timeout = true;
			}
			

			if (FD_ISSET(sockJ1, &readSet) && !fini) {
				// Reception de coup 
				printf("\t Coup J%d\n",coulJ1);  
				fini = traitementCoup(sockJ1,sockJ2,coulJ1,timeout);
				timeout = false;
				nbCoup++;
			}
			if (FD_ISSET(sockJ2, &readSet) && !fini) {
				// Reception de coup pour le j2 
				printf("\t Coup J%d\n",coulJ2);
				fini = traitementCoup(sockJ2,sockJ1,coulJ2,timeout);
				timeout = false;
				nbCoup++;
			}
			
		}
		
		
		
		
		nbPartie++;
		if(nbPartie == 1){
			printf("\n");
			printf("\n");
			printf("\n");
			printf("\n");
			
			printf("Partie 1 FINI \n");
			
			printf("\n");
			printf("\n");
			printf("\n");
			printf("\n");
			printf("Debut partie 2 \n");
			printf("\n");
			printf("\n");
			printf("\n");
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

    
    // arret et fermeture
    shutdown(sockJ1, 2); close(sockJ1);
    shutdown(sockJ2, 2); close(sockJ2);
    close(sockConx);

    return 0;
}

