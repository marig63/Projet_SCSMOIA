all: serveur

serveur: serveur.c fonctionsTCP.c colonne.o
	gcc serveur.c fonctionsTCP.c colonne.o -o serveur 

clean:
	rm *~ ; rm -i \#* ; \
        rm serveur ;