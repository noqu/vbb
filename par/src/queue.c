/*---------------------------------------------------------------*/
/* queue.c		                        		 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Gepufferte unidirektionale Kommunikation ueber Queues. Nur
 * innerhalb eines Transputers moeglich. Es handelt sich um
 * Ein-Leser-Ein-Schreiber-Queues, die keine Synchronisation zum
 * Schutz vor konkurrierendem Zugriff benoetigen.
 * Die Queue darf nur von genau einem Sender und einem Empfaenger 
 * betrieben werden. Es findet keinerlei Pruefung oder Sicherung 
 * gegen mehrfachen Zugriff statt. 
 * Jedes Queue-Element besteht aus einem beliebigen Pointer (data) 
 * und einem Typ (type). Es ist Aufgabe von Sender und Empfaenger, 
 * sich ueber die Art der Pointer zu einigen. 
 * q_write () und q_read () realisieren eine nichtblockierende
 * Queue und geben bei voller bzw. leerer Queue OK_FALSE, bei 
 * erfolgreichem Sende- bzw. Empfangsvorgang OK_TRUE zurueck.
 * q_write_bl () und q_read_bl () realisieren darauf aufbauend
 * ein blockierendes Protokoll mit Hilfe von ProcReschedule ().
 * Den blockierenden Funktionen wird ein Flag mitgegeben, ueber
 * das eine Abbruchbedingung realisiert werden kann.
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#include  <stdio.h>
#include  <stdlib.h>
#include  <process.h>
#include  "queue.h"
#include  "error.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

public boolean	q_read (
			queue_t*	/* queue */, 
			int*		/* ptype */, 
			void**		/* pdata */);

public boolean	q_read_bl (
			queue_t*	/* queue */, 
			int*		/* ptype */, 
			void**		/* pdata */, 
#ifdef VBB_ORIGINAL
			int*		/* dead */);
#else
			volatile int*	/* dead */);
#endif

public boolean	q_write (
			queue_t*	/* queue */, 
			int		/* type */, 
			void*		/* data */);

public boolean	q_write_bl (
			queue_t*	/* queue */, 
			int		/* type */, 
			void*		/* data */, 
#ifdef VBB_ORIGINAL
			int*		/* dead */);
#else
			volatile int*	/* dead */);
#endif

public int	q_length (
			queue_t*	/* queue */);

public int	q_make_queue (
			int		/* size */, 
			queue_t**	/* queue */);

public int	q_size (
			queue_t*	/* queue */);

/*------------------------ Q_MAKE_QUEUE -------------------------*/
/* Legt eine Queue der Groesse 'size' an. (Eine solche Queue kann
 * size - 1 Elemente aufnehmen (!)
 */

int	q_make_queue (size, queue)

int		size;
queue_t		**queue;

   {
    queue_t	*q;

    q = (queue_t *) malloc (sizeof (queue_t));
    if (!q)
	return (Q_MAKE_QUEUE);

    q->buf = (q_item_t *) malloc (sizeof (q_item_t) * size);
    if (!q->buf)
       {
	free (q);
	return (Q_MAKE_QUEUE);
       }

    q->size = size;
    q->read = 0;
    q->write = 0;
    *queue = q;
    return (OK);
   }

/*------------------------ Q_WRITE ------------------------------*/
/* Schreibt ein Element vom Typ 'type' in die Queue. Wenn die 
 * Queue voll ist, liefert die Funktion OK_FALSE zurueck, 
 * anderenfalls OK_TRUE. Der Aufrufer wird in keinem Fall blockiert.
 */

boolean	q_write (queue, type, data)

queue_t		*queue;
int		type;
void		*data;

   {
    if ((queue->write + 1) % queue->size == queue->read)
	return (OK_FALSE);

    queue->buf[queue->write].type = type;
    queue->buf[queue->write].data = data;
    queue->write = (queue->write + 1) % queue->size;
    return (OK_TRUE);
   }

/*------------------------ Q_READ -------------------------------*/
/* Liest ein Element aus der Queue. Ist die Queue leer, liefert die
 * Funktion OK_FALSE, ansonsten OK_TRUE. Der Aufrufer wird in keinem
 * Fall blockiert. ptype zeigt auf den Typ der gelesenen Nachricht,
 * pdfata auf die Nachricht selbst. (Beide Werte sind bei leerer
 * Queue nicht definiert.
 */

boolean	q_read (queue, ptype, pdata)

queue_t		*queue;
int		*ptype;
void		**pdata;


   {
    if (queue->write == queue->read)
	return (OK_FALSE);

    *ptype = queue->buf[queue->read].type;
    *pdata = queue->buf[queue->read].data;
    queue->read = (queue->read + 1) % queue->size;
    return (OK_TRUE);
   }

/*------------------------ Q_LENGTH -----------------------------*/
/* Liefert die momentane Laenge einer Queue.
 */

int	q_length (queue)

queue_t		*queue;

   {
    return ((queue->size + queue->write - queue->read) % queue->size);
   }

/*------------------------ Q_SIZE -------------------------------*/
/* Liefert die Groesse einer Queue, d.h. die maximale Zahl von
 * Elementen, die sie Queue aufnehmen kann ( + 1 )
 */

int	q_size (queue)

queue_t		*queue;

   {
    return (queue->size);
   }

/*------------------------ Q_WRITE_BL ---------------------------*/
/* Schreibt blockierend in eine Queue. Der aufrufende Prozess wird
 * blockiert, bis der Schreibvorgang erfolgreich war oder das Flag, auf
 * das 'dead' zeigt, ungleich 0 wird. Durch das Flag kann ein endloses
 * Blockieren bei Prozessende auf der lesenden Seite vermieden werden.
 * Der Rueckgabewert ist Q_PARTNER_DEAD, wenn das Flag zum Abbruch
 * gefuehrt hat, sonst der Rueckgabewert von q_write().
 * Busy-Waiting wird durch ProcReschedule() vermieden.
 */

boolean	q_write_bl (queue, type, data, dead)

queue_t		*queue;
int		type;
void		*data;
#ifdef VBB_ORIGINAL
int		*dead;
#else
volatile int	*dead;
#endif

   {
    int		sts;

    while ((sts = q_write (queue, type, data)) == OK_FALSE)
       {
	ProcReschedule ();
	if (*dead)
	    return (Q_PARTNER_DEAD);
       }
    return (sts);
   }

/*------------------------ Q_READ_BL ----------------------------*/
/* Liest blockierend aus einer Queue. Der aufrufende Prozess wird
 * blockiert, bis der Lesevorgang erfolgreich war oder das Flag, auf
 * das 'dead' zeigt, ungleich 0 wird. Durch das Flag kann ein endloses
 * Blockieren bei Prozessende auf der schreibenden Seite vermieden 
 * werden.
 * Der Rueckgabewert ist Q_PARTNER_DEAD, wenn das Flag zum Abbruch
 * gefuehrt hat, sonst der Rueckgabewert von q_write().
 * Busy-Waiting wird durch ProcReschedule() vermieden.
 */

boolean	q_read_bl (queue, ptype, pdata, dead)

queue_t		*queue;
int		*ptype;
void		**pdata;
#ifdef VBB_ORIGINAL
int		*dead;
#else
volatile int	*dead;
#endif
 
   {
    int		sts;

    while ((sts = q_read (queue, ptype, pdata)) == OK_FALSE)
       {
	ProcReschedule ();
	if (*dead)
	    return (Q_PARTNER_DEAD);
       }
    return (sts);
   }

/*------------------------ ENDE ---------------------------------*/
