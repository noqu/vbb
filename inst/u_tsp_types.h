/*---------------------------------------------------------------*/
/* u_tsp_types.h						 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.0		1.3.93 					 */
/*---------------------------------------------------------------*/
/* Instantiierung der generischen Datentypen fuer das TSP
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  U_TYPES_H
#define  U_TYPES_H

#include "error.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

#ifdef SUN_CC

/* Sun-Compiler versteht kein 'signed char': */
typedef		char	byte;

#else

typedef		signed char	byte;

#endif

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
    byte	*list;	/* Indexliste fuer freie und fixierte Ecken */
    byte	*zmin;	/* Indizes der Zeilenminima in der
    			 * Entfernungsmatrix
			 */
    byte	*smin;	/* Indizes der Spaltenminima in der
    			 * Entfernungsmatrix
			 */
    int		fixed;	/* Anfang der Indexliste der fixierten Ecken */
    int		free;	/* Anfang der Indexliste der freien Ecken */
    u_zf_t	fixed_cost;
			/* Gesamtkosten der fixierten Subtour */
    u_zf_t	l_bound;
			/* Untere Schranke	*/
    int		depth;	/* Tiefe des Knotens im Suchbaum	*/
   }
u_node_t;

#endif /* U_TYPES_H */
/*------------------------ ENDE ---------------------------------*/
