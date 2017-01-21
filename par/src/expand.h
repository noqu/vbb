/*---------------------------------------------------------------*/
/* expand.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Funktionstypen fuer expand.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  EXPAND_H
#define  EXPAND_H

#include  "error.h"

/*------------------------ FUNKTIONEN ---------------------------*/

extern void	expander (
			Process*	/* p */,
			queue_t*	/* from_mem */,
			queue_t*	/* to_heap */,
			queue_t*	/* from_heap */,
			u_glob_t*	/* u_glob */,
			queue_t*	/* to_tracer */);
			
#endif /* EXPAND_H */
/*------------------------ ENDE ---------------------------------*/
