/*---------------------------------------------------------------*/
/* inout.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Server fuer den Zugriff auf die Links. Auf jedem Prozessor
 * laufen bis zu 4 In-Server und ebensoviele Out-Server
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

/* #define		TRACE_ON */
#define		TRACE_CONDITION		glob.flags & TRACE_INOUT

#include  <stdio.h>
#include  <stdlib.h>
#include  <process.h>
#include  <channel.h>
#include  "types.h"
#include  "u_types.h"
#include  "u_funcs.h"
#include  "queue.h"
#include  "error.h"
#include  "inout.h"
#include  "trace.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

extern int	d_finished;
extern int	m_finished;
extern int	in_finished[4];
extern int	out_finished[4];

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

public void	out (
			Process*	/* p */,
			int		/* nr */,
			u_glob_t*	/* u_glob */,
			Channel*	/* channel */,
			queue_t*	/* from_disp */,
			queue_t*	/* to_mem */,
			queue_t*	/* to_tracer */);

public void	in (
			Process*	/* p */,
			int		/* nr */,
			u_glob_t*	/* u_glob */,
			Channel*	/* channel */,
			queue_t*	/* from_mem */,
			queue_t*	/* to_disp */,
			queue_t*	/* to_tracer */);

/*^*/
/*------------------------ OUT ----------------------------------*/
/* Serverprozess, der die Queue 'from_disp' ueberwacht. Jeder darauf
 * eingehende Knoten wird in eine flache Datenstruktur kopiert, die 
 * auf den Kanal 'channel' gelegt wird. Der leere Knoten wird ueber
 * die Queue 'to_mem' an den memserver zurueckgegeben. 'nr' bezeichnet
 * die Nummer des Prozesses [0..3], 'size' die Problemgroesse.
 */

void	out (p, nr, u_glob, channel, from_disp, to_mem, to_tracer)

Process		*p;
int		nr;
u_glob_t	*u_glob;
Channel		*channel;
queue_t		*from_disp;
queue_t		*to_mem;
queue_t		*to_tracer;

   {
    int		*buf;
    int		*intbuf;
    char	*charbuf;
    int		type;
    int		buf_size;
    node_t	*node;
    void	*q_item;
    int		keep_node = OK_FALSE;

    p = p;
    to_tracer = to_tracer;

    /* Speicherplatz fuer flache Knoten bereitstellen: */
    buf_size = u_sizeof_flat_node (u_glob) + 3 * sizeof (int);
    buf = (int *) malloc (buf_size);
    if (!buf)
       {
	out_finished[nr] = IO_MALLOC;
	return;
       }

    /* intbuf zeigt als int-Zeiger auf den Anfang von buf: */
    intbuf = buf;
    /* charbuf zeigt als char-Zeiger auf buf + 3 * sizeof (int): */
    charbuf = (char *)(buf + 3);

    /* Endlosschleife fuer Server: */
    while (!out_finished[nr])
       {
        /* Wenn ein Knoten in der Eingangsqueue: */
	if (q_read (from_disp, &type, &q_item))
	    switch (type)
	       {
		/* Wenn irgendein Knoten: */
		case Q_INC:
		case Q_INIT_INC:
		    keep_node = OK_TRUE;
		    /* weiter wie bei */
		case Q_NODE:
		    node = (node_t *) q_item;
		    /* Knoten flachklopfen: */
		    u_flat_node (u_glob, node->u_node, charbuf);
		    /* Typ und k-node-Daten eintragen: */
		    intbuf[0] = node->priority;
		    intbuf[1] = node->value;
		    intbuf[2] = node->l_bound;
		    /* Flachen Knoten ueber Kanal ausgeben: */
		    ChanOutInt (channel, type);
		    ChanOut (channel, (void *) buf, buf_size);
		    TKN ("Knoten verschickt:", node)
		    if (!keep_node)
		       {
			/* Leeren Knoten an Memserver zurueckgeben: */
			q_write_bl (to_mem, Q_EMPTY_NODE, 
				    node, &m_finished);
		       }
		    keep_node = OK_FALSE;
		    break;
		/* Wenn irgendein Token: */
		case Q_ERROR:
		case Q_TOKEN:
		case Q_FINISH:
		    TV ("Token/Finish verschickt", (int) q_item);
		    ChanOutInt (channel, type);
		    ChanOutInt (channel, (int) q_item);
		    /* Wenn Fehlermeldung vom Dispatcher: */
		    if (type == Q_ERROR)
		       {
			/* Selbst abbrechen und Dispatcher beenden: */
			out_finished[nr] = (int) q_item;
			d_finished = (int) q_item;
		       }
		    if (type == Q_FINISH)
			out_finished[nr] = OK_TRUE;
		    break;
	       }
	else
	    ProcReschedule ();
       }
   }

