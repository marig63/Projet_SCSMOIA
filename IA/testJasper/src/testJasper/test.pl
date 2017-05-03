:-use_module(library(random)).
 :-use_module(library(lists)).



listeCoupPossible(Plateau,CouleurPion,NbPion,Res):-
        %placementPion(Plateau,1,NbPion,CouleurPion,[],Res).
        deplacementPion(Plateau,1,NbPion,CouleurPion,[],Res).

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
pionAppartient([CouleurPion|_],CouleurPion).

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
     
deplacementPion([],_,_,_,Acc,Res):-
        random_member(Res,Acc).

deplacementPion([Case|Plateau],NumCase,NbPion,CouleurPion,Acc,Res):-
        NbPion >= 8,
        pionAppartient(Case,CouleurPion),
        deplace(NumCase,Plateau,Acc1),
        NumCaseSuiv is NumCase +1,
        deplacementPion(Plateau,NumCaseSuiv,NbPion,CouleurPion,[Acc1|Acc],Res).

deplace(NumCaseDep,Plateau,Acc1):-
        deplacementPossible(NumCaseDep,NumCaseArr),
        caseValide(NumCaseArr),
        caseLibre(NumCaseArr,Plateau),
        append([NumCaseDep,NumCaseArr],Acc1,Acc1).
        %ResCoup = [[NumCaseDep,NumCaseArr]|ResCoup].

        

