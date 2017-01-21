/*---------------------------------------------------------------*/
/* read.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Funktionstypen fuer read.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  READ_H
#define  READ_H

#include        "set.h"

/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

extern int	kl_input ();
extern int	matrin ();
extern int	read_param ();
extern int	init_params ();

#else

extern int	kl_input (
			FILE*		/* idat */,
			int*		/* Xsize */,
			int*		/* Ysize */,
			int**		/* values */,
			set_t***	/* pred */,
			set_t***	/* succ */);

extern int	matrin (
			FILE*		/* indat */, 
			int***		/* matrix */, 
			int*		/* zeilen */, 
			int*		/* spalten */);

extern int	read_param (
			char*		/* para */, 
			char**		/* val */);
	
extern int	init_params (
			char**		/* buffer */,
			int		/* ac */,
			char**		/* av */);
#endif

#endif /* READ_H */
/*------------------------ ENDE ---------------------------------*/
