#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#include "fonctionsTCP.h"
#include "protocolColonne.h"
#include "validation.h"

/* taille du buffer de reception */
#define TAIL_BUF 500
#define MAX_CL 1020
/*
void lireOperation(Operation *op,TResultat *result){
switch(op->operateur){
case PLUS :
result->codeErreur = OP_OK;
result->res = op->op1 + op->op2;
break;
case MOIN:
result->codeErreur = OP_OK;
result->res = op->op1 - op->op2;
break;
case MULT:
result->codeErreur = OP_OK;
result->res = op->op1 * op->op2;
break;
case DIV:
result->codeErreur = OP_OK;
result->res = op->op1 / op->op2;
break;
}
}*/

int main(int argc, char** argv) {
	int  sockConx,        /* descripteur socket connexion */
		sockTrans,       /* descripteur socket transmission */
		sockJ1,
		sockJ2,
		port,            /* numero de port */
		sizeAddr,        /* taille de l'adresse d'une socket */
		err;	        /* code d'erreur */

	char buffer[TAIL_BUF]; /* buffer de reception */


	struct sockaddr_in nomTransmis;	/* adresse socket de transmission */
	struct sockaddr_in nomConnecte;	/* adresse de la socket connectee */
	
	char nomJ1[TNOM];
	char nomJ2[TNOM];

	TPartieReq reqJ1;
	TPartieReq reqJ2;

	int scl[MAX_CL];
	int nbsc = 0;
	int nfsd = FD_SETSIZE + 1;

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

	err = accept(err,
		(struct sockaddr *)&nomTransmis,
		(socklen_t *)&sizeAddr);
	if (err < 0) {
		perror("serveurTCP:  erreur sur accept");
		return -5;
	}

	sockJ1 = err;

	err = accept(err,
		(struct sockaddr *)&nomTransmis,
		(socklen_t *)&sizeAddr);
	if (err < 0) {
		perror("serveurTCP:  erreur sur accept");
		return -5;
	}

	sockJ2 = err;

	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(sockJ1, &readSet);
	FD_SET(sockJ2, &readSet);

	err = select(nfsd, &readSet, NULL, NULL, NULL);

	// Reception des demande de partie
	int demande = 0;
	while (demande == 0) {
		if (FD_ISSET(sockJ1, &readSet)) {
			/* Reception de la demande du j1 */
			
			err = recv(sockJ1,&reqJ1,sizeof(TPartieReq), 0);
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

			if (reqJ1.idRequest == PARTIE) {
				//nomJ1 = reqJ1.nomJoueur;
			}
			else {
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

			demande = 1;
		}
		if (FD_ISSET(sockJ1, &readSet)) {
			/* Reception de la demande du j2 */
			
			err = recv(sockJ2,&reqJ2,sizeof(TPartieReq), 0);
			if (err < 0) {
				perror("serveurTCP: erreur dans la reception");
				// envoie erreur aux joueur
				TPartieRep rep;
				rep.err = ERR_PARTIE;
				send(sockJ1, &rep, sizeof(TPartieRep), 0);
				send(sockJ2, &rep, sizeof(TPartieRep), 0);
				shutdown(sockJ2, 2); close(sockJ2);
				shutdown(sockJ1, 2); close(sockJ1);
				return -1;
			}

			if (reqJ2.idRequest == PARTIE) {
				//nomJ2 = reqJ2.nomJoueur;
			}
			else {
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

			demande = 2;
		}
	}

	if (demande == 1) {
		TPartieRep repJ1;
		repJ1.err = ERR_OK;
		repJ1.coul = BLANC;
		repJ1.nomAdvers = reqJ2.nomJoueur; //TODO
		send(sockJ1, &repJ1, sizeof(TPartieRep), 0);

		TPartieRep repJ2;
		repJ2.err = ERR_OK;
		repJ2.coul = NOIR;
		repJ2.nomAdvers = reqJ1.nomJoueur; //TODO
		send(sockJ2, &repJ2, sizeof(TPartieRep), 0);
	}
	if (demande == 2) {
		TPartieRep repJ1;
		repJ1.err = ERR_OK;
		repJ1.coul = NOIR;
		repJ1.nomAdvers = nomJ2; // TODO
		send(sockJ1, &repJ1, sizeof(TPartieRep), 0);

		TPartieRep repJ2;
		repJ2.err = ERR_OK;
		repJ2.coul = BLANC;
		repJ2.nomAdvers = nomJ1; // TOOD
		send(sockJ2, &repJ2, sizeof(TPartieRep), 0);
	}

	// Debut de la partie

	int nbCoup = 0;
	while (nbCoup < 40) {

		if (FD_ISSET(sockJ1, &readSet)) {
			/* Reception de coup pour le j1 */
			TCoupReq reqJ1;

			err = recv(sockJ1,&reqJ1,sizeof(TPartieReq), 0);
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
			int joueur;
			if (reqJ1.coul == BLANC) { joueur = 1; }
			else { joueur = 2; }
			bool res;
			res = validationCoup(joueur,reqJ1,&propCoup);

			// Reponse
			TCoupRep rep;
			rep.err = ERR_OK;
			// VALID ...
			if (res) { rep.validCoup = VALID; }

			rep.validCoup = VALID;
			rep.propCoup = propCoup;
			send(sockJ1, &rep, sizeof(TCoupRep), 0);

			nbCoup++;
		}
		if (FD_ISSET(sockJ1, &readSet)) {
			/* Reception de coup pour le j2 */
			TCoupReq reqJ2;

			err = recv(sockJ2,&reqJ2, sizeof(TPartieReq), 0);
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
			int joueur;
			if (reqJ2.coul == BLANC) { joueur = 1; }
			else { joueur = 2; }
			bool res;
			res = validationCoup(joueur, reqJ2, &propCoup);

			// Reponse
			TCoupRep rep;
			rep.err = ERR_OK;
			// VALID ...
			if(res){ rep.validCoup = VALID; }
			
			rep.propCoup = propCoup;
			send(sockJ2, &rep, sizeof(TCoupRep), 0);

			nbCoup++;
		}
		
	}

	
	// arret et fermeture
	shutdown(sockJ1, 2); close(sockJ1);
	shutdown(sockJ2, 2); close(sockJ2);
	close(sockConx);

	return 0;
}


