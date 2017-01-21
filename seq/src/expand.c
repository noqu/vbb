/*---------------------------------------------------------------*/
/* expand.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Expander: Funktionen zur Expansion von Knoten
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

/* #define		TRACE_ON */
#define		TRACE_CONDITION		glob.flags & TRACE_EXP

#include  <stdio.h>
#include  <stdlib.h>
#include  "types.h"
#include  "u_types.h"
#include  "mem.h"
#include  "expand.h"
#include  "heap.h"
#include  "u_funcs.h"
#include  "error.h"
#include  "trace.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

extern stat_t		statistic;
extern glob_t		glob;

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

static int	try_pruning ();

#else

static int	try_pruning (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */);
#endif

/*^*/
/*------------------------ EXPANDER -----------------------------*/
/* Schleife zur Expansion von Knoten. Nimmt jeweils einen 
 * (Vater-)Knoten vom Heap, erzeugt dessen Soehne und versucht sie 
 * zu eliminieren. Leere Knoten zur Erzeugung der Soehne werden aus 
 * der Freelist entnommen. Die nicht eliminierten Soehne werden auf 
 * den Heap gelegt Dispatcher weitergegeben. Wenn ein erzeugter 
 * Sohn eine neue Bestloesung (Incumbent) darstellt, wird er als 
 * solche eingetragen. Wenn ein Fehler auftritt, beendet sich die 
 * Funktion mit einem Status. geschickt und der Prozess beendet. 
 */

int expander (u_glob) 

u_glob_t	*u_glob;

   {
    boolean		e_finished;
    node_t		*parent;
    int			sts;
    node_t		*son;
    int			last_selection;
    node_t		*incumbent;
    int			prof_slot;

    e_finished = OK_FALSE;

    /* Hole leeren Knoten fuer ersten Sohn: */
    sts = m_get_node (u_glob, &son);
    if (sts > ERROR)
	e_finished = sts;

    /* Solange nicht fertig */
    while (!e_finished)
       {
	/* Hole besten Knoten vom Heap: */
	sts = h_get_from_heap (&parent);
	/* Wenn Heap leer: */
	if (sts == OK_FALSE)
	   {
	    T ("Expander finished");
	    /* Hier Schluss machen: */
	    e_finished = OK_TRUE;
	    /* Kein Fehler: */
	    sts = OK;
	   }
	else
	   {
	    B_ST
	    TKN ("Vom Heap geholt", parent);
	    statistic.expanded++;
	    /* Wenn Profil der L-Bounds erwuenscht: */
	    if (glob.flags & PROFILE)
	       {
		if (parent->l_bound < glob.prof_start)
		    prof_slot = 0;
		else if (parent->l_bound > glob.prof_start +
			 glob.prof_unit * (PROF_BUF_SIZE - 2))
		    prof_slot = PROF_BUF_SIZE - 1;
		else
		    prof_slot = (parent->l_bound + glob.prof_unit 
				 - glob.prof_start) / glob.prof_unit;
		statistic.profile[prof_slot]++;
	       }
	    /* Erzeuge ersten Sohn: */
	    sts = u_first_child (u_glob, parent->u_node, son->u_node,
				 &last_selection);
	    B_ST
	    /* Solange noch ein Sohn entstanden */
	    while (sts)
	       {
		/* T ("Sohn erzeugt"); */
		statistic.nodes++;
		/* Wenn Sohn eine zulaessige Loesung ist */
		sts = u_is_feasible (u_glob, son->u_node);
		if (sts)
		   {
		    B_ST
		    statistic.feasible++;
		    /* Erzeuge die Loesung: */
		    sts = u_solution (u_glob, son->u_node, 
				      &son->value);
		    B_ST

		    /* Prioritaet setzen fuer korrektes Tracing: */
#		    ifdef TRACE_ON
			son->priority = 0;
#		    endif
		    TKN ("Sohn ist zulaessige Loesung:", son);

		    /* Incumbent holen: */
		    incumbent = h_read_incumbent ();
		    /* Wenn Sohn besser als incumbent */
		    if (son->value < incumbent->value)
		       {
			/* Verkuende neue incumbent: */
			h_new_incumbent (son);
			T ("Neue Incumbent");
			statistic.improved++;
			/* Hole Knoten fuer neuen Sohn: */
			sts = m_get_node (u_glob, &son);
			B_ST
		       }
		    /* (Vergiss diesen Sohn) */
		   }
		else
		   {
		    B_ST
		    /* Versuche Sohn zu eliminieren: */
		    sts = try_pruning (u_glob, son->u_node);
		    B_ST
		    /* Wenn Sohn ueberlebt: */
		    if (sts == OK_FALSE)
		       {
			/* Berechne Prioritaet fuer den Sohn: */
			sts = u_priority (u_glob, son->u_node, 
					  glob.prio_par, 
					  &son->priority);
			B_ST
			/* Kopiere l_bound von u_node: */
			sts = u_lower_bound (u_glob, son->u_node,
					     &son->l_bound);
			B_ST
			/* Merke Dir die Familienzugehoerigkeit: */
			son->father = parent;
			/* Schicke Sohn an Dispatcher: */
			TKN ("Sohn an Dispatcher geschickt:", son);
			h_put_on_heap (son);
			/* Hole Knoten fuer neuen Sohn: */
			sts = m_get_node (u_glob, &son);
			B_ST
		       }
		    else
		       {
			/* (Vergiss diesen Sohn) */
			/* Uebertrage Werte fuer korrektes Tracing: */
#			ifdef TRACE_ON
			    /* Berechne Prioritaet fuer den Sohn: */
			    sts = u_priority (u_glob, son->u_node, 
					      glob.prio_par, 
					      &son->priority);
			    B_ST
			    /* Kopiere l_bound von u_node: */
			    sts = u_lower_bound (u_glob, son->u_node,
						 &son->l_bound);
			    B_ST
#			endif
			TKN ("Sohn eliminiert", son);
		       }
		   }
		/* Erzeuge naechsten Sohn: */
		sts = u_next_child (u_glob, parent->u_node, 
				    son->u_node, &last_selection);
		B_ST
	       } /* end while */
	    B_ST
	    /* Abgearbeitete Elternknoten recyclen: */
	    m_free_node (parent);
	   } /* end else */
       } /* end while */
    return (sts);
   } 

