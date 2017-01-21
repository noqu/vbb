/*---------------------------------------------------------------*/
/* mem.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Memserver zur Versorgung aller anderen Prozesse mit leeren 
 * Knoten. Verwaltet eine Freelist von Leerknoten, sowie eine
 * Sicherheitsreserve fuer den Betrieb bei Speicherknappheit. 
 * (dann Umschaltung des Heaps auf LIFO).
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

/* #define		TRACE_ON */
#define		TRACE_CONDITION		glob.flags & TRACE_MEM

#define  	ANZ_BLOECKE	128

#include  <stdio.h>
#include  <stdlib.h>
#include  <process.h>
#include  <misc.h>
#include  "types.h"
#include  "u_types.h"
#include  "u_funcs.h"
#include  "queue.h"
#include  "error.h"
#include  "mem.h"
#include  "trace.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

static	node_t		*freelist;
static	node_t		*reserve;
extern	stat_t		statistic;
extern  glob_t		glob;

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

public	void	mem_server (
			Process*	/* p */, 
			queue_t**	/* inqs */, 
			queue_t**	/* outqs */, 
			u_glob_t*	/* u_glob */,
			int		/* min_level */,
			int		/* max_level */,
			queue_t*	/* to_tracer */);

static	int	m_get_node (
			u_glob_t*	/* u_glob */,
			node_t**	/* node */);

static	void	m_free_node (
			node_t*		/* node */);

static	int	m_init_mem (
			u_glob_t*	/* u_glob */);

static	int	m_init_reserve (
			u_glob_t*	/* u_glob */);

/*^*/
/*------------------------ MEM_SERVER ---------------------------*/
/* Serverprozess zur Verwaltung der leeren Knoten. Leert alle
 * eingehenden Queues und legt die leeren Knoten auf einer Free-Liste
 * ab. Die ausgehenden Queues werden staendig mit leeren Knoten 
 * aufgefuellt. Prozess endet, wenn ein Fehler auftritt.
 */

void	mem_server (p, inqs, outqs, u_glob, min_level, 
		    max_level, to_tracer)

Process		*p;
queue_t		**inqs;
queue_t		**outqs;
u_glob_t	*u_glob;
int		min_level;
int		max_level;
queue_t		*to_tracer;

   {
    extern	int		m_finished;
    queue_t	**q;
    node_t	*node_got;
    node_t	*node_to_send;
    int		type;
    int		sts;
    int		written = OK_FALSE;
    int		out_of_memory = OK_FALSE;

    p = p;
    to_tracer = to_tracer;

    sts = m_get_node (u_glob, &node_to_send);
    if (sts > ERROR )
	m_finished = sts;

    while (!m_finished)
       {
	/* Fuelle alle Outqueues unter min_level auf: */
	for (q = outqs; *q; q++)
	   {
	    /* Wenn zu wenig Nodes in der Queue: */
	    if (q_length (*q) < min_level)
	       {
		/* Fuelle die Queue bis zum Rand auf: */
		do
		   {
		    /* Wenn kein Speicher da, fuelle mit Fehlertokens */
		    if (out_of_memory)
			written = q_write (*q, Q_ERROR, 
					   (void *) (M_INIT_MEM));
		    else
			written = q_write (*q, Q_EMPTY_NODE, 
					   (void *) node_to_send);
		    /* Wenn etwas nachgefuellt wurde: */
		    if (written && !out_of_memory)
		       {
			/* Hole Nachschub aus Freelist: */
			sts = m_get_node (u_glob, &node_to_send);
			if (sts > ERROR)
			    out_of_memory = OK_TRUE;
			T("m_get_node ()")
		       }
		   } while (written);
		TV ("Filled Queue", (int)(q - outqs))
	       }
	   }

	/* Leere alle ueberfuellten Inqueues: */
	for (q = inqs; *q; q++)
	   {
	    /* Wenn zu viele Nodes in Queue: */
	    if (q_length (*q) > max_level)
	       {
		/* Leere Queue komplett: */
		while (q_read (*q, &type, (void **) &node_got))
		    if (type != Q_ERROR)
		       {
			m_free_node (node_got);
			T("m_free_node ()")
		       }
		TV ("Drained Queue", (int)(q - inqs))
	       }
	   }
	
	/* Auf jeden Fall Zeitscheibe abgeben: */
	ProcReschedule ();
       }

    if (m_finished != OK_TRUE)
	TV ("Memserver stopped", m_finished);
   }

/*^*/
/*------------------------ M_GET_NODE ---------------------------*/
/* Liefert einen neuen Knoten aus der Freelist. Falls diese leer 
 * ist, wird m_init_mem aufgerufen. Beim ersten Aufruf wird 
 * grundsaetzlich m_init_mem aufgerufen, so dass keine vorherige 
 * Initialisierung noetig ist. 
 * (Interne Funktion, nur vom Server aufgerufen)
 */

