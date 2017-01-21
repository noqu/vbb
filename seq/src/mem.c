/*---------------------------------------------------------------*/
/* mem.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Funktionen zur Verwaltung einer Freelist von Leerknoten, 
 * sowie einer Sicherheitsreserve fuer den Betrieb bei 
 * Speicherknappheit. (dann Umschaltung des Heaps auf LIFO).
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

/* #define		TRACE_ON */
#define		TRACE_CONDITION		glob.flags & TRACE_MEM

#define  	ANZ_BLOECKE	128

#include  <stdio.h>
#include  <stdlib.h>
#include  "types.h"
#include  "u_types.h"
#include  "u_funcs.h"
#include  "error.h"
#include  "mem.h"
#include  "trace.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

static	node_t		*freelist;
static	node_t		*reserve;
extern	stat_t		statistic;
extern  glob_t		glob;

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

public	int	m_get_node ();
public	void	m_free_node ();
static	int	m_init_mem ();
static	int	m_init_reserve ();

#else

public	int	m_get_node (
			u_glob_t*	/* u_glob */,
			node_t**	/* node */);

public	void	m_free_node (
			node_t*		/* node */);

static	int	m_init_mem (
			u_glob_t*	/* u_glob */);

static	int	m_init_reserve (
			u_glob_t*	/* u_glob */);

#endif

/*^*/
/*------------------------ M_GET_NODE ---------------------------*/
/* Liefert einen neuen Knoten aus der Freelist. Falls diese leer 
 * ist, wird m_init_mem aufgerufen. Beim ersten Aufruf wird 
 * grundsaetzlich m_init_mem aufgerufen, so dass keine vorherige 
 * Initialisierung noetig ist. 
 */

public	int	m_get_node (u_glob, node)

u_glob_t	*u_glob;
node_t		**node;

   {
    int		sts;

    /* Wenn freelist leer, nachallokieren: */
    if (!freelist)
	{
	 /* if (statistic.allocated>200 && *((int *)get_param(3))==1) */
	    /* { */
	     /* return (M_INIT_MEM); */
	    /* } */
	 sts = m_init_mem (u_glob);
	 R_ST
	}
    /* Spitze der freelist abschneiden und ausliefern: */
    *node = freelist;
    freelist = freelist->leftson;
    (*node)->leftson = (node_t *) 0;
    (*node)->rightson = (node_t *) 0;
    (*node)->father = (node_t *) 0;
    return (OK);
   }

/*^*/
/*------------------------ M_FREE_NODE --------------------------*/
/* Haengt den Knoten node vorne in die Freelist ein.
 */

public	void	m_free_node (node)

node_t		*node;

   {
    /* Node in freelist einhaengen: */
    node->leftson = freelist;
    freelist = node;
   }

/*^*/
/*------------------------ M_INIT_MEM ---------------------------*/
/* Fuellt die freelist mit ANZ_BLOECKE Bloecken neu auf.
 */

static int	m_init_mem (u_glob)

u_glob_t	*u_glob;

   {
    int		i;
    int		sts = OK;
    static int	reserve_used;
    u_node_t	*u_list[ANZ_BLOECKE];

    /* Im ersten Durchgang Reserve beiseitelegen: */
    if (!reserve && !reserve_used)
	sts = m_init_reserve (u_glob);
    R_ST

    /* Allokiere eine Menge von Bloecken: */
    freelist = (node_t *) malloc (sizeof (node_t) * ANZ_BLOECKE);
    if (!freelist)
	sts = M_INIT_MEM;

    /* Allokiere eine Menge von u_node-Bloecken: */
    i = u_create_node (u_glob, ANZ_BLOECKE, u_list);
    if (i != ANZ_BLOECKE)
	sts = M_INIT_MEM;

    /* Wenn kein Speicher mehr da: */
    if (sts == M_INIT_MEM)
       {
	/* Wenn noch Reserve vorhanden: */
	if (!reserve_used && reserve)
	   {
	    /* Verwende Reserve: */
	    freelist = reserve;
	    reserve_used = OK_TRUE;
	    /* Teile Heap mit, dass es nicht mehr gibt: */
	    glob.max_heap_size = statistic.allocated;
	    statistic.allocated += glob.reserve_size;
	    reserve = 0;
	    return (OK);
	   }
	/* Sonst (keine Reserve mehr): */
	else
	    return (M_INIT_MEM);
       }
    /* Sonst (Speicher wurde hier erfolgreich allokiert) */
    else
       {
	/* Baue daraus eine freelist auf: */
	for (i = 0; i < ANZ_BLOECKE; i++)
	   {
	    freelist[i].leftson = freelist + i + 1;
	    freelist[i].u_node = u_list[i];
	   }
	freelist[ANZ_BLOECKE - 1].leftson = (node_t *) 0;
	statistic.allocated += ANZ_BLOECKE;
	return (OK);
       }
   }

/*------------------------ M_INIT_RESERVE -----------------------*/
/* Fuellt die Reserveliste mit glob.reserve_size Bloecken.
 */

static int	m_init_reserve (u_glob)

u_glob_t	*u_glob;

   {
    int		i;
    u_node_t	**u_list;

    /* Platz fuer u_Liste allokieren: */
    u_list = (u_node_t **) malloc (glob.reserve_size 
    				   * sizeof (u_node_t *));
    if (!u_list)
	return (M_INIT_MEM);

    /* Allokiere eine Menge von Bloecken: */
    reserve = (node_t *) malloc (sizeof (node_t) * glob.reserve_size);
    if (!reserve)
	return (M_INIT_MEM);

    /* Allokiere eine Menge von u_node-Bloecken: */
    i = u_create_node (u_glob, glob.reserve_size, u_list);
    if (i != glob.reserve_size)
	return (M_INIT_MEM);

    /* Baue daraus eine Reserve-Liste auf: */
    for (i = 0; i < glob.reserve_size; i++)
       {
	reserve[i].leftson = reserve + i + 1;
	reserve[i].u_node = u_list[i];
       }
    reserve[glob.reserve_size - 1].leftson = (node_t *) 0;
    
    free (u_list);

    return (OK);
   }

/*------------------------ ENDE ---------------------------------*/
