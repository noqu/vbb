/*---------------------------------------------------------------*/
/* disp.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93 					 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Dispatcher: Steuert die Kommunikation zwischen den Prozessoren.
 * Nimmt Knoten von in und expander entgegen, gibt Knoten an
 * out und expander weiter. Eigene Knoten werden auf Heap gelagert
 * (Code fuer Heap in heap.c)
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

/* #define		TRACE_ON */
#define		TRACE_CONDITION		glob.flags & TRACE_DISP

#include  <stdio.h>
#include  <stdlib.h>
#include  <process.h>
#include  <time.h>
#include  "types.h"
#include  "queue.h"
#include  "error.h"
#include  "disp.h"
#include  "heap.h"
#include  "trace.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

extern	stat_t		statistic;
extern	int		d_finished;
extern	int		m_finished;
extern	int		e_finished;
extern	int		out_finished[4];
extern	int		parallel;

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

public void	disp_server (
			Process*	/* p */, 
			queue_t*	/* to_exp */, 
			queue_t*	/* from_exp */, 
			queue_t*	/* to_mem */, 
			queue_t**	/* out */, 
			queue_t**	/* in */, 
			int		/* cpu_nr */,
			queue_t*	/* to_tracer */);

static int	to_all_neighbors (
			queue_t**	/* out */,
			int		/* type */,
			node_t*		/* node */,
			int*		/* out_finished */);

/*^*/
/*------------------------ DISP_SERVER --------------------------*/
/* Serverprozess fuer die Verwaltung der aktiven Knoten und der 
 * momentanen Bestloesung. Schickt dem Expander ueber die Queue
 * 'to_exp' jeweils den Knoten mit der groessten Prioritaet vom Heap
 * zur Expansion. Die expandierten Knoten werden vom Expander auf
 * der Queue 'from_exp' empfangen und auf dem Heap abgelegt. Wenn der
 * Expander eine neue Bestloesung findet, schickt er diese entsprechend 
 * markiert ueber die Queue 'from_exp'. Alle Knoten mit schlechterer
 * Lower Bound werden dann vom Heap entfernt und die leeren Knoten ueber
 * die Queue 'to_mem' an den Memserver geschickt. 
 */ 
  
void	disp_server (p, to_exp, from_exp, to_mem, 
		     out, in, cpu_nr, to_tracer)

