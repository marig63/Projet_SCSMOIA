/* -*- Mode:Prolog; coding:iso-8859-1; indent-tabs-mode:nil; prolog-indent-width:8; prolog-paren-indent:3; tab-width:8; -*- */



 :-use_module(library(clpfd)).
 :-use_module(library(lists)).
 :-use_module(library(clpq)).

cryptoGT([M,O,S,E,N,R,D,Y]):-
        L=[0,1,2,3,4,5,6,7,8,9],
        member(M,L),
        member(O,L),
        member(S,L),
        member(E,L),
        member(N,L),
        member(R,L),
        member(D,L),
        member(Y,L),
        dif(M,O),dif(M,S),dif(M,E),dif(M,N),dif(M,R),dif(M,D),dif(M,0),dif(M,Y),
        dif(O,S),dif(O,E),dif(O,N),dif(O,R),dif(O,D),dif(O,Y),
        dif(S,E),dif(S,N),dif(S,R),dif(S,D),dif(S,Y),
        dif(E,N),dif(E,R),dif(E,D),dif(E,Y),
        dif(N,R),dif(N,D),dif(N,Y),
        dif(R,D),dif(R,Y),
        dif(D,Y),
        Somme is (S+M) * 1000 + (E+O) * 100 + (N+R) * 10 + (D+E),
        Somme is (M * 10000) + (O*1000) + (N*100) + (E*10) + Y.



cryptoBT([M,O,S,E,N,R,D,Y]):-
        
        select(E,[0,1,2,3,4,5,6,7,8,9],L1),
        select(D,L1,L2),
        calcul(E,D,0,Y,R1),
        select(Y,L2,L3),
        
        select(N,L3,L4),
        select(R,L4,L5),
        calcul(N,R,R1,E,R2),
        %select(Y,L5,L6),
        
        %select(E,L6,L7),
        select(O,L5,L7),
        calcul(E,O,R2,N,R3),
        %select(N,L8,L9),
        
        select(S,L7,L8),
        select(M,L8,_),
        dif(M,0),
        calcul(S,M,R3,O,M).
        
        %member(M,L9).
        
        

calcul(X,Y,R1,Somme,R2):-
        S1 is X+Y+R1,
        Somme is mod(S1,10),
        R2 is S1//10.
     


cryptoPLC([M,O,S,E,N,R,D,Y]):-
        domain([M,O,S,E,N,R,D,Y],0,9),
        all_different([M,O,S,E,N,R,D,Y]), M #\= 0,
        (1000 * (S+M) + 100 * (E+O) + 10 * (N+R) + D+E ) #= ((M * 10000) + (O*1000) + (N*100) + (E*10) + Y),
        labeling([],[M,O,S,E,N,R,D,Y]).
      

% Partie Reine GT

         

construitListe(0,[]).
construitListe(N,[N|R]):-
        N>0,
        N1 is N-1,
        construitListe(N1,R).

placerGT(0,_,[]).
placerGT(N,L,[[X,Y]|R]):-
        N>0,
        member(X,L),
        member(Y,L),
        N1 is N-1,
        placerGT(N1,L,R).


testerGT([]).
testerGT([R1|LR]):-
        testerGTReine(LR,R1),
        testerGT(LR).

testerGTReine([],_).
testerGTReine([[X2,Y2]|LR],[X1,Y1]):-
        dif(X1,X2),dif(Y1,Y2),
        abs(X1-X2) =\= abs(Y1-Y2),
        testerGTReine(LR,[X1,Y1]).

reineGT(N,R):-
        construitListe(N,L),
        placerGT(N,L,R),
        testerGT(R).
        
% Partie Reine BT

testerBT([],_).
testerBT([[X2,Y2]|LR], [X1,Y1]) :-
        abs(X1-X2) =\= abs(Y1-Y2),
        %ligne pour les amazone
        abs(X1-X2)+abs(Y1-Y2) > 3,
        % --
        testerBT(LR,[X1,Y1]).

placerBT(0,_,R,R).
placerBT(N,L,Acc,R) :-
        N > 0,
        select(Y,L,L1),
        testerBT(Acc,[N,Y]),
        N1 is N-1,
        placerBT(N1,L1,[[N,Y]|Acc],R).

reineBT(N,R) :-
        construitListe(N,L),
        placerBT(N,L,[],R).


% Partie Reine PLC %


testerPLC([],_).
testerPLC([[X2,Y2]|LR],[X1,Y1]) :-
        abs(X1-X2) #\= abs(Y1-Y2),
        %ligne pour les amazone
        abs(X1-X2)+abs(Y1-Y2) #> 3,
        % -- 
        Y1 #\= Y2,
        testerPLC(LR, [X1,Y1]).

contraintes(0,_,[],R,R).
contraintes(X,N,[Y|LY],Acc,R) :-
        X>0,
        domain([Y],1,N),
        testerPLC(Acc,[X,Y]),
        X1 is X-1,
        contraintes(X1,N,LY,[[X,Y]|Acc],R).

reinePLC(N,R) :-
        contraintes(N,N,LY,[],R),
        labeling([],LY).
                               