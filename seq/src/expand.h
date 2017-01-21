/*---------------------------------------------------------------*/
/* expand.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Funktionstypen fuer expand.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  EXPAND_H
#define  EXPAND_H

#include  "error.h"

/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

extern int	expander ();

#else

extern int	expander (
			u_glob_t*	/* u_glob */);
#endif
			
#endif /* EXPAND_H */
/*------------------------ ENDE ---------------------------------*/