/*^*/
/*------------------------ TRY_PRUNING --------------------------*/
/* Versucht, den Knoten 'node' zu eliminieren. Berechnet die Upper 
 * Bound; wenn diese groesser als die incumbent ist, kann der 
 * Knoten eliminiert werden. Sonst wird die Upper Bound berechnet. 
 * Ist diese kleiner als die incumbent, so liegt eine neue 
 * incumbent vor. Diese wird veroeffentlicht und 'node' eliminiert. 
 * Sonst werden die Bounds verglichen: Ist die Lower Bound nicht 
 * kleiner als die Upper Bound, kann 'node' eliminiert werden. Der 
 * Rueckgabewert ist 'OK_TRUE', wenn 'node' eliminiert werden kann. 
 */

static int	try_pruning (u_glob, node)

u_glob_t	*u_glob;
u_node_t	*node;

   {
    u_zf_t		l_bound;
    node_t		*incumbent;
    static node_t	*solution;
    int			sts;
    u_zf_t		value;
    extern glob_t	glob;

    /* Incumbent holen: */
    incumbent = h_read_incumbent ();
    /* Berechne Lower Bound: */
    sts = u_lower_bound (u_glob, node, &l_bound);
    R_ST
    /* Wenn schlechter als incumbent oder unbeschraenkt */
    if (l_bound >= incumbent->value || l_bound == MAXINT)
       {
	statistic.elim_by_lb++;
	return (OK_TRUE);
       }

    /* Hole bei Bedarf neuen leeren Knoten fuer heuristische Loesung: */
    if (!solution)
       {
        sts = m_get_node (u_glob, &solution);
	R_ST
       }
	
    /* Wenn Verwendung von Heuristik erwuenscht: */
    if (glob.flags & ELIM_BY_HEUR)
       {
	/* Berechne Upper Bound (heuristische Loesung): */
	sts = u_heur_loes (u_glob, node, solution->u_node, &value);
	R_ST
	/* TV ("Heuristische Loesung", value); */
	statistic.heur_solved++;
	/* Wenn besser als incumbent */
	if (value < incumbent->value)
	   {
	    /* Trage value in node ein: */
	    solution->value = value;
	    TKN ("Neue Bestloesung:", solution);
	    statistic.improved++;
	    /* Verkuende neue incumbent: */
	    h_new_incumbent (solution);
	    /* solution muss beim naechsten Mal neu allokiert werden: */
	    solution = (node_t *) 0;
	   }

	/* Wenn Lower Bound = Upper Bound */
	if (l_bound == value)
	   {
	    T ("Lower = Upper Bound");
	    statistic.elim_by_ub++;
	    return (OK_TRUE);
	   }
       }

    return (OK_FALSE);
   }

/*------------------------ ENDE ---------------------------------*/
