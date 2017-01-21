/*---------------------------------------------------------------*/
/* u_types.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.0		1.3.93 					 */
/*---------------------------------------------------------------*/
/* Generische Fassung der Datentypen fuer Benutzerfunktionen
 * (werden von Kernelfunktionen included, machen keine Annahmen 
 * ueber u_glob_t und u_node_t.)
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  U_TYPES_H
#define  U_TYPES_H

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/
/*------------------------ U_ZF_T -------------------------------*/
/* Typ fuer Zielfunktionswerte und Schrankenwerte.
 */

typedef		int	u_zf_t;

/*------------------------ U_GLOB_T -----------------------------*/
/* Globale Daten des Benutzers (Aufgabenstellung etc).
 */

typedef void	u_glob_t;

/*------------------------ U_NODE_T ----------------------------*/
/* Benutzersicht auf einen Knoten im Branch-and-Bound-Baum. 
 * Enthaelt alle fuer das zugrundeliegende BB-Verfahren wichtigen 
 * Daten: Das fuer den darunterliegenden Ast zu loesende 
 * Teilproblem, evtl schon berechnete Schranken etc. 
 */

typedef void	u_node_t;

#endif /* U_TYPES_H */
/*------------------------ ENDE ---------------------------------*/
