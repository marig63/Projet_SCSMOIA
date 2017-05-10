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

void creationTCase(TCase* cases,int numCase){
	int colCase = (numCase + 3)%3;
	int ligneCase = (numCase - 1)/3;
	switch(colCase){
		case 0:
			cases->col = C;
			break;
		case 1:
				cases->col = A;
				break;
		case 2:
				cases->col = B;
				break;
	}

	switch(ligneCase){
		case 0:
			cases->lg = UN;
			break;
		case 1:
				cases->lg = DEUX;
				break;
		case 2:
				cases->lg = TROIS;
				break;
	}
}

int creationNumCase(TCase cases){
	int res;
	switch(cases.col){
		case A:
			switch(cases.lg){
				case UN:
					res = 1;
					break;

				case DEUX:
					res = 4;	
					break;
				case TROIS:
					res = 7;	
					break;
			}
			break;
		case B:
				switch(cases.lg){
				case UN:
					res = 2;
					break;

				case DEUX:
					res = 5;	
					break;
				case TROIS:
					res = 8;	
					break;
			}
				break;
		case C:
				switch(cases.lg){
				case UN:
					res = 3;
					break;

				case DEUX:
					res = 6;	
					break;
				case TROIS:
					res = 9;	
					break;
			}
	}
	return res;
}
//gere les coup de l'adverse 
void gestionCoupAdverse(TCoupReq repCoup,int sockJava){
	int ligne;
	char col;
	int caseDep,caseArr,err;
	if(repCoup.typeCoup == POS_PION ){
		gestionTCase(repCoup.action.posPion,&ligne,&col);
		printf("Coup de l'adversaire: Positionnement en %d%c\n",ligne,col);
		caseDep = creationNumCase(repCoup.action.posPion);
		caseArr = caseDep;
	}else{
		if(repCoup.typeCoup == DEPL_PION){
			gestionTCase(repCoup.action.deplPion.caseDep,&ligne,&col);
			printf("Coup de l'adversaire: Deplacement");
			printf("De %d%c",ligne,col);
			gestionTCase(repCoup.action.deplPion.caseArr,&ligne,&col);
			printf("Vers %d%c\n",ligne,col);
			caseDep = creationNumCase(repCoup.action.deplPion.caseDep);
			caseArr = creationNumCase(repCoup.action.deplPion.caseArr);			
		}else{
			printf("L'adversaire a passer son tour");
			caseDep = 0;
			caseArr = 0;
		}
	}
	int caseEnvoie = caseArr*10 + caseDep;
	err = send(sockJava, &caseEnvoie, sizeof(int), 0);
	if(err<0){
	  printf("erreur envoie coup adverse vers java (case de depart)");
	  exit(3);
  }
	printf("caseDep: %d, caseArr: %d err: %d\n",caseDep,caseArr,err);

	/*err = send(sockJava, &caseArr, sizeof(int), 0);
	 if(err<0){
	  printf("erreur envoie coup adverse vers java (case de arrivée)");
	  exit(3);
  }*/
}

