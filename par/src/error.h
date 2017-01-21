/*---------------------------------------------------------------*/
/* error.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.3.93  				 */
/* Version 4.0							 */
/*---------------------------------------------------------------*/
/* Definiert Fehlernummern, Flags fuer glob und Fehlermakros.
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef ERROR_H
#define	 ERROR_H

#include  <stdio.h>

#define  OK		0
#define  OK_TRUE	1
#define  OK_FALSE	0
#define  ERROR		2

#define  D_DONE		 10
#define  IO_MALLOC	 11
#define  M_INIT_MEM	 12
#define	 Q_MAKE_QUEUE	 13
#define	 Q_WRONG_TYPE	 14
#define	 Q_PARTNER_DEAD	 15
#define  S_PROC_ALLOC	 16
#define  READ_ERR	 17
#define  U_FUNCS_ERR	200

#define  TRACE_DISP		1
#define  TRACE_EXP		2
#define  TRACE_HEAP		4
#define  TRACE_INOUT		8
#define  TRACE_MEM		16
#define  TRACE_STARTUP		32
#define  ELIM_BY_HEUR		64
#define	 START_HEUR		128
#define	 SINGLE_CPU		256
#define	 DIST_COUNT_FATHERS	512
#define	 PROFILE		1024
#define	 STATISTIC		2048
#define	 STAT_SUMS		8192
#define	 STAT_KEEP	       16384
#define	 NO_SOLUTION	       32768

#define  MAXINT		((unsigned) (~0)/2)

#define	 public	

#ifndef SUN_CC
extern int	print_error (
			int		/* p */,
			int		/* sts */);
#endif

/*------------------------ MAKROS -------------------------------*/
/*------------------------ SPRINTF ------------------------------*/
/* Wegen unterschiedlicher Definition in SysV und BSD: */

#ifdef SUN_CC
#define SPRINTF(ARGS)	(strlen (sprintf ARGS))
#else
#define SPRINTF(ARGS)	(sprintf ARGS)
#endif

/*------------------------ PRINT_ERROR --------------------------*/
/* Ausgabe von Fehlern als Text: */

#define	 P_FEHLER(STS) \
   { \
    print_error ((STS) / 10000, (STS) % 10000); \
   }

/*------------------------ FEHLER -------------------------------*/
/* Aufruf einer Fehlerausgabe ohne Bedingung, sts etc:
 */
#define	 FEHLER(STS) \
       { \
	P_FEHLER (STS); \
	exit_terminate (STS); \
       }
    
/*------------------------ R_ST ---------------------------------*/
/* Fehlerbehandlung in Funktionen ausserhalb main: gibt den Status
 * an aeussere Funktion weiter.
 */

#define  R_ST	\
   { \
    if (sts > ERROR) \
	return (sts); \
   }

/*------------------------ P_ST ---------------------------------*/
/* Fehlerbehandlung in main: Schreibt Fehlermeldung und beendet
 * den Prozess mit exit.
 */

#define  P_ST	\
   { \
    if (sts > ERROR) \
       FEHLER (sts) \
   }
   
/*------------------------ B_ST ---------------------------------*/
/* Fehlerbehandlung fuer Server: Verlaesst Schleife mit 'break'
 */

#define  B_ST	\
   { \
    if (sts > ERROR) \
       break; \
   }
/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

typedef	int	boolean;

#endif /* ERROR_H */
/*------------------------ ENDE ---------------------------------*/
