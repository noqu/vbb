/*---------------------------------------------------------------*/
/* startup.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Konstanten und Defaultwerten fuer startup.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  STARTUP_H
#define  STARTUP_H

#include  "error.h"

#define	 NR_OF_PROCESSES	12
#define  STACK_SIZE		8192
#define	 DEF_PROF_START		100
#define	 DEF_PROF_UNIT		2
#define	 DEF_PROF_SCALE		10
#define	 DEF_PRIO_PAR		0
#define	 DEF_DIST_LOW		8
#define	 DEF_DIST_HIGH		4
#define	 DEF_DIST_IDLE		1
#define	 DEF_MAX_HEAP_SIZE	8000000
#define	 DEF_MIN_HEAP_SIZE	3
#define	 DEF_RESERVE_SIZE	2000
#define  MEM_OUT_Q_SIZE		40
#define  MEM_IN_Q_SIZE		40
#define  MEM_MIN_LEVEL		4
#define  MEM_MAX_LEVEL		36
#define  EXP_TO_DISP_Q_SIZE	10
#define  DISP_TO_EXP_Q_SIZE	3
#define  DISP_TO_OUT_Q_SIZE	5
#define  IN_TO_DISP_Q_SIZE	5
#define  TRACER_Q_SIZE		20

#endif /* STARTUP_H */
/*------------------------ ENDE ---------------------------------*/
