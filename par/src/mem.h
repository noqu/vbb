/*---------------------------------------------------------------*/
/* mem.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Funktionstypen fuer mem.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  MEM_H
#define  MEM_H

#include  "error.h"

/*------------------------ FUNKTIONEN ---------------------------*/

extern	void	mem_server (
			Process*	/* p */, 
			queue_t**	/* inqs */, 
			queue_t**	/* outqs */, 
			u_glob_t*	/* u_glob */,
			int		/* min_level */,
			int		/* max_level */,
			queue_t*	/* to_tracer */);

#endif /* MEM_H */
/*------------------------ ENDE ---------------------------------*/
