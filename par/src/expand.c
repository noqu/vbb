/*---------------------------------------------------------------*/
/* expand.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Expander: Bekommt Knoten ueber Queue vom Dispatcher, liefert
 * deren Soehne ueber eine Queue zurueck. Bezieht Leerknoten vom
 * Memserver
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

/* #define		TRACE_ON */
#define		TRACE_CONDITION		glob.flags & TRACE_EXP

#include  <stdio.h>
#include  <stdlib.h>
#include  <process.h>
#include  "types.h"
#include  "u_types.h"
#include  "queue.h"
#include  "mem.h"
#include  "expand.h"
#include  "disp.h"
#include  "heap.h"
#include  "u_funcs.h"
#include  "error.h"
#include  "trace.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

extern stat_t		statistic;
extern int		m_finished;
extern int		d_finished;
extern int		e_finished;
extern glob_t		glob;

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

public void	expander (
			Process*	/* p */,
			queue_t*	/* from_mem */,
			queue_t*	/* to_disp */,
			queue_t*	/* from_disp */,
			u_glob_t*	/* u_glob */,
			queue_t*	/* to_tracer */);
			

static int	try_pruning (
			u_glob_t*	/* u_glob */, 
			queue_t*	/* from_mem */,
			queue_t*	/* to_disp */,
			u_node_t*	/* node */,
			queue_t*	/* to_tracer */);

/*^*/
/*------------------------ EXPANDER -----------------------------*/
/* Eigener Prozess zum Expandieren von Knoten. Liest aus der Queue
 * 'from_disp' jeweils einen (Vater-)Knoten, erzeugt dessen Soehne 
 * und versucht sie zu eliminieren. Leere Knoten zur Erzeugung der 
 * Soehne werden aus der Queue 'from_mem' gelesen. Die nicht 
 * eliminierten Soehne werden ueber die Queue 'to_disp' an den 
 * Dispatcher weitergegeben.
 * Wenn ein erzeugter Sohn eine neue Bestloesung (Incumbent) darstellt,
 * wird er (als solche markiert) an den Dispatcher weitergegeben.
 * Wenn ein Fehler auftritt, wird ein Fehlertoken an den Dispatcher
 * geschickt und der Prozess beendet.
 */ 

void expander (p, from_mem, to_disp, from_disp, u_glob, to_tracer) 

Process		*p;
queue_t		*from_mem;
queue_t		*to_disp;
queue_t		*from_disp;
u_glob_t	*u_glob;
queue_t		*to_tracer;

   {
    boolean		e_finished;
    node_t		*parent;
    int			sts;
    node_t		*son;
    int			last_selection;
    node_t		*incumbent;
    int			type;
    int			prof_slot;

    p = p;
    to_tracer = to_tracer;

    e_finished = OK_FALSE;

    /* Hole leeren Knoten fuer ersten Sohn: */
    sts = q_read_bl (from_mem, &type, (void **) &son, 
		     &m_finished);
    if (sts > ERROR || type == Q_ERROR)
	e_finished = sts;

    /* Solange nicht fertig */
    while (!e_finished)
       {
	/* Hole besten Knoten vom Dispatcher: */
	sts = q_read_bl (from_disp, &type, (void **) &parent, 
			 &d_finished);
	/* Wenn Dispatcher tot weil Finish-Token erhalten: */
	if (sts == Q_PARTNER_DEAD && d_finished == D_DONE)
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
	    TKN ("Vom Dispatcher erhalten", parent);
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
		    #ifdef TRACE_ON
			son->priority = 0;
		    #endif
		    TKN ("Sohn ist zulaessige Loesung:", son);

		    /* Incumbent holen: */
		    incumbent = h_read_incumbent ();
		    /* Wenn Sohn besser als incumbent */
		    if (son->value < incumbent->value)
		       {
			/* Verkuende neue incumbent: */
			q_write_bl (to_disp, Q_INC, 
				    son, &d_finished);
			T ("An Dispatcher als neue Incumbent");
			statistic.improved++;
			/* Hole Knoten fuer neuen Sohn: */
			sts = q_read_bl (from_mem, &type, 
					 (void **) &son, &m_finished);
			if (type == Q_ERROR)
			    sts = (int) son;
			B_ST
		       }
		    /* (Vergiss diesen Sohn) */
		   }
		else
		   {
		    B_ST
		    /* Versuche Sohn zu eliminieren: */
		    sts = try_pruning (u_glob, from_mem, 
				       to_disp, son->u_node,
				       to_tracer);
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
			q_write_bl (to_disp, Q_NODE, 
				    son, &d_finished);
			/* Hole Knoten fuer neuen Sohn: */
			sts = q_read_bl (from_mem, &type, 
					 (void **) &son, &m_finished);
			if (type == Q_ERROR)
			    sts = (int) son;
			B_ST
		       }
		    else
		       {
			/* (Vergiss diesen Sohn) */
			/* Uebertrage Werte fuer korrektes Tracing: */
			#ifdef TRACE_ON
			    /* Berechne Prioritaet fuer den Sohn: */
			    sts = u_priority (u_glob, son->u_node, 
					      glob.prio_par, 
					      &son->priority);
			    B_ST
			    /* Kopiere l_bound von u_node: */
			    sts = u_lower_bound (u_glob, son->u_node,
						 &son->l_bound);
			    B_ST
			#endif
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
	    q_write_bl (to_disp, Q_EMPTY_NODE, 
			parent, &d_finished);
	   } /* end else */
       } /* end while */

    /* Wenn ein Fehler aufgetreten ist: */
    if (sts > ERROR)
       {
	/* Fehlertoken an Dispatcher schicken: */
	q_write (to_disp, Q_ERROR, (void *) sts);
       }
    else
	e_finished = OK_TRUE;
   } 

/*^*/
/*------------------------ TRY_PRUNING --------------------------*/
/* Versucht, den Knoten 'node' zu eliminieren. Berechnet die Lower 
 * Bound; wenn diese groesser als die incumbent ist, kann der 
 * Knoten eliminiert werden. Sonst wird die Upper Bound berechnet. 
 * Ist diese kleiner als die incumbent, so liegt eine neue 
 * incumbent vor. Diese wird als solche markiert an den Dispatcher
 * weitergegeben und 'node' eliminiert. 
 * Sonst werden die Bounds verglichen: Ist die Lower Bound nicht 
 * kleiner als die Upper Bound, kann 'node' eliminiert werden. Der 
 * Rueckgabewert ist 'OK_TRUE', wenn 'node' eliminiert werden kann. 
 */

static int	try_pruning (u_glob, from_mem, to_disp, 
			       node, to_tracer)

u_glob_t	*u_glob;
queue_t		*from_mem;
queue_t		*to_disp;
u_node_t	*node;
queue_t		*to_tracer;

   {
    u_zf_t		l_bound;
    node_t		*incumbent;
    static node_t	*solution;
    int			sts;
    u_zf_t		value;
    int			type;
    extern glob_t	glob;

    to_tracer = to_tracer;

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
	sts = q_read_bl (from_mem, &type, 
			 (void **) &solution, &m_finished);
	if (type == Q_ERROR)
	    sts = (int) solution;
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
	    q_write_bl (to_disp, Q_INC, solution, &d_finished);
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
