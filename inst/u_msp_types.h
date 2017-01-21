/*---------------------------------------------------------------*/
/* u_msp_types.h						 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.1		28.4.93 				 */
/*---------------------------------------------------------------*/
/* Instantiierung der generischen Datentypen fuer das MSP
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  U_TYPES_H
#define  U_TYPES_H

#include "error.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

#ifdef SUN_CC

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
    int		size;
    int		sum_d0;
    int		*d1d2;
    int		**d;
   }
u_glob_t;

/*------------------------ U_NODE_T ----------------------------*/
/* Benutzersicht auf einen Knoten im Branch-and-Bound-Baum. 
 * Enthaelt alle fuer das zugrundeliegende BB-Verfahren wichtigen 
 * Daten: Das fuer den darunterliegenden Ast zu loesende 
 * Teilproblem, evtl schon berechnete Schranken etc. 
 */

typedef struct u_node_st
   {
    byte		*list;
    int			fixed;
    int			free;
    int			fw[3];
    int			sum_d1;
    int			sum_d2;
    int			min_d2;
    int 		min_d1d2;
    u_zf_t		l_bound;	/* Untere Schranke	*/
    int			depth;
   }
u_node_t;

#endif /* U_TYPES_H */
/*------------------------ ENDE ---------------------------------*/
