/*---------------------------------------------------------------*/
/* heap.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Funktionstypen fuer heap.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  HEAP_H
#define  HEAP_H

#include  "error.h"

/*------------------------ FUNKTIONEN ---------------------------*/

extern node_t	*h_read_incumbent (void);

extern boolean	h_get_from_heap (
			node_t** 	/* node */);

extern int	h_new_incumbent (
			node_t*	/* new */,
			queue_t*	/* to_mem */);

extern boolean	h_init_incumbent (
			node_t*		/* new */);

extern void	h_init_heap (void);

extern void	h_put_on_heap (
			node_t*		/* node */);

extern int	h_get_best_prio (
			int*		/* prio*/);

extern int	h_heap_size (void);

#endif /* HEAP_H */
/*------------------------ ENDE ---------------------------------*/