static	int	m_get_node (u_glob, node)

u_glob_t	*u_glob;
node_t		**node;

   {
    int		sts;

    /* Wenn freelist leer, nachallokieren: */
    if (!freelist)
	{
	 sts = m_init_mem (u_glob);
	 R_ST
	}
    /* Spitze der freelist abschneiden und ausliefern: */
    *node = freelist;
    freelist = freelist->leftson;
    (*node)->leftson = (node_t *) 0;
    (*node)->rightson = (node_t *) 0;
    (*node)->father = (node_t *) 0;
    return (OK);
   }

/*^*/
/*------------------------ M_FREE_NODE --------------------------*/
/* Haengt den Knoten node vorne in die Freelist ein.
 * (Interne Funktion, nur vom Server aufgerufen)
 */

static	void	m_free_node (node)

node_t		*node;

   {
    /* Node in freelist einhaengen: */
    node->leftson = freelist;
    freelist = node;
   }

/*^*/
/*------------------------ M_INIT_MEM ---------------------------*/
/* Fuellt die freelist mit ANZ_BLOECKE Bloecken neu auf.
 * (Interne Funktion, nur vom Server aufgerufen)
 */

static int	m_init_mem (u_glob)

u_glob_t	*u_glob;

   {
    int		i;
    int		sts = OK;
    static int	reserve_used;
    u_node_t	*u_list[ANZ_BLOECKE];

    /* Im ersten Durchgang Reserve beiseitelegen: */
    if (!reserve && !reserve_used)
	sts = m_init_reserve (u_glob);
    R_ST

    /* Allokiere eine Menge von Bloecken: */
    freelist = (node_t *) malloc (sizeof (node_t) * ANZ_BLOECKE);
    if (!freelist)
	sts = M_INIT_MEM;

    /* Allokiere eine Menge von u_node-Bloecken: */
    i = u_create_node (u_glob, ANZ_BLOECKE, u_list);
    if (i != ANZ_BLOECKE)
	sts = M_INIT_MEM;

    /* Wenn kein Speicher mehr da: */
    if (sts == M_INIT_MEM)
       {
	/* Wenn noch Reserve vorhanden: */
	if (!reserve_used && reserve)
	   {
	    /* Verwende Reserve: */
	    freelist = reserve;
	    reserve_used = OK_TRUE;
	    /* Teile Heap mit, dass es nicht mehr gibt: */
	    glob.max_heap_size = statistic.allocated;
	    statistic.allocated += glob.reserve_size;
	    reserve = 0;
	    return (OK);
	   }
	/* Sonst (keine Reserve mehr): */
	else
	    return (M_INIT_MEM);
       }
    /* Sonst (Speicher wurde hier erfolgreich allokiert) */
    else
       {
	/* Baue daraus eine freelist auf: */
	for (i = 0; i < ANZ_BLOECKE; i++)
	   {
	    freelist[i].leftson = freelist + i + 1;
	    freelist[i].u_node = u_list[i];
	   }
	freelist[ANZ_BLOECKE - 1].leftson = (node_t *) 0;
	statistic.allocated += ANZ_BLOECKE;
	return (OK);
       }
   }

/*------------------------ M_INIT_RESERVE -----------------------*/
/* Fuellt die Reserveliste mit glob.reserve_size Bloecken.
 * (Interne Funktion, nur (1x) vom Server aufgerufen)
 */

static int	m_init_reserve (u_glob)

u_glob_t	*u_glob;

   {
    int		i;
    u_node_t	**u_list;

    /* Platz fuer u_Liste allokieren: */
    u_list = (u_node_t **) malloc (glob.reserve_size 
    				   * sizeof (u_node_t *));
    if (!u_list)
	return (M_INIT_MEM);

    /* Allokiere eine Menge von Bloecken: */
    reserve = (node_t *) malloc (sizeof (node_t) * glob.reserve_size);
    if (!reserve)
	return (M_INIT_MEM);

    /* Allokiere eine Menge von u_node-Bloecken: */
    i = u_create_node (u_glob, glob.reserve_size, u_list);
    if (i != glob.reserve_size)
	return (M_INIT_MEM);

    /* Baue daraus eine Reserve-Liste auf: */
    for (i = 0; i < glob.reserve_size; i++)
       {
	reserve[i].leftson = reserve + i + 1;
	reserve[i].u_node = u_list[i];
       }
    reserve[glob.reserve_size - 1].leftson = (node_t *) 0;
    
    free (u_list);

    return (OK);
   }

/*------------------------ ENDE ---------------------------------*/
