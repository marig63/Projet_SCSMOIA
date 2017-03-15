#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>

#define TAIL_BUF 500
#define T_NOM 20

int socketServeur(ushort nbPort);
int socketClient(char* nomMachine, ushort nbPort);
int socketUDP(ushort nbPort);
int adresseUDP(char* nomMachine, ushort nbPort, struct sockaddr_in* addr);
