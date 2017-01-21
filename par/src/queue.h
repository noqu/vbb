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
    int		type;
    void	*data;
   }
q_item_t;

typedef struct queue_st
   {
    int		read;
    int		write;
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
			int*		/* dead */);

extern boolean	q_write (
			queue_t*	/* queue */, 
			int		/* type */, 
			void*		/* data */);

extern boolean	q_write_bl (
			queue_t*	/* queue */, 
			int		/* type */, 
			void*		/* data */, 
			int*		/* dead */);

extern int	q_length (
			queue_t*	/* queue */);

extern int	q_make_queue (
			int		/* size */, 
			queue_t**	/* queue */);

extern int	q_size (
			queue_t*	/* queue */);

#endif /* QUEUE_H */
/*------------------------ ENDE ---------------------------------*/
