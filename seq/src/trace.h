/*---------------------------------------------------------------*/
/* trace.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Makros zum Ausgeben von Traceinformationen. Werden nur 
 * mitcompiliert, wenn im betreffenden Modul TRACE_ON definiert ist. 
 * Werden nur aufgerufen, wenn zusatzlich TRACE_CONDITION erfuellt
 * ist (Einstellung ueber Parameterdatei und glob.flags). Die
 * Ausgabefunktionen muessen vom als Benutzerfunktionen zum Kern
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
			      trace_string (STR); \
			}

#define  TV(STR, VAL)	{ extern glob_t		glob; \
			  if (TRACE_CONDITION) \
			      trace_value (STR, VAL); \
			}

#define  TA(ARR, LEN)	{ extern glob_t		glob; \
			  if (TRACE_CONDITION) \
			      trace_array (ARR, LEN); \
			}

#define  TKN(STR, NODE)	{ extern glob_t		glob; \
			  if (TRACE_CONDITION) \
			      trace_node (STR, NODE); \
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
#ifdef TRACE_ON
#ifdef SUN_CC

extern void	trace_string ();
extern void	trace_value ();
extern void	trace_array ();
extern void	trace_node ();

#else

extern void	trace_string (
			char*		/* str */); 

extern void	trace_value (
			char*		/* str */, 
			int		/* val */);

extern void	trace_array (
			int*		/* arr */,
			int		/* len */);

extern void	trace_node (
			char*		/* str */, 
			node_t*		/* node */);

#endif
#endif /* TRACE_ON */
#endif /* TRACE_H */
/*------------------------ ENDE ---------------------------------*/
