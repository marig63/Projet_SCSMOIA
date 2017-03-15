/*
 **********************************************************
 *
 *  Programme : validation.h
 *
 *  ecrit par : FB / VF
 *
 *  resume : entete pour la validation des coups
 *
 *  date :      09 / 03 / 17
 *  modifie : 
 ***********************************************************
 */

#ifndef _validation_h
#define _validation_h

/* Validation d'un coup :
 * parametres : 
 *    le numero du joueur : 1(le premier avec les blancs) ou 
 *                          2(le deuxième avec les noirs)
 *    le coup (TCoupReq)
 * resultat : type bool (coup valide ou non) 
 *            propriete du coup  (GAGNE, PERDU, NULLE - le coup rend le joueur gagnant, perdant ou la partie est nulle, ou CONT si aucune des autres)
*/
bool validationCoup(int joueur, TCoupReq coup, TPropCoup* propCoup);

#endif 
