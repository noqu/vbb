/*---------------------------------------------------------------*/
/* u_gen_types.h						 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.2		1.6.93 					 */
/*---------------------------------------------------------------*/
/* Dekloarationen fuer generische Datentypen als Vorlage zur
 * Entwicklung neuer Instanzfunktionen
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  U_TYPES_H
#define  U_TYPES_H

#include "error.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/
/*------------------------ U_ZF_T -------------------------------*/
/* Typ fuer Zielfunktionswerte und Schrankenwerte.
 */

typedef		int	u_zf_t;

/*------------------------ U_GLOB_T -----------------------------*/
/* Globale Daten des Benutzers (Aufgabenstellung etc).
 */

typedef struct u_glob_st
   {
    int		size;	/* Anzahl Ecken des Problems	*/
    int		**d;	/* Entfernungsmatrix	*/
   }
u_glob_t;

/*------------------------ U_NODE_T ----------------------------*/
/* Benutzersicht auf einen Knoten im Branch-and-Bound-Baum. 
 * Enthaelt alle fuer das zugrundeliegende BB-Verfahren wichtigen 
 * Daten: Das fuer den darunterliegenden Ast zu loesende 
 * Teilproblem, evtl. vererbte Informationen des Vaters,
 * Schrankenwerte etc.
 */

typedef struct u_node_st
   {
    u_zf_t	l_bound;
			/* Untere Schranke	*/
    int		depth;	/* Tiefe des Knotens im Suchbaum	*/
   }
u_node_t;

#endif /* U_TYPES_H */
/*------------------------ ENDE ---------------------------------*/
