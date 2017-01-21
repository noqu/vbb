/*---------------------------------------------------------------*/
/* inout.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Funktionstypen fuer inout.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  INOUT_H
#define  INOUT_H

#include  "error.h"

/*------------------------ FUNKTIONEN ---------------------------*/

extern void	out (
			Process*	/* p */,
			int		/* nr */,
			u_glob_t*	/* u_glob */,
			Channel*	/* channel */,
			queue_t*	/* from_disp */,
			queue_t*	/* to_mem */,
			queue_t*	/* to_tracer */);

extern void	in (
			Process*	/* p */,
			int		/* nr */,
			u_glob_t*	/* u_glob */,
			Channel*	/* channel */,
			queue_t*	/* from_mem */,
			queue_t*	/* to_disp */,
			queue_t*	/* to_tracer */);

#endif /* INOUT_H */
/*------------------------ ENDE ---------------------------------*/
