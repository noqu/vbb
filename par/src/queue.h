/*---------------------------------------------------------------*/
/* queue.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Konstanten, Daten- und Funktionstypen fuer
 * queue.c
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  QUEUE_H
#define  QUEUE_H

#include  "error.h"

#define Q_NODE		-1
#define Q_INC		-2
#define Q_EMPTY_NODE	-3
#define Q_INIT_INC	-4
#define Q_TOKEN		-5
#define Q_FINISH  	-6
#define Q_IDLE  	-7
#define Q_ERROR  	-8

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

typedef struct q_item_st
   {
#ifdef VBB_ORIGINAL
    int		type;
    void	*data;
#else
    volatile int	type;
    volatile void	*data;
#endif
   }
q_item_t;

typedef struct queue_st
   {
#ifdef VBB_ORIGINAL
    int		read;
    int		write;
#else
    volatile int	read;
    volatile int	write;
#endif
    q_item_t	*buf;
    int		size;
   }
queue_t;

/*------------------------ FUNKTIONEN ---------------------------*/

extern boolean	q_read (
			queue_t*	/* queue */, 
			int*		/* ptype */, 
			void**		/* pdata */);

extern boolean	q_read_bl (
			queue_t*	/* queue */, 
			int*		/* ptype */, 
			void**		/* pdata */, 
#ifdef VBB_ORIGINAL
			int*		/* dead */);
#else
			volatile int*	/* dead */);
#endif

extern boolean	q_write (
			queue_t*	/* queue */, 
			int		/* type */, 
			void*		/* data */);

extern boolean	q_write_bl (
			queue_t*	/* queue */, 
			int		/* type */, 
			void*		/* data */, 
#ifdef VBB_ORIGINAL
			int*		/* dead */);
#else
			volatile int*	/* dead */);
#endif

extern int	q_length (
			queue_t*	/* queue */);

extern int	q_make_queue (
			int		/* size */, 
			queue_t**	/* queue */);

extern int	q_size (
			queue_t*	/* queue */);

#endif /* QUEUE_H */
/*------------------------ ENDE ---------------------------------*/
