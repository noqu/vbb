/*---------------------------------------------------------------*/
/* trace.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.2		1.6.93					 */
/*---------------------------------------------------------------*/
/* Generische Trace-Funktionen (parallele Version)
 */
/*------------------------ PRAEPROCESSOR ------------------------*/

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <time.h>
#include  "u_types.h"
#include  "u_funcs.h"
#include  "types.h"
#include  "queue.h"
#include  "trace.h"
#include  "error.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

static	int	dummy = 0;

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

public void	trace_string (
			char*		/* str */, 
			queue_t*	/* to_tracer */);

public void	trace_value (
			char*		/* str */, 
			int		/* val */,
			queue_t*	/* to_tracer */);

public void	trace_array (
			int*		/* arr */,
			int		/* len */,
			queue_t*	/* to_tracer */);

public void	trace_node (
			char*		/* str */, 
			node_t*		/* node */,
			queue_t*	/* to_tracer */);

/*------------------------ TRACE_STRING -------------------------*/
/* Schickt ein Trace-Paket mit einem String an 'startup'
 */

void	trace_string (str, to_tracer)

char		*str;
queue_t		*to_tracer;

   {
    trace_t	*trace_buf;

    trace_buf = (trace_t *) malloc (TRACE_BUF_SIZE);
    if (trace_buf)
       {
	trace_buf->time = (int) clock ();
	sprintf (trace_buf->str, "%s\n", str);
       }
    q_write_bl (to_tracer, (int) strlen (trace_buf->str), 
		trace_buf, &dummy);
   }

/*------------------------ TRACE_VALUE --------------------------*/
/* Schickt ein Trace-Paket mit einem String und einem Wert
 * an 'startup'
 */

void	trace_value (str, val, to_tracer)

char		*str;
int		val;
queue_t		*to_tracer;

   {
    trace_t	*trace_buf;

    trace_buf = (trace_t *) malloc (TRACE_BUF_SIZE);
    if (trace_buf)
       {
	trace_buf->time = (int) clock ();
	sprintf (trace_buf->str, "%s: %d\n", str, val);
       }
    q_write_bl (to_tracer, (int) strlen (trace_buf->str), 
		trace_buf, &dummy);
   }

/*------------------------ TRACE_ARRAY --------------------------*/
/* Schickt ein Trace-Paket mit einem Array der Laenge 'len'
 * an 'startup'
 */

void	trace_array (arr, len, to_tracer)

int		*arr;
int		len;
queue_t		*to_tracer;

   {
    trace_t	*trace_buf;
    char	*wp;
    int		i;

    trace_buf = (trace_t *) malloc (TRACE_BUF_SIZE);
    if (trace_buf)
       {
	trace_buf->time = (int) clock ();
	wp = trace_buf->str;
	for (i = 0; i < len; i++)
	    wp += sprintf (wp, "%4d", arr[i]);
	sprintf (wp, "\n");
       }
    q_write_bl (to_tracer, (int) strlen (trace_buf->str), 
		trace_buf, &dummy);
   }

/*------------------------ TRACE_NODE ---------------------------*/
/* Schickt ein Trace-Paket mit einem Knoten an 'startup'. Der u_node
 * wird dabei durch die Funktion u_trace_node() dargestellt.
 */

void	trace_node (str, node, to_tracer)

char		*str;
node_t		*node;
queue_t		*to_tracer;

   {
    trace_t	*trace_buf;
    char	*wp;

    trace_buf = (trace_t *) malloc (TRACE_BUF_SIZE);
    if (trace_buf)
       {
	trace_buf->time = (int) clock ();
	wp = trace_buf->str;

	wp += sprintf (wp, "%s\n", str);
	wp += sprintf (wp, "----- KNOTEN -----\n");
	wp = u_trace_node (node->u_node, wp);
	wp += sprintf (wp, "Prioritaet         :%8d\n", 
		       node->priority);
	wp += sprintf (wp, "Lower Bound        :%8d\n", 
		       node->l_bound);
	wp += sprintf (wp, "Value              :%8d\n", 
		       node->value);
	wp += sprintf (wp, "\n");
       }
    q_write_bl (to_tracer, (int) strlen (trace_buf->str), 
		trace_buf, &dummy);
   }

/*------------------------ ENDE ---------------------------------*/
