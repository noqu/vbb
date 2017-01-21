/*---------------------------------------------------------------*/
/* u_scp_types.h						 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.2		1.6.93 					 */
/*---------------------------------------------------------------*/
/* Instantiierung der generischen Datentypen fuer das SCP
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  U_TYPES_H
#define  U_TYPES_H

/* Zur Behandlung von Mengen wird das Modul 'set' verwendet: */
#include        "set.h"

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
   int		Xsize;	/* Anzahl Client-Ecken	*/
   int		Ysize;	/* Anzahl Server-Ecken	*/
   int		*c;	/* Kosten fuer die Server-Ecken	*/
   set_t	*X;	/* Menge der Clients	*/
   set_t	*Y;	/* Menge der Server	*/
   set_t	**succ;	/* Zu jedem Server: Menge der ueberdeckten
			 * Clients
			 */
   set_t	**pred;	/* Zu jedem Client: Menge der ihn
   			 * ueberdeckenden Server
   			 */
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
    set_t	*A;	/* Menge der Ecken fuer diese Teilloesung */
    set_t	*B;	/* Menge der von dieser Teilloesung
    			 * ausgeschlossenen Ecken.
			 */
    set_t	*succA;	/* In dieser Teilloesung bereits ueberdeckte
			 * Ecken
			 */
    u_zf_t	fixed_cost;
			/* Kosten der fixierten Ecken	*/
    u_zf_t	l_bound;/* Untere Schranke	*/
    int		depth;	/* Tiefe des Knotens im Suchbaum	*/
   }
u_node_t;

#endif /* U_TYPES_H */
/*------------------------ ENDE ---------------------------------*/
