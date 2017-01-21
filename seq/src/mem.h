/*---------------------------------------------------------------*/
/* mem.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Funktionstypen fuer mem.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  MEM_H
#define  MEM_H

#include  "error.h"

/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

extern	int	m_get_node ();
extern	void	m_free_node ();

#else

extern	int	m_get_node (
			u_glob_t*	/* u_glob */,
			node_t**	/* node */);

extern	void	m_free_node (
			node_t*		/* node */);

#endif

#endif /* MEM_H */
/*------------------------ ENDE ---------------------------------*/