Process		*p;
queue_t		*to_exp;
queue_t		*from_exp;
queue_t		*to_mem;
queue_t		**out;
queue_t		**in;
int		cpu_nr;
queue_t		*to_tracer;

   {
    int		served;
    node_t	*node;
    int		type;
    int		outstanding = 0;
    int		sts = OK;
    int		i;
    int		idle = OK_FALSE;
    int		i_have_a_token;
    int		token_creator = -1;
    int		sent_work = OK_TRUE;
    int		next_in_ring = 0;
    int		counter = 0;
    static int	last_sent_to[4];
    int		priority;
    extern glob_t	glob;
    int		idle_timer;
    static int	interval[4];
    int		just_sent_to = 0;
    int		fair_i;

    p = p;
    to_tracer = to_tracer;

    h_init_heap ();

    /* Nur ROOT-Transputer hat zu Beginn ein Token:*/
    if (!cpu_nr)
	i_have_a_token = OK_TRUE;
    else
	i_have_a_token = OK_FALSE;

    /* Verteilungsstrategie nimmt zunaechst alle als idle an */
    for (i = 0; i < 4; i++)
	interval[i] = glob.dist_idle;

    while (!d_finished)
       {
	served = OK_FALSE;

	/* Expander-Ausgabe bearbeiten: */
	/* Wenn ein Knoten in der Queue vom Expander: */
	if (q_read (from_exp, &type, (void **) &node))
	   {
	    switch (type)
	       {
		case Q_INIT_INC: 
		/* Erste Incumbent ueberhaupt: */
		    if (h_init_incumbent (node))
		       {
			TKN ("Incumbent initialisiert", node)
			/* Weitergeben an Nachbarn: */
			sts = to_all_neighbors (out, Q_INIT_INC,
						node, out_finished);
			sent_work = OK_TRUE;
		       }
		    break;
		case Q_INC: 
	        /* Neue Incumbent: */
		    TKN ("Neue Incumbent vom Expander", node)
		    sts = h_new_incumbent (node, to_mem);
		    B_ST
		    /* Wenn besser als alte: */
		    if (sts)
		       {
			/* Weitergeben an Nachbarn: */
			sts = to_all_neighbors (out, Q_INC,
						node, out_finished);
			sent_work = OK_TRUE;
		       }
		    break;
		case Q_NODE:
		/* Erzeugter Sohn, soll auf Heap: */
		    /* TKN ("Neuer Knoten vom Expander", node) */
		    h_put_on_heap (node);
		    counter++;
		    break;
		case Q_EMPTY_NODE:
		/* Fertig bearbeiteter Vater, soll an Memserver: */
		    if (outstanding)
			outstanding--;
		    sts = q_write_bl (to_mem, Q_EMPTY_NODE, 
				      node, &m_finished);
		    if (glob.flags & DIST_COUNT_FATHERS)
			counter++;
		    break;
		case Q_ERROR:
		/* Expander oder Memserver ist auf Fehler gelaufen: */
		    sts = ((int) node) + 10000 * cpu_nr;
		    break;
		default:
		    sts = Q_WRONG_TYPE;
	       }
	    B_ST
	    served = OK_TRUE;
	   }

	/* Expander-Eingabe nachfuellen: */
	if (!idle && q_length (to_exp) == 0 && !outstanding)
	   {
	    /* Wenn noch was auf dem Heap: */
	    if (h_get_from_heap (&node))
	       {
		/* TKN ("Knoten vom Heap an Expander", node) */
		q_write (to_exp, Q_NODE, node);
		outstanding++;
		served = OK_TRUE;
	       }
	    else /* (Heap ist leer) */
		/* Wenn auch keine Knoten mehr bei Exp. in Arbeit: */
		if (q_length (from_exp) == 0 && !outstanding)
		   {
		    if (!idle)
		       {
			sts = to_all_neighbors (out, Q_IDLE, NULL,
					        out_finished);
			B_ST
			T ("Going idle")
			idle_timer = (int) clock ();
		       }
		    idle = OK_TRUE;
		   }
	   }
	    
	/* Empfangene Knoten bearbeiten: */
	for (i = 0; in[i]; i++)
	   {
	    if (q_read (in[i], &type, (void**) &node))
	       {
		switch (type)
		   {
		    case Q_INIT_INC: 
		    /* Erste Incumbent ueberhaupt: */
			TKN ("Erste Incumbent von in", node)
			if (h_init_incumbent (node))
			   {
			    /* Weitergeben an Nachbarn: */
			    sts = to_all_neighbors (out, Q_INIT_INC,
						    node, out_finished);
			    sent_work = OK_TRUE;
			   }
			break;
		    case Q_INC: 
		    /* Neue Incumbent empfangen: */
			TKN ("Neue Incumbent von in", node)
			sts = h_new_incumbent (node, to_mem);
			B_ST
			if (sts)
			   {
			    /* Weitergeben an Nachbarn: */
			    sts = to_all_neighbors (out, Q_INC,
						    node, out_finished);
			    sent_work = OK_TRUE;
			   }
			break;
		    case Q_NODE:
		    /* Empfangener Knoten, soll auf Heap: */
			if (idle)
			   {
			    statistic.idle += ((int) clock ()) 
					       - idle_timer;
			    idle = OK_FALSE;
			   }
			TKN ("Knoten von in", node)
			if (h_get_best_prio (&priority))
			    interval[i] = (node->priority > priority)
			    		? glob.dist_low
			    		: glob.dist_high;
			statistic.got++;
			h_put_on_heap (node);
			break;
		    case Q_TOKEN:
		    /* Token zur Termination-Detection: */
			i_have_a_token = OK_TRUE;
			token_creator = (int) node;
			TV ("Token von in", token_creator);
			break;
		    case Q_IDLE:
		    /* Sender ist idle: */
			interval[i] = glob.dist_idle;
			break;
		    case Q_FINISH:
		    /* Aufforderung zum Beenden: */
			token_creator = (int) node;
			TV ("Finish von in", token_creator);
			/* Wenn nicht ich der Urheber bin: */
			if (token_creator != cpu_nr)
			   {
			    /* Aufforderung weitergeben: */
			    sts = q_write_bl (out[next_in_ring], 
					  Q_FINISH, 
					  (void *) token_creator, 
					  &out_finished[next_in_ring]);
			    TV ("Finish weitergegeben", token_creator);
			   }
			/* Ich selber kann jetzt Schluss machen: */
			d_finished = D_DONE;
			break;
		    case Q_ERROR:
		    /* Memserver oder Ringvorgaenger meldet Fehler: */
			sts = (int) node;
			if (sts < 10000)
			    sts += 10000 * cpu_nr;
			TV ("Error von in", sts);
			break;
		    default:
			sts = Q_WRONG_TYPE;
		   }
		served = OK_TRUE;
		B_ST
		if (d_finished)
		    break;
	       }
	   }
	B_ST
	if (d_finished)
	    break;

	/* Verschicke Knoten an Nachbarn, wenn noetig: */
	if (h_heap_size () != 0 && parallel)
	    for (i = 0; i < 4 
			&& h_heap_size () > glob.min_heap_size; i++)
	       {
		fair_i = (just_sent_to + 1 + i) % 4; 
		if (out[fair_i] 
		    && counter - last_sent_to[fair_i] 
		       >= interval[fair_i])
		   {
		    just_sent_to = fair_i;
		    /* Hole Knoten vom Heap: */
		    h_get_from_heap (&node);
		    last_sent_to[fair_i] = counter;
		    h_get_best_prio (&priority);
		    interval[fair_i] = (node->priority > priority)
				? glob.dist_low
				: glob.dist_high;
		    /* Versuche Knoten an Adressaten zu verschicken: */
		    if (q_write (out[fair_i], Q_NODE, node))
		       {
			TV ("Knoten abgegeben an", fair_i)
			TKN ("Knoten an out", node)
			sent_work = OK_TRUE;
			served = OK_TRUE;
			statistic.sent++;
			/* Max. einen pro Durchgang abgeben: */
			break;
		       }
		    else
		       {
			/* Lege Knoten zurueck auf Heap (sonst weg): */
			h_put_on_heap (node);
		       }
		   }
	       }

	/* Wenn idle, Token bearbeiten: */
	if (idle && i_have_a_token && parallel)
	   {
	    /* Wenn was verschickt seit letztem Tokendurchgang: */
	    if (sent_work)
	       {
		/* Neuer Tokendurchgang startet: */
		sent_work = OK_FALSE;
		/* Neues eigenes Token schicken: */
		sts = q_write_bl (out[next_in_ring], Q_TOKEN, 
			          (void *) cpu_nr, 
			          &out_finished[next_in_ring]);
		B_ST
		TV ("Neues Token an out", cpu_nr)
		/* Ich habe kein Token mehr: */
		i_have_a_token = OK_FALSE;
	       }
	    else 
	       {
		/* Wenn eigenes Token wiedergekommen: */
		if (token_creator == cpu_nr)
		   {
		    /* Wir sind fertig ! */
		    /* Broadcaste die gute Nachricht: */
		    sts = q_write_bl (out[next_in_ring], Q_FINISH, 
				      (void *) cpu_nr, 
				      &out_finished[next_in_ring]);
		    B_ST
		    TV ("Finish-Meldung an out", cpu_nr)
		    /* (Selbst erst nach 1 Runde Schluss machen) */
		    i_have_a_token = OK_FALSE;
		   }
		else
		   {
		    /* Gib Token unveraendert weiter: */
		    sts = q_write_bl (out[next_in_ring], Q_TOKEN, 
				      (void *) token_creator, 
				      &out_finished[next_in_ring]);
		    B_ST
		    TV ("Token weitergegeben an out", token_creator)
		    /* Ich habe kein Token mehr: */
		    i_have_a_token = OK_FALSE;
		   }
	       }
	   }

	/* Im sequentiellen Fall reicht ein idle zum Abbruch: */
	if (idle && !parallel)
	    d_finished = D_DONE;

	/* Reschedule Prozess, wenn in dieser Runde nix getan: */
	if (!served)
	    ProcReschedule ();

	B_ST
       }

    /* Wenn ein Fehler passiert ist: */
    if (sts > ERROR)
       {
	TV ("Dispatcher hat Fehlerstatus", sts)
	if (sts < 10000)
	    sts += 10000 * cpu_nr;
	/* Fehler weitergeben an Ringnachfolger: */
	/* (out[0] schaltet den Dispatcher danach ab) */
	q_write_bl (out[next_in_ring], Q_ERROR, (void *) sts,
		    &out_finished[next_in_ring]);
	T ("Error gemeldet an out")
       }

    if (d_finished != D_DONE)
	TV ("Dispatcher stopped", d_finished);
   }

/*------------------------ TO_ALL_NEIGHBORS ---------------------*/
/* Verschickt einen Knoten an alle Nachbarn, die in out eingetragen
 * sind. Liefert den Status von q_write_bl () zurueck.
*/

int	to_all_neighbors (out, type, node, out_finished)

queue_t		**out;
int		type;
node_t		*node;
int		*out_finished;

   {
    int		i;
    int		sts = OK;

    for (i = 0; out[i]; i++)
       {
	sts = q_write_bl (out[i], type, node, &out_finished[i]);
	B_ST
       }
    return (sts);
   }

/*------------------------ ENDE ---------------------------------*/
