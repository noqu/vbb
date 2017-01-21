/*---------------------------------------------------------------*/
/* heap.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Verwaltung des Knoten-Heaps, auf dem sich die aktiven BB-Knoten
 * befinden. Version mit Binaerbaum und LIFO-Stack.
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#include  <stdio.h>
#include  <stdlib.h>
#include  "types.h"
#include  "error.h"
#include  "heap.h"
#include  "mem.h"

#define  NIL	(node_t *) 0

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

static	node_t		heap;
static	node_t		lifo;
static	int		heap_size;
static	int		heap_full;
static	node_t		*best_of_heap;
static	node_t		*incumbent;
extern	stat_t		statistic;
extern	int		m_finished;
extern	glob_t		glob;

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

public node_t	*h_read_incumbent ();
public boolean	h_get_from_heap ();
public int	h_new_incumbent ();
public boolean	h_init_incumbent ();
public void	h_init_heap ();
public void	h_put_on_heap ();
public int	h_get_best_prio ();
public int	h_heap_size ();

#else

public node_t	*h_read_incumbent (void);

public boolean	h_get_from_heap (
			node_t** 	/* node */);

public int	h_new_incumbent (
			node_t*	/* new */);

public boolean	h_init_incumbent (
			node_t*		/* new */);

public void	h_init_heap (void);

public void	h_put_on_heap (
			node_t*		/* node */);

public int	h_get_best_prio (
			int*		/* prio*/);

public int	h_heap_size (void);

#endif

/*^*/
/*------------------------ H_READ_INCUMBENT ---------------------*/
/* Liest die incumbent aus der modulglobalen Variable und liefert 
 * sie zurueck.
 */

node_t	*h_read_incumbent ()

   {
    return (incumbent);
   }

/*^*/
/*------------------------ H_GET_FROM_HEAP ----------------------*/
/* Holt den Knoten mit der groessten Prioritaet vom Heap, d.h. in 
 * diesem Falle aus der verketteten Liste. Rueckgabewert ist
 * OK_FALSE, wenn Heap leer, sonst OK_TRUE.
 * (Wird nur vom Dispatcher verwendet, nicht von anderen Prozessen)
 */

public	boolean	h_get_from_heap (node)

node_t		**node;

   {
    /* Wenn noch Elemente auf Lifo-Stack: */
    if (lifo.leftson)
       {
	/* Erstes Element der Lifo-Liste nehmen: */
	*node = lifo.leftson;
	lifo.leftson = lifo.leftson->leftson;
	statistic.from_lifo++;
       }
    /* Sonst von Heap nehmen: */
    else if (best_of_heap != &heap)
       {
	*node = best_of_heap;
	statistic.from_heap++;
	/* Best_of_heap muss rechter Sohn seines Vaters sein */
	/* Best_of_heap kann keinen rechten Sohn haben */
	/* Vater bekommt seinen rechts/links-Enkel als rechten Sohn */
	best_of_heap->father->rightson = best_of_heap->leftson;
	/* Wenn es diesen Enkel gibt: */
	if (best_of_heap->leftson)
	   {
	    /* Vater des Enkels aktualisieren: */
	    best_of_heap->leftson->father = best_of_heap->father;
	   }
       /* Neues best_of_heap suchen: (Beginnend beim Vater) */
       /* (Nur rechter Ast wird durchsucht) */
       best_of_heap = best_of_heap->father;
       while (best_of_heap->rightson)
	   best_of_heap = best_of_heap->rightson;
       }
    /* Sonst ist gar nix mehr da: */
    else
       return (OK_FALSE);
    heap_size--;
    return (OK_TRUE);
   }

/*^*/
/*------------------------ H_INIT_HEAP --------------------------*/
/* (Wird nur vom Dispatcher verwendet, nicht von anderen Prozessen)
 */
 
public	void	h_init_heap ()

   {
    /* Verkettete Liste von Nodes initialisieren (leer) */
    heap.leftson = NIL;
    heap.rightson = NIL;
    heap.father = NIL;
    heap.priority = - MAXINT;
    best_of_heap = &heap;
    lifo.leftson = NIL;
    heap_size = 0;
   }

/*------------------------ DEL_NODE ----------------------------*/
/* Loescht einen Knoten aus dem Binaerbaum. Der symmetrische
 * Vorgaenger symvor wird evtl. zur Vertauschung verwendet. Interne
 * Funktione des Moduls 'heap'
 */

