/*---------------------------------------------------------------*/
/* trace.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.0 seq	29.3.93					 */
/*---------------------------------------------------------------*/
/* Generische Trace-Funktionen (sequentielle Version)
 */
/*------------------------ PRAEPROCESSOR ------------------------*/

#include  <stdio.h>
#include  <stdlib.h>
/* #include  <string.h> */
#include  "u_types.h"
#ifndef SUN_CC
#include  "u_funcs.h"
#endif
#include  "types.h"
#include  "trace.h"
#include  "error.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/
/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

public void	trace_string ();
public void	trace_value ();
public void	trace_array ();
public void	trace_node ();

#else

public void	trace_string (
			char*		/* str */); 

public void	trace_value (
			char*		/* str */, 
			int		/* val */);

public void	trace_array (
			int*		/* arr */,
			int		/* len */);

public void	trace_node (
			char*		/* str */, 
			node_t*		/* node */);

#endif

/*------------------------ TRACE_STRING -------------------------*/

void	trace_string (str)

char		*str;

   {
    fprintf (stderr, "%s\n", str);
   }

/*------------------------ TRACE_VALUE --------------------------*/

void	trace_value (str, val)

char		*str;
int		val;

   {
    fprintf (stderr, "%s: %d\n", str, val);
   }

/*------------------------ TRACE_ARRAY --------------------------*/

void	trace_array (arr, len)

int		*arr;
int		len;

   {
    int		i;

    for (i = 0; i < len; i++)
	fprintf (stderr, "%4d", arr[i]);
    fprintf (stderr, "\n");
   }

/*------------------------ TRACE_NODE ---------------------------*/

void	trace_node (str, node)

char		*str;
node_t		*node;

   {
    char	*trace_buf;

    trace_buf = (char *) malloc (TRACE_BUF_SIZE);
    if (trace_buf)
       {
	fprintf (stderr, "%s\n", str);
	fprintf (stderr, "----- KNOTEN -----\n");
	u_trace_node (node->u_node, trace_buf);
	fprintf (stderr, "%s", trace_buf);
	free (trace_buf);
	fprintf (stderr, "Prioritaet         :%8d\n", 
		       node->priority);
	fprintf (stderr, "Lower Bound        :%8d\n", 
		       node->l_bound);
	fprintf (stderr, "Value              :%8d\n", 
		       node->value);
	fprintf (stderr, "\n");
       }
   }

/*------------------------ ENDE ---------------------------------*/