void creationCoup(TCoupReq* coup,int couleur,int sockJava){
	
	int caseDep=0,caseArr=0,err;

	//reception des 2 cases
	err = recv(sockJava,&caseDep,sizeof(int),0);
	if(err<0){
	  printf("erreur reception case de depart de l'ia err: %d\n",errno);
	  exit(3);
  }

	caseArr = caseDep/10;
	caseDep = caseDep%10;	
	printf("Choix du coup %d-%d\n",caseDep,caseArr);

	coup->idRequest = COUP;
	//definition de la couleur
	if(couleur == 1){
		coup->coul = BLANC;
	}else{
		coup->coul = NOIR;
	}

	TCase cases1,cases2;
	TDeplPion depl;
	if(caseDep == caseArr){
		coup->typeCoup = POS_PION;
		creationTCase(&cases1,caseDep);
		coup->action.posPion = cases1;
	}else{
		if(caseDep == 0 && caseArr == 0){
			coup->typeCoup = PASSE;
			creationTCase(&cases1,caseDep);
			coup->action.posPion = cases1;
		}else{
			//choix entre positionnement et deplacement suivant l'ia
			coup->typeCoup = DEPL_PION;
			creationTCase(&cases1,caseDep);
			creationTCase(&cases2,caseArr);
			depl.caseDep = cases1;
			depl.caseArr = cases2;
		
			coup->action.deplPion = depl;
		}	
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
	int continu =0;
	int portJava;
	int sockJava;
	int tmp = 1;
	/* verification des arguments */
  if (argc != 5) {
    printf("usage : client nom_machine no_port nomJoueur no_port_Java\n");
    exit(1);
  }

	//Récupere le nom du joueur et vérfier qu'il ne soit pas trop long
	nomJoueur	= argv[3];
	while(nomJoueur[nbCarac]!='\0')
    ++nbCarac;

	if (nbCarac > TNOM-1) {
    printf("nom du joueur trop long\n");
		shutdown(sockJava, 2); close(sockJava);
    exit(2);
  }

	//création socket java
	portJava = atoi(argv[4]);
	printf("num port java %d\n",portJava);
	err = socketClient("localhost" , portJava);
  if(err<0){
	  printf("erreur connexion socket Java\n");
	  exit(3);
  }
	sockJava = err;

  nomMachine = argv[1];
  port = atoi(argv[2]);

  err = socketClient(nomMachine , port);
  
  if(err<0){
	  printf("erreur connexion socket c\n");
		shutdown(sockJava, 2); close(sockJava);
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
		shutdown(sockJava, 2); close(sockJava);
		shutdown(sock, 2); close(sock);
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
		printf("Erreur reception validation de la partie\n");
		shutdown(sockJava, 2); close(sockJava);
		shutdown(sock, 2); close(sock);
		exit(4);
	}	

	//envoie de la couleur de pion au java
	err = send(sockJava, &couleurPion, sizeof(int), 0);
	if(err <= 0){
		printf("Erreur send couleur vers java\n");
		shutdown(sockJava, 2); close(sockJava);
		shutdown(sock, 2); close(sock);
		exit(4);
	}

	//envoie coup
	//demande coup a l'IA

	//Si on joue en premier on recoie le premier coup de l'adversaire avant de rentre dans la boucle de jeu
	if((couleurPion == 2 && nbPartie == 0) || (couleurPion == 1 && nbPartie == 1)){

		//reception de la validation du coup adverse
		err = recv(sock,&repCoup,sizeof(TCoupRep),0);
		if(err <= 0){
			printf("Erreur reception validation du coup adverse 1\n");
			//shutdown(sockJava, 2); close(sockJava);
			shutdown(sock, 2); close(sock);
			exit(5);
		}

		if(repCoup.err == ERR_OK && repCoup.propCoup == CONT){
			//reception du coup
			err = recv(sock,&coupAdverse,sizeof(TCoupReq),0);

			if(err <= 0){
				printf("Erreur lors de la reception du coup adverse\n");
  		  //shutdown(sockJava, 2); close(sockJava);
  	  	shutdown(sock, 2); close(sock);
				exit(6);
			}
			gestionCoupAdverse(coupAdverse,sockJava);
			continu = 1;
		}
	}


	if(continu == 1 || couleurPion == 1 || nbPartie == 1){

	do{

		//envoie du prochain coup
		
		creationCoup(&coup,couleurPion,sockJava);
		err = send(sock, &coup, sizeof(TCoupReq), 0);
		if(err <= 0){
			printf("Erreur send d'envoie du coup\n");
	  	//shutdown(sockJava, 2); close(sockJava);
	  	shutdown(sock, 2); close(sock);
			exit(7);
		}

	//reception de la validation du coup
	err = recv(sock,&repCoup,sizeof(TCoupRep),0);
		if(err <= 0){
			printf("Erreur reception validation de mon coup \n");
			//shutdown(sockJava, 2); close(sockJava);
      shutdown(sock, 2); close(sock);
			exit(8);
		}

	if(repCoup.err != ERR_OK && repCoup.validCoup != VALID){
		printf("Erreur coup invalide\n");
		break;
	}

	if(repCoup.propCoup == CONT){
	//reception coup adverse
		//reception de la validation du coup adverse
		err = recv(sock,&repCoup,sizeof(TCoupRep),0);
		if(err <= 0){
			printf("Erreur reception validation du coup adverse 2\n");
			//shutdown(sockJava, 2); close(sockJava);
			shutdown(sock, 2); close(sock);
			exit(9);
		}

		if(repCoup.err == ERR_OK && repCoup.propCoup == CONT){
			//reception du coup
			err = recv(sock,&coupAdverse,sizeof(TCoupReq),0);

			if(err <= 0){
				printf("Erreur lors de la reception du coup adverse\n");
				//shutdown(sockJava, 2); close(sockJava);
				shutdown(sock, 2); close(sock);
				exit(10);
			}
			gestionCoupAdverse(coupAdverse,sockJava);
			continu = 1;
		}
	}
		int bidon = 55;
		if(repCoup.propCoup == CONT){
			tmp = 1;
			err = send(sockJava, &tmp, sizeof(int), 0);
		}else{
			tmp = 0;

			err = send(sockJava, &bidon, sizeof(int), 0);
			err = send(sockJava, &tmp, sizeof(int), 0);	
		}

		if(err <= 0){
				printf("Erreur send de continu partie vers java 2\n");
				//shutdown(sockJava, 2); close(sockJava);
				shutdown(sock, 2); close(sock);
				exit(7);
			}
	}while(repCoup.propCoup == CONT);
	nbPartie++;
	printf("Fin de partie 1\n");

		
		if((couleurPion == 2 && nbPartie == 0) || (couleurPion == 1 && nbPartie == 1)){

			err = recv(sock,&repCoup,sizeof(TCoupRep),0);
			

			if(repCoup.err == ERR_OK && repCoup.propCoup == CONT){
				err = recv(sock,&coupAdverse,sizeof(TCoupReq),0);

				if(err <= 0){
					printf("Erreur lors de la reception du coup adverse\n");
	  	  			shutdown(sock, 2); close(sock);
					exit(6);
				}
				gestionCoupAdverse(coupAdverse,sockJava);
				continu = 1;
			}
		}

		do{

				//envoie du prochain coup
		
			creationCoup(&coup,couleurPion,sockJava);
			err = send(sock, &coup, sizeof(TCoupReq), 0);
			

			err = recv(sock,&repCoup,sizeof(TCoupRep),0);
				

			if(repCoup.err != ERR_OK && repCoup.validCoup != VALID){
				printf("Erreur coup invalide\n");
				break;
			}

			if(repCoup.propCoup == CONT){

				err = recv(sock,&repCoup,sizeof(TCoupRep),0);

				if(repCoup.err == ERR_OK && repCoup.propCoup == CONT){
	
					err = recv(sock,&coupAdverse,sizeof(TCoupReq),0);

					gestionCoupAdverse(coupAdverse,sockJava);
					continu = 1;
				}
			}

			int bidon = 55;
			if(repCoup.propCoup == CONT){
				tmp = 1;
				err = send(sockJava, &tmp, sizeof(int), 0);
			}else{
				tmp = 0;

				err = send(sockJava, &bidon, sizeof(int), 0);
				err = send(sockJava, &tmp, sizeof(int), 0);	
			}

		}while(repCoup.propCoup == CONT);
	

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
	//shutdown(sockJava, 2); close(sockJava);
  shutdown(sock, 2); close(sock);
	/*-----------------*/
	}

}