node_t	*del_node (node, symvor)

node_t	*node;
node_t	*symvor;
   
   {
    node_t	*only_son;
    node_t	*del;
    u_node_t	*u;

    /* Wenn node ein innerer Knoten mit 2 Soehnen ist: */
    if (node->leftson && node->rightson)
       {
        /* Vertausche Inhalte v. node und symmetrischen Vorgaenger: */
	/* (Priority und Bound von node sind egal) */
	u = node->u_node;
	node->u_node = symvor->u_node;
	symvor->u_node = u;
	node->priority = symvor->priority;
	node->l_bound = symvor->l_bound;
	node->value = symvor->value;
	/* Jetzt soll der symmetrische Vorgaenger geloescht werden: */
	del = symvor;
       }
    else
       {
	/* node kann direkt geloescht werden: */
	del = node;
       }

    /* Wenn del keinen linken Sohn hat: */
    if (!del->leftson)
	only_son = del->rightson;
    else if (!del->rightson)
	only_son = del->leftson;

    /* Wenn del linker Sohn seines Vaters ist: */
    if (del == del->father->leftson)
	del->father->leftson = only_son;
    /* Sonst (del ist rechter Sohn seines  Vaters) */
    else
	del->father->rightson = only_son;

    /* Aktualisiere Vater von only_son */
    if (only_son)
       {
	/* only_sons Grossvater ist jetzt sein Vater geworden: */
	only_son->father = del->father;
       }

    return (del);
   }

/*------------------------ SYMNACH -----------------------------*/
/* Bestimmt den symmetrischen Nachfolger eines Knotens im Binaerbaum.
 * Interne Funktion dieses Moduls.
 */

node_t	*symnach (node)

node_t	*node;

   {
    /* Wenn node einen rechten Sohn hat: */
    if (node->rightson)
       {
	/* Gehe einen nach rechts... */
	node = node->rightson;
	/* ... und dann so weit wie moeglich nach links: */
	while (node->leftson)
	    node = node->leftson;
	return (node);
       }
    /* Sonst wenn node keinen Vater hat: */
    else if (!node->father)
	return (NIL);
    /* Sonst wenn node linker Sohn seines Vaters ist: */
    else if (node == node->father->leftson)
	return (node->father);
    /* Sonst (node ist rechter Sohn seines Vaters) */
    else
       {
	/* Solange es links aufwaerts geht: */
	while (node->father && node == node->father->rightson)
	    node = node->father;
	/* Gehe noch eine Stufe (nach rechts) hoch: */
	/* (Bei der Wurzel ist dies NIL) */
	node = node->father;
	return (node);
       }
   }

/*^*/
/*------------------------ H_NEW_INCUMBENT ----------------------*/
/* Traegt eine neue Incumbent (beste Loesung) in die modulglobale
 * Variable ein. Durchsucht den Heap nach Knoten, die aufgrund
 * der neuen Incumbent eliminiert werden koennen und eliminiert 
 * diese. 
 * (Wird nur vom Dispatcher verwendet, nicht von anderen Prozessen)
 */

public	int	h_new_incumbent (new)

node_t		*new;

   {
    node_t	*akt;
    node_t	*elim;
    node_t	*symvor;

    if (new->value >= incumbent->value)
	return (OK_FALSE);

    /* Neue incumbent eintragen */
    incumbent = new;

    /* Spule an den Anfang des Heaps (ganz links): */
    akt = heap.rightson; 
    while (akt && akt->leftson)
	akt = akt->leftson;
    symvor = &heap;

    /* Durchlaufe heap in symmetrischer Reihenfolge (inorder): */
    while (akt)
       {
	/* Wenn Knoten eliminiert werden soll: */
	if (akt->l_bound >= incumbent->value)
	   {
	    elim = del_node (akt, symvor);
	    m_free_node (elim);
	    statistic.elim_from_heap++;
	    heap_size--;
	    /* Wenn Knoten direkt geloescht wurde: */
	    if (elim == akt)
		akt = symnach (symvor);
	    /* Sonst (Knoten wurde mit symm. Vorg. vertauscht) */
	    else
	       {
		symvor = akt;
		akt = symnach (akt);
	       }
	   }
	else
	   {
	    symvor = akt;
	    akt = symnach (akt);
	   }
       }

    /* Best of Heap ist der als letzter betrachtete Knoten: */
    best_of_heap = symvor;

    /* Lifo ebenfalls durchsuchen: */
    akt = &lifo; 
    while (akt->leftson)
       {
	if (akt->leftson->l_bound >= incumbent->value)
	   {
	    elim = akt->leftson;
	    akt->leftson = akt->leftson->leftson;
	    m_free_node (elim);
	    statistic.elim_from_heap++;
	    heap_size--;
	   }
	else
	    akt = akt->leftson;
       }

    return (OK_TRUE);
   }

