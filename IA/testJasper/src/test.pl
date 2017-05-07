:-use_module(library(random)).
:-use_module(library(lists)).

pred(text,42,X,Y,Z):-
        X is 64,
        Y is 128,
        Z is 256.

listeCoupPossible(Plateau,CouleurPion,NbPion,Res):-
        placementPion(Plateau,1,NbPion,CouleurPion,[],Res).
listeCoupPossible(Plateau,CouleurPion,NbPion,Res):-       
        deplacementPion(Plateau,NbPion,CouleurPion,[1,2,3,4,5,6,7,8,9],[],Res).

placementPion([],_,_,_,Acc,[]):-
        length(Acc,Taille),
        Taille = 0.

placementPion([],_,_,_,Acc,Res):-
        random_member(Res,Acc).

placementPion([Case|Plateau],NumCase,NbPion,_,Acc,Res):-
        NbPion < 8,
        length(Case,NbPionCase),
        NbPionCase < 3,
        NumCaseSuiv is NumCase +1,
        placementPion(Plateau,NumCaseSuiv,NbPion,_,[[NumCase,NumCase]|Acc],Res).

placementPion([Case|Plateau],NumCase,NbPion,_,Acc,Res):-
        NbPion < 8,
        length(Case,NbPionCase),
        NbPionCase = 3,
        NumCaseSuiv is NumCase +1,
        placementPion(Plateau,NumCaseSuiv,NbPion,_,Acc,Res).

% vérifie si le pion au dessus de la case et de la meme couleur que la notre 
pionMemeCouleur([Couleur|_],CouleurPion):-
        Couleur = CouleurPion.
pionAppartient(NumCase,Plateau,CouleurPion):-
        nth1(NumCase,Plateau, Case),
        pionMemeCouleur(Case,CouleurPion).

nbColonne(3).
nbLigne(3).

caseValide(NumCase):-
        NumCase > 0,
        NumCase < 10.

testMemeLigne(NumCaseDep,NumCaseArr):-
        nbColonne(Nc),
        NumLigne is (NumCaseDep-1)//Nc,
        NumLigne is (NumCaseArr-1)//Nc.

deplacementPossible(NumCaseDep,NumCaseArr):-
        NumCaseArr is NumCaseDep + 1,
        testMemeLigne(NumCaseDep,NumCaseArr).

deplacementPossible(NumCaseDep,NumCaseArr):-
        NumCaseArr is NumCaseDep - 1,
        testMemeLigne(NumCaseDep,NumCaseArr).

deplacementPossible(NumCaseDep,NumCaseArr):-
        nbColonne(Nc),
        NumCaseArr is NumCaseDep - Nc.

deplacementPossible(NumCaseDep,NumCaseArr):-
        nbColonne(Nc),
        NumCaseArr is NumCaseDep + Nc.

caseLibre(NumCaseArr,Plateau):-
        nth1(NumCaseArr,Plateau, Case),
        length(Case,NbPionCase),
        NbPionCase < 3.   
     
deplacementPion(_,_,_,_,Acc,Acc):-
        length(Acc,Taille),
        Taille > 0,!.
        %random_member(Res,Acc).

deplacementPion(_,_,_,[],_,[]):-!.

deplacementPion(Plateau,NbPion,CouleurPion,ListeCase,_,Res):-
        NbPion >= 8,
        random_select(NumCase,ListeCase,ListeCase2),
        %pionAppartient(NumCase,Plateau,CouleurPion),
        !,
        profondeur([],NumCase,Plateau,CouleurPion,Acc),
        deplacementPion(Plateau,NbPion,CouleurPion,ListeCase2,Acc,Res).

deplacementPion(Plateau,NbPion,CouleurPion,ListeCase,Acc,Res):-
        NbPion >=8,
        deplacementPion(Plateau,NbPion,CouleurPion,ListeCase,Acc,Res).
   
deplace(NumCaseDep,Plateau,_,[NumCaseDep,NumCaseArr]):-
        deplacementPossible(NumCaseDep,NumCaseArr),
        caseLibre(NumCaseArr,Plateau),
        caseValide(NumCaseDep).
        %\+member([NumCaseDep,NumCaseArr],Acc).
        %deplace(NumCaseDep,Plateau,[[NumCaseDep,NumCaseArr]|Acc],Res).

profondeur(L,NumCaseDep,Plateau,CouleurPion,L):-
               \+pionAppartient(NumCaseDep,Plateau,CouleurPion),!.       

profondeur(L,NumCaseDep,Plateau,CouleurPion,Sol):-
                pionAppartient(NumCaseDep,Plateau,CouleurPion),
                 deplace(NumCaseDep,Plateau,_,Res),
                 \+member(Res,L),!,
                 append([Res],L,LL),
                 profondeur(LL,NumCaseDep,Plateau,CouleurPion,Sol). 

profondeur(L,_,_,_,L):-!.
       