/*^*/
/*------------------------ IN -----------------------------------*/
/* Serverprozess, der den Kanal 'channel' ueberwacht. Jeder darauf
 * eingehende flache Knoten wird in einen normalen (leeren) Knoten 
 * kopiert, der ueber die Queue 'from_mem' vom Memserver geholt wird. 
 * Der so gefueellte Knoten wird dann ueber die Queue 'to_disp' an 
 * den Dispatcher weitergegeben. 'nr' bezeichnet die Nummer des 
 * Prozesses [0..3], 'size' die Problemgroesse.
 */

void	in (p, nr, u_glob, channel, from_mem, to_disp, to_tracer)

Process		*p;
int		nr;
u_glob_t	*u_glob;
Channel		*channel;
queue_t		*from_mem;
queue_t		*to_disp;
queue_t		*to_tracer;

   {
    int		*buf;
    int		*intbuf;
    char	*charbuf;
    int		type;
    int		dummy;
    int		buf_size;
    int		sts = OK;
    node_t	*node;

    p = p;
    to_tracer = to_tracer;

    /* Speicherplatz fuer flache Knoten bereitstellen: */
    buf_size = u_sizeof_flat_node (u_glob) + 3 * sizeof (int);
    buf = (int *) malloc (buf_size);
    if (!buf)
       {
	in_finished[nr] = IO_MALLOC;
	return;
       }

    /* intbuf zeigt als int-Zeiger auf den Anfang von buf: */
    intbuf = buf;
    /* charbuf zeigt als char-Zeiger auf buf + 3 * sizeof (int): */
    charbuf = (char *)(buf + 3);

    /* Endlosschleife fuer Server: */
    while (!in_finished[nr])
       {
	type = ChanInInt (channel);
	switch (type)
	   {
	    /* Irgendein Knoten angekommen: */
	    case Q_NODE:
	    case Q_INC:
	    case Q_INIT_INC:
		/* Lies flachen Knoten aus Kanal (blockiert selbst): */
		ChanIn (channel, (void *) buf, buf_size);
		/* Hole leeren Knoten vom Memserver: */
		sts = q_read_bl (from_mem, &dummy, (void **) &node, 
				 &m_finished);
		/* Wenn Memserver einen Fehler meldet: */
		if (sts > ERROR || dummy == Q_ERROR)
		   {
		    /* Fehler an Dispatcher weitergeben: */
		    q_write (to_disp, Q_ERROR, (void *) sts);
		    /* Aber nicht selbst beenden !!! */
		    /* (Auf Fehlertoken v. Ringvorgaenger warten) */
		    continue;
		   }
		/* Mache richtigen Knoten aus flachem Knoten: */
		u_unflat_node (u_glob, charbuf, node->u_node);
		/* Hole k-node-Daten: */
		node->priority = intbuf[0];
		node->value = intbuf[1];
		node->l_bound = intbuf[2];
		TKN ("Knoten empfangen:", node)
		/* Gib Knoten an Dispatcher weiter: */
		q_write_bl (to_disp, type, node, &d_finished);
		break;
	    case Q_ERROR:
	    case Q_TOKEN:
	    case Q_FINISH:
		/* Tokenbesitzer empfangen: */
		node = (node_t *) ChanInInt (channel);
		TV ("Token empfangen", (int) node);
		/* Wenn Fehlermeldung empfangen: */
		if (type == Q_ERROR)
		   {
		    /* Selbst beenden: */
		    in_finished[nr] = (int) node;
		    T ("ERROR erhalten")
		   }
		if (type == Q_FINISH)
		   {
		    in_finished[nr] = OK_TRUE;
		    T ("FINISH erhalten")
		   }
		/* Token weitergeben an Dispatcher: */
		sts = q_write_bl (to_disp, type, (void *) node, &d_finished);
		TV("Token an Dispatcher weitergegeben, sts", sts)
		break;
	   }
       }
   }
	    
/*------------------------ ENDE ---------------------------------*/
