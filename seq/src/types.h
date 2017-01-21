/*---------------------------------------------------------------*/
/* types.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Datentypen und sonstige Vereinbarungen fuer alle Funktionen
 * die nicht benutzerspezifisch sind.
 */
/*------------------------ PRAEPROZESSOR ------------------------*/
#ifndef  TYPES_H
#define  TYPES_H

#include  "u_types.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

#define	 PROF_BUF_SIZE	102

/*------------------------ NODE_T -------------------------------*/
/* Ein Knoten im Branch-and-Bound-Baum aus der Sicht des Kernels. 
 * Enthaelt die fuer die Verwaltung wichtigen Daten und (als eigene 
 * Struktur eingebettet) die Daten des BB-Verfahrens). 
 */

typedef struct node_st
   {
    u_node_t		*u_node;	/* BB-Knoten aus der Sicht
					 * des BB-Verfahrens
					 */
    int			priority;	/* Prioritaet (dieses
					 * Knotens)
					 */
    u_zf_t		value;		/* Zielfunktionswert */
    u_zf_t		l_bound;	/* Untere Schranke */
    struct node_st	*leftson;	/* Zeiger fuer die
					 * Verkettung in Listen oder
					 * Baeumen
					 */
    struct node_st	*rightson;	/* dito	*/
    struct node_st	*father;	/* dito	*/
   }
node_t;

/*------------------------ STAT_T -------------------------------*/

typedef struct stat_st
   {
    int			nodes;
    int			allocated;
    int			expanded;
    int			from_heap;
    int			from_lifo;
    int			heap_cycles;
    int			heur_solved;
    int			elim_by_lb;
    int			elim_by_ub;
    int			elim_from_heap;
    int			feasible;
    int			improved;
    int			got;
    int			sent;
    int			time;
    int			idle;
    int			profile[PROF_BUF_SIZE];
   } 
stat_t;

/*------------------------ GLOB_T ------------------------------*/

typedef struct glob_st
   {
    int			flags;
    int			prof_start;
    int			prof_unit;
    int			prof_classes;
    int			prof_scale;
    int			dist_low;
    int			dist_high;
    int			dist_idle;
    int			prio_par;
    int			max_heap_size;
    int			min_heap_size;
    int			reserve_size;
   }
glob_t;

#endif /* TYPES_H */
/*------------------------ ENDE ---------------------------------*/
