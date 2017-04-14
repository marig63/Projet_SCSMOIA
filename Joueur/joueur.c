#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "fonctionsTCP.h"
#include "protocolColonne.h"


//gere les cases
void gestionTCase(TCase cases,int* ligne,char* col){
	switch(cases.col){
			case A:
				*ligne = cases.lg+1;
				*col = 'A';
			break;

			case B:
				*ligne = cases.lg+1;
				*col = 'B';
			break;

			case C:
				*ligne = cases.lg+1;
				*col = 'C';
			break;
	}
}

//gere les coup de l'adverse 
void gestionCoupAdverse(TCoupReq repCoup){
	int ligne;
	char col;
	if(repCoup.typeCoup == POS_PION ){
		gestionTCase(repCoup.action.posPion,&ligne,&col);
		printf("Coup de l'adversaire: Positionnement en %d%c\n",ligne,col);
	}else{
		if(repCoup.typeCoup == DEPL_PION){
			gestionTCase(repCoup.action.deplPion.caseDep,&ligne,&col);
			printf("Coup de l'adversaire: Deplacement");
			printf("De %d%c",ligne,col);
			gestionTCase(repCoup.action.deplPion.caseArr,&ligne,&col);
			printf("Vers %d%c\n",ligne,col);
		}
	}
}

void creationCoup(TCoupReq* coup,int couleur){
	
	//demande coup a l'ia

	coup->idRequest = COUP;
	if(couleur == 1){
		coup->coul = BLANC;
	}else{
		coup->coul = NOIR;
	}

	//choix entre positionnement et deplacement suivant l'ia
	coup->typeCoup = POS_PION;
	
	TCase cases;
	if(couleur == 1){
		cases.lg = UN;
	}else{
		cases.lg = DEUX;
	}
	cases.col = A;

	coup->action.posPion = cases;
}

void affichageMessFin(TCoupRep repCoup){
	//affichage du resultat de la partie
	switch(repCoup.propCoup){
		case GAGNE:
			printf("Vous avez gagné\n");
		break;

		case PERDU:
			printf("Vous avez perdu\n");
		break;

		case NULLE:
			printf("Match nulle\n");
		break;
	}
}

int main(int argc, char **argv) {

  int sock,                /* descripteur de la socket locale */
      port,                /* variables de lecture */
      err;                 /* code d'erreur */
  char* nomMachine;
	char* nomJoueur;
	int nbCarac =0;
	char nom[TNOM];
	int couleurPion;
	TCoupReq coup;
	TCoupReq coupAdverse;
	TCoupRep repCoup;
	TCoupRep repCoupAdverse;
	int continu = 0;
	
	/* verification des arguments */
  if (argc != 4) {
    printf("usage : client nom_machine no_port nomJoueur\n");
    exit(1);
  }

	//Récupere le nom du joueur et vérfier qu'il ne soit pas trop long
	nomJoueur	= argv[3];
	while(nomJoueur[nbCarac]!='\0')
    ++nbCarac;

	if (nbCarac > TNOM-1) {
    printf("nom du joueur trop long\n");
    exit(2);
  }

  nomMachine = argv[1];
  port = atoi(argv[2]);

  err = socketClient(nomMachine , port);
  
  if(err<0){
	  // Erreur
	  exit(3);
  }

  sock = err;

	//Création de la requete partie
	int nbPartie = 0;
	TPartieReq partie;
	partie.idRequest = PARTIE;

	int t=0;
	while(t <= nbCarac){
		partie.nomJoueur[t] = nomJoueur[t];
		t++;
	}


	/*----------------*/

	//envoie demande partie
	err = send(sock, &partie, sizeof(TPartieReq), 0);

	if(err <= 0){
		printf("Erreur send demande de partie\n");
		exit(4);
	}

	TPartieRep repPartie;
 
	//reception de la validation 
	err = recv(sock,&repPartie,sizeof(TPartieRep),0);
	if(repPartie.err == ERR_OK){
			printf("nom du joueur adverse: %s\n",repPartie.nomAdvers);

			if(repPartie.coul == BLANC){
				printf("Possede les pion blancs\n");
				couleurPion = 1;
			}else{
				printf("Possede les pion noir\n");
				couleurPion = 2;
			}
	}
	else{
		printf("Erreur \n");
	}	

	//envoie coup
	//demande coup a l'IA
	while(nbPartie < 2){

	if((couleurPion == 2 && nbPartie == 0) || (couleurPion == 1 && nbPartie == 1)){

		//reception de la validation du coup adverse
		err = recv(sock,&repCoupAdverse,sizeof(TCoupRep),0);
		if(err <= 0){
			printf("Erreur reception validation du coup adverse 1\n");
			exit(5);
		}

		if(repCoupAdverse.err == ERR_OK && repCoupAdverse.propCoup == CONT){
			//reception du coup
			err = recv(sock,&coupAdverse,sizeof(TCoupReq),0);

			if(err <= 0){
				printf("Erreur lors de la reception du coup adverse 2 \n");
				exit(6);
			}
			gestionCoupAdverse(coupAdverse);
			continu = 1;
		}
	}

	if(continu == 1 || couleurPion == 1 || nbPartie == 1){

	do{

		//envoie du prochain coup
		creationCoup(&coup,couleurPion);
		err = send(sock, &coup, sizeof(TCoupReq), 0);
		if(err <= 0){
			printf("Erreur send d'envoie du coup\n");
			exit(7);
		}

	//reception de la validation du coup
	err = recv(sock,&repCoup,sizeof(TCoupRep),0);
		if(err <= 0){
			printf("Erreur reception validation de mon coup 3\n");
			exit(8);
		}

	if(repCoup.err != ERR_OK && repCoup.validCoup != VALID){
		printf("Erreur coup invalide\n");
		break;
	}

	if(repCoup.propCoup == CONT){
	//reception coup adverse
		//reception de la validation du coup adverse
		err = recv(sock,&repCoupAdverse,sizeof(TCoupRep),0);
		if(err <= 0){
			printf("Erreur reception validation du coup adverse 2\n");
			exit(9);
		}

		if(repCoupAdverse.err == ERR_OK && repCoupAdverse.propCoup == CONT){
			//reception du coup
			err = recv(sock,&coupAdverse,sizeof(TCoupReq),0);

			if(err <= 0){
				printf("Erreur lors de la reception du coup adverse\n");
				exit(10);
			}
			gestionCoupAdverse(coupAdverse);
			continu = 1;
		}
	}
	}while(repCoupAdverse.propCoup == CONT && repCoup.propCoup == CONT);
	nbPartie++;
	}

	if(repCoup.propCoup == GAGNE){
		printf("Vous avez gagné\n");
	}else{
		if(repCoup.propCoup == NULLE){
			printf("Match nulle\n");
		}else{
			printf("Vous avez perdu\n");
		}
	}

	//affichageMessFin(repCoup);	

	/*-----------------*/
	}

}



