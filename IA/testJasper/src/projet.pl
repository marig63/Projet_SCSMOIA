
caseLibre(_,_,[]).

caseLibre(NumCase,Cpt,[_|Plateau]):-
        dif(NumCase,Cpt),
        CptSuiv is Cpt + 1,
        caseLibre(CptSuiv,CptSuiv,Plateau).

caseLibre(NumCase,Cpt,[Case|_]):-
        NumCase = Cpt,
        length(Case,NbPionCase),
        NbPionCase < 3.

deplacementPossible(NumCaseDep,NumCaseArr):-
        NumCaseArr is NumCaseDep + 1,
        testMemeLigne(NumCaseArr).

deplacementPossible(NumCaseDep,NumCaseArr):-
        NumCaseArr is NumCaseDep - 1,
        testMemeLigne(NumCaseArr).

deplacementPossible(NumCaseDep,NumCaseArr):-
        nbColonne(Nc),
        NumCaseArr is NumCaseDep - Nc.

deplacementPossible(NumCaseDep,NumCaseArr):-
        nbColonne(Nc),
        NumCaseArr is NumCaseDep + Nc.

deplace(NumCaseDep,Plateau,Res):-
        deplacementPossible(NumCaseDep,NumCaseArr),
        caseValide(NumCaseArr),
        caseLibre(NumCaseArr,1,Plateau),
        Res = [[NumCaseDep,NumCaseArr]|Res].




        
                        
                        
        
   