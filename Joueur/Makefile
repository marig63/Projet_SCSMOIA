#########
#
# Makefile pour mini
#
#########

# For Solaris
#LD_FLAGS = -lsocket -lnsl

# For Linux
LD_FLAGS =

all: joueur

joueur: joueur.c fonctionsTCP.c
	gcc joueur.c fonctionsTCP.c -o joueur   $(LD_FLAGS)

clean:
	rm *~ ; rm -i \#* ; rm *.o; \
        rm joueur