/*^*/
/*------------------------ H_INIT_INCUMBENT ---------------------*/
/* Wenn es noch keine gueltige Incumbent gibt, traegt diese Funktion
 * 'new' als solche ein und liefert OK_TRUE. Sonst wird OK_FALSE
 * zurueckgegeben.
 */

public boolean	h_init_incumbent (new)

node_t		*new;

   {
    if (incumbent)
	return (OK_FALSE);

    incumbent = new;
    return (OK_TRUE);
   }

/*^*/
/*------------------------ H_PUT_ON_HEAP ------------------------*/
/* Ordnet den Knoten 'node' in die lineare Liste an der richtigen 
 * Stelle ein. (Fallende Sortierung nach Prioritaeten). 
 * (Wird nur vom Dispatcher verwendet, nicht von anderen Prozessen)
 */

public	void	h_put_on_heap (node)

node_t		*node;

   {
    node_t	*akt;

    if (heap_size > glob.max_heap_size)
	heap_full = OK_TRUE;
    else
	heap_full = OK_FALSE;

    /* Wenn Heap voll: */
    if (heap_full)
       {
	/* Lege Knoten auf Lifo-Liste: */
	akt = &lifo;
	/* Vorspulen bis zum ersten Knoten mit kleinerer Prio: */
	/* (Nur unter Knoten mit gleichem Vater => Brueder sortiert) */
	while (akt->leftson 
	 && akt->leftson->priority > node->priority
	 && akt->father == node->father)
	    akt = akt->leftson;
	node->leftson = akt->leftson;
	akt->leftson = node;
       }
    else
       {
	akt = &heap;
	while (akt)
	   {
	    /* Wenn kleinere Prioritaet: linker Ast */
	    if (node->priority < akt->priority)
	       {
		/* Wenn linker Ast existiert: */
		if (akt->leftson)
		   {
		    /* Dort weitermachen: */
		    akt = akt->leftson;
		   }
		else
		   {
		    /* Knoten dort einfuegen und fertig: */
		    akt->leftson = node;
		    break;
		   }
	       }
	    /* Sonst: rechter Ast */
	    else
	       {
		/* Wenn rechter Ast existiert: */
		if (akt->rightson)
		   {
		    /* Dort weitermachen: */
		    akt = akt->rightson;
		   }
		else
		   {
		    /* Knoten dort einfuegen und fertig: */
		    akt->rightson = node;
		    break;
		   }
	       }
	    statistic.heap_cycles++;
	   } /* end while */

	node->father = akt;
	node->leftson = NIL;
	node->rightson = NIL;
	/* Neuer Knoten hat Vorrang vor altem gleichwertigen */
	/* (damit best_of_heap keine rechten Soehne hat) */
	if (node->priority >= best_of_heap->priority)
	    best_of_heap = node;
       }
    heap_size++;
   }

/*------------------------ H_GET_BEST_PRIO ----------------------*/
/* Liefert die Prioritaet des besten Knotens vom Heap in prio.
 * Rueckgabewert ist OK_FALSE, wenn Heap leer, sonst OK_TRUE.
 * (Wird nur vom Dispatcher verwendet, nicht von anderen Prozessen)
 */

public int	h_get_best_prio (prio)

int	*prio;

   {
    /* Wenn Heap leer: */
    if (!heap.rightson)
       return (OK_FALSE);
    *prio = best_of_heap->priority;
    return (OK_TRUE);
   }

/*------------------------ H_HEAP_SIZE --------------------------*/
/* Liefert die Anzahl Knoten auf dem Heap.
 */

public int	h_heap_size ()

   {
    return (heap_size);
   }

/*------------------------ ENDE ---------------------------------*/
