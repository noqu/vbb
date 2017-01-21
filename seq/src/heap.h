/*---------------------------------------------------------------*/
/* heap.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Funktionstypen fuer heap.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  HEAP_H
#define  HEAP_H

#include  "error.h"

/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

extern node_t	*h_read_incumbent ();
extern boolean	h_get_from_heap ();
extern int	h_new_incumbent ();
extern boolean	h_init_incumbent ();
extern void	h_init_heap ();
extern void	h_put_on_heap ();
extern int	h_get_best_prio ();
extern int	h_heap_size ();

#else

extern node_t	*h_read_incumbent (void);

extern boolean	h_get_from_heap (
			node_t** 	/* node */);

extern int	h_new_incumbent (
			node_t*	/* new */);

extern boolean	h_init_incumbent (
			node_t*		/* new */);

extern void	h_init_heap (void);

extern void	h_put_on_heap (
			node_t*		/* node */);

extern int	h_get_best_prio (
			int*		/* prio*/);

extern int	h_heap_size (void);

#endif

#endif /* HEAP_H */
/*------------------------ ENDE ---------------------------------*/
