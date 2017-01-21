/*---------------------------------------------------------------*/
/* trace.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Makros zum Ausgeben von Traceinformationen. Werden nur 
 * mitcompiliert, wenn im betreffenden Modul TRACE_ON definiert ist. 
 * Werden nur aufgerufen, wenn zusatzlich TRACE_CONDITION erfuellt
 * ist (Einstellung ueber Parameterdatei und glob.flags). Die
 * Ausgabefunktion u_trace_node() muss als Benutzerfunktionen zum Kern
 * gebunden werden.
 */
/*------------------------ PRAEPROZESSOR ------------------------*/
#ifndef	TRACE_H
#define	TRACE_H

#define		TRACE_BUF_SIZE	1024	/* QQQ oder wieviel ?	*/

/*------------------------ MAKROS -------------------------------*/

#ifdef  TRACE_ON

#define  T(STR)		{ extern glob_t		glob; \
			  if (TRACE_CONDITION) \
			      trace_string (STR, to_tracer); \
			}

#define  TV(STR, VAL)	{ extern glob_t		glob; \
			  if (TRACE_CONDITION) \
			      trace_value (STR, VAL, to_tracer); \
			}

#define  TA(ARR, LEN)	{ extern glob_t		glob; \
			  if (TRACE_CONDITION) \
			      trace_array (ARR, LEN, to_tracer); \
			}

#define  TKN(STR, NODE)	{ extern glob_t		glob; \
			  if (TRACE_CONDITION) \
			      trace_node (STR, NODE, to_tracer); \
			}

#else
#define  T(STR)		{ ; /* nix */ }
#define  TV(STR, VAL)	{ ; /* nix */ }
#define  TA(ARR, LEN)	{ ; /* nix */ }
#define  TKN(STR, NODE)	{ ; /* nix */ }
#endif

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

typedef struct	trace_st
   {
    int			processor;
    int			time;
    char		str[1];
   }
trace_t;

/*------------------------ FUNKTIONEN ---------------------------*/

extern void	trace_string (
			char*		/* str */, 
			queue_t*	/* to_tracer */);

extern void	trace_value (
			char*		/* str */, 
			int		/* val */,
			queue_t*	/* to_tracer */);

extern void	trace_array (
			int*		/* arr */,
			int		/* len */,
			queue_t*	/* to_tracer */);

extern void	trace_node (
			char*		/* str */, 
			node_t*		/* node */,
			queue_t*	/* to_tracer */);

#endif /* TRACE_H */
/*------------------------ ENDE ---------------------------------*/
