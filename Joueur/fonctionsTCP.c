#include "fonctionsTCP.h"

int socketServeur(ushort nbPort){
	
	
	  int sockConx,        /* descripteur socket connexion */
		sockTrans,       /* descripteur socket transmission */
       port = nbPort,            /* numero de port */
       sizeAddr,        /* taille de l'adresse d'une socket */
       err;	        /* code d'erreur */
       
      sizeAddr = sizeof(struct sockaddr_in);
	  
	  
	  struct sockaddr_in nomTransmis;	/* adresse socket de transmission */
	  struct sockaddr_in nomConnecte;	/* adresse de la socket connectee */
	  
	  /* 
	   * creation de la socket, protocole TCP 
	   */
	  sockConx = socket(AF_INET, SOCK_STREAM, 0);
	  if (sockConx < 0) {
		perror("serveurTCP: erreur de socket\n");
		return -2;
	  }
	  
	  /* 
	   * initialisation de l'adresse de la socket 
	   */
	  nomConnecte.sin_family = AF_INET;
	  nomConnecte.sin_port = htons(port); // conversion en format réseau (big endian)
	  nomConnecte.sin_addr.s_addr = INADDR_ANY; 
	  // INADDR_ANY : 0.0.0.0 (IPv4) donc htonl inutile ici, car pas d'effet
	  bzero(nomConnecte.sin_zero, 8);
	  
	  /* 
	   * attribution de l'adresse a la socket
	   */
	  err = bind(sockConx, (struct sockaddr *)&nomConnecte, sizeAddr);
	  if (err < 0) {
		perror("serveurTCP: erreur sur le bind");
		close(sockConx);
		return -3;
	  }
	  
	  /* 
	   * utilisation en socket de controle, puis mise à disposition pour les 
	   * connexions.
	   */
	  err = listen(sockConx, 1);
	  if (err < 0) {
		perror("serveurTCP: erreur dans listen");
		return -4;
	  }
	  
	  /*
	   //attente de connexion
	   
	  sockTrans = accept(sockConx, 
				 (struct sockaddr *)&nomTransmis, 
				 (socklen_t *)&sizeAddr);
	  if (sockTrans < 0) {
		perror("serveurTCP:  erreur sur accept");
		return -5;
	}
	
	return sockTrans;*/
	
	return sockConx;
}


int socketClient(char* nomMachine, ushort nbPort){
	// socket connectée avec le socket identifié (machine + port)
	
	 char chaine[TAIL_BUF];   /* buffer */
  int sock,                /* descripteur de la socket locale */
      port = nbPort,       /* variables de lecture */
      err;                 /* code d'erreur */
  struct hostent*    host; /* description de la machine serveur */
  struct sockaddr_in nom;  /* adresse de la socket du serveur */

  int size_addr_in = sizeof(struct sockaddr_in);


  /* 
   * creation d'une socket, domaine AF_INET, protocole TCP 
   */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("client : erreur sur la creation de socket");
    return -1;
  }
  
  /* 
   * initialisation de l'adresse de la socket 
   */
  nom.sin_family = AF_INET;
  bzero(nom.sin_zero, 8);
  nom.sin_port = htons(port);
  
  /* recherche de l'adresse de la machine serveur */
  host = gethostbyname (nomMachine);
  if (host == NULL) {   
    printf("client : erreur gethostbyname %d\n", errno);
    return -2;
  }
  
  /* recopie de l'adresse IP */
  nom.sin_addr.s_addr = ((struct in_addr *) (host->h_addr_list[0]))->s_addr;
					     
  /* 
   * connection au serveur 
   */
  err = connect(sock, (struct sockaddr *)&nom, size_addr_in);
  if (err < 0) {
    perror("client : erreur a la connection de socket\n");
    return -3;
  }
	
	
	return sock;
}

int socketUDP(ushort nbPort){
	
	int             sock,	        /* descripteur de socket locale */
                    err;	        /* code d'erreur */

    struct sockaddr_in adr_recep,	/* adresse de la socket */
                       adr_emet;        /* adresse emetteur */

    char buffer[TAIL_BUF];       	/* buffer de reception */
                  

    int size_addr;	                /* taille de l'adresse d'une socket */

  
   /* creation de la socket, protocole UDP */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
	perror("socketUDP : erreur de socket\n");
	exit(2);
    }

    /* 
     * initialisation de l'adresse de la socket 
     */
    adr_recep.sin_family = AF_INET;
    adr_recep.sin_port = htons(nbPort);
    adr_recep.sin_addr.s_addr = INADDR_ANY;
	// INADDR_ANY : 0.0.0.0 (IPv4) donc htonl inutile ici, car pas d'effet
    bzero(adr_recep.sin_zero, 8);

    /*
     * initialisation de la taille d'une adresse de socket
     */
    size_addr = sizeof(struct sockaddr_in);

    /* 
     * attribution de l'adresse a la socket
     */
    err = bind(sock, (struct sockaddr *)&adr_recep, size_addr);
    if (err < 0) {
	perror("socketUDP : erreur sur le bind");
	exit(3);
    }
	
	
	return sock;
}



int adresseUDP(char* nomMachine, ushort nbPort, struct sockaddr_in* addr){
	
	int sock,               /* descripteur de la socket locale */
      port = nbPort,               /* port recepteur */
      size_addr, 	  /* taille de l'adresse d'une socket */
      err;                /* code d'erreur */
  char chaine[TAIL_BUF];
  
  struct sockaddr_in adrLocal;	  /* adresse de la socket locale */

  struct hostent* host;    /* pour l'adresse du recepteur */
    
  /* creation de la socket, protocole UDP */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socketUDP : erreur de socket\n");
    exit(2);
  }

  /* 
   * initialisation de l'adresse de la socket 
   */
  adrLocal.sin_family = AF_INET;
  adrLocal.sin_port = htons(nbPort);
  adrLocal.sin_addr.s_addr = INADDR_ANY;
  	// INADDR_ANY : 0.0.0.0 (IPv4) donc htonl inutile ici, car pas d'effet
  bzero(adrLocal.sin_zero, 8);

  /* 
   * initialiser la longueur d'une structure d'adresse de socket 
   */
  size_addr = sizeof(struct sockaddr_in);

  /* 
   * attribution de l'adresse a la socket
   */
  err = bind(sock, (struct sockaddr *)&adrLocal, size_addr);
  if (err < 0) {
    perror("socketUDP : erreur sur le bind");
    exit(3);
  }

  /*
   * saisie et initialisation de l'adresse du destinataire
   */
   
  port = 0;
  printf("client : donner le port dest : ");
  scanf("%d", &port);
  printf("client : initaddr pour %s, %d\n",  nomMachine, port);


  host = gethostbyname(nomMachine);
  if (host == NULL) {
    printf("initAddr : erreur gethostbyname\n");
    exit(4);
  }

  /* recopie de l'adresse IP */
  addr->sin_addr.s_addr = ((struct in_addr *) (host->h_addr_list[0]))->s_addr;
      
  /* initialisation de l'adresse de la socket */
  addr->sin_family = AF_INET;
  bzero(addr->sin_zero, 8);
  addr->sin_port = htons(port);

	
	return sock;
}

