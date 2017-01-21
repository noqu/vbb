/*---------------------------------------------------------------*/
/* disp.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Funktionstypen fuer disp.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  DISP_H
#define  DISP_H

#include  "error.h"

/*------------------------ FUNKTIONEN ---------------------------*/

extern void	disp_server (
			Process*	/* p */, 
			queue_t*	/* to_exp */, 
			queue_t*	/* from_exp */, 
			queue_t*	/* to_mem */, 
			queue_t**	/* out */, 
			queue_t**	/* in */, 
			int		/* cpu_nr */,
			queue_t*	/* to_tracer */);

#endif /* DISP_H */
/*------------------------ ENDE ---------------------------------*/
