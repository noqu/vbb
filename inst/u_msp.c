/*---------------------------------------------------------------*/
/* u_msp.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.1		28.4.93					 */
/*---------------------------------------------------------------*/
/* Benutzerfunktionen fuer das Machine Scheduling Problem
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  "u_msp_types.h"
#include  "u_funcs.h"
#include  "read.h"
#include  "error.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/
/*------------------------ MAKROS -------------------------------*/

#define  MAX(A, B)	((A > B) ? A : B)
#define  MIN(A, B)	((A < B) ? A : B)

/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

public boolean	u_first_child ();
public boolean	u_is_feasible ();
public boolean	u_next_child ();
public int	u_heur_loes ();
public int	u_init_glob ();
public int	u_lower_bound ();
public int	u_mk_start_node ();
public int	u_print_sol ();
public int	u_priority ();
public int	u_solution ();
public int	u_create_node ();
public int	u_sizeof_flat_node ();
public void	u_copy_node ();
public void	u_flat_node ();
public void	u_unflat_node ();
public int	u_flat_glob ();
public int	u_unflat_glob ();
public char	*u_trace_node ();

#else

public boolean	u_first_child (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* parent */, 
			u_node_t*	/* son */, 
			int*		/* selection */);

public boolean	u_is_feasible (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */);

public boolean	u_next_child (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* parent */, 
			u_node_t*	/* son */, 
			int*		/* selection */);

public int	u_heur_loes (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */, 
		        u_node_t*	/* solution */, 
		        u_zf_t*		/* value */);

public int	u_init_glob (
			u_glob_t**	/* u_glob */);

public int	u_lower_bound (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */, 
			u_zf_t*		/* l_bound */);

public int	u_mk_start_node (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* startnode */);

public int	u_print_sol (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* sol */);

public int	u_priority (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */, 
			int		/* para */,
			int*		/* priority */);

public int	u_solution (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */, 
			u_zf_t*		/* value */);


public int	u_create_node (
			u_glob_t*	/* u_glob */,
			int		/* anzahl */, 
			u_node_t**	/* nodes */);

public int	u_sizeof_flat_node (
			u_glob_t*	/* u_glob */);

public void	u_copy_node (
			u_glob_t*	/* u_glob */,
			u_node_t*	/* from */, 
			u_node_t*	/* to */); 

public void	u_flat_node (
			u_glob_t*	/* u_glob */,
			u_node_t*	/* node */, 
			char*		/* flat */);

public void	u_unflat_node (
			u_glob_t*	/* u_glob */,
			char*		/* flat */, 
			u_node_t*	/* node */);

public int	u_flat_glob (
			u_glob_t*	/* u_glob */,
			char**		/* flat */,
			int*		/* size */);

public int	u_unflat_glob (
			char*		/* flat */,
			u_glob_t**	/* u_glob */);

public char	*u_trace_node (
			u_node_t*	/* u_node */,
			char*		/* wp */);

#endif

/*^*/
/*------------------------ U_MK_STARTNODE -----------------------*/
/* Erzeugt den Knoten fuer das Ausgangsproblem als Wurzel des 
 * BB-Baums. Der Speicherplatz fuer 'startnode' wird von der 
 * rufenden Funktion bereitgestellt. Der Rueckgabewert ist OK oder 
 * ein problemspezifischer Statuswert. 
 */

int	u_mk_start_node (u_glob, startnode)

u_glob_t	*u_glob;
u_node_t	*startnode;

   {
    int		i;

    /* Fixedlist ist leer: */
    startnode->fixed = -1;

    /* Freelist enthaelt alle anderen: */
    startnode->free = 0;
    for (i = 0; i < u_glob->size - 1; i++)
       {
	startnode->list[i] = i + 1;
       } /* end for */
    startnode->list[u_glob->size - 1] = -1;

    /* fw auf Null setzen */
    for (i = 0; i < 3; i++)
	startnode->fw[i] = 0;
    
    /* Summen und Minimum initialisieren: */
    startnode->sum_d1 = 0;
    startnode->sum_d2 = 0;
    startnode->min_d2 = 0;
    startnode->min_d1d2 = 0;
    for (i = 0; i < u_glob->size; i++)
       {
	startnode->sum_d1 += u_glob->d[1][i];
	startnode->sum_d2 += u_glob->d[2][i];
	if (u_glob->d1d2[i] < u_glob->d1d2[startnode->min_d1d2])
	    startnode->min_d1d2 = i;
	if (u_glob->d[2][i] < u_glob->d[2][startnode->min_d2])
	    startnode->min_d2 = i;
       } /* end for */

    startnode->l_bound = 0;
    startnode->depth = 0;
    return (OK);
   }

/*^*/
/*------------------------ U_FIRST_CHILD ------------------------*/
/* Erzeugt den ersten Sohn eines Knotens 'parent'. Der Sohn wird als 
 * neuer BB-Knoten in 'son' zurueckgegeben. Der Speicherplatz fuer 
 * den Sohn wird von der rufenden Funktion bereitgestellt. In 
 * 'selection' wird ein Indexwert zurueckgegeben, der den erzeugten 
 * Sohn von seinen Bruedern eindeutig unterscheidet. Dieser 
 * Indexwert wird von u_next_child () zur Erzeugung des naechsten 
 * Sohns verwendet. Wenn keine Soehne mehr erzeugt werden koennen, 
 * wird OK_FALSE zurueckgegeben, sonst OK_TRUE. 
 */

boolean	u_first_child (u_glob, parent, son, selection)

u_glob_t	*u_glob;
u_node_t	*parent;
u_node_t	*son;
int		*selection;

   {
    /* Wenn keine Soehne mehr zu erzeugen: */
    if (parent->free == -1)
	return (OK_FALSE);
    /* Alles kopieren aus Vater: */
    u_copy_node (u_glob, parent, son);
    /* Ersten freien als selection merken: */
    *selection = parent->free;
    /* Aus freelist aushaengen: */
    son->free = son->list[son->free];
    /* An fixed_list (vorne) dranhaengen: */
    son->list[*selection] = son->fixed;
    son->fixed = *selection;
    /* Zeiten zu fw hinzurechnen: */
    son->fw[0] += u_glob->d[0][*selection];
    son->fw[1] = MAX (parent->fw[1], son->fw[0]) 
    	       + u_glob->d[1][*selection];
    son->fw[2] = MAX (parent->fw[2], son->fw[1]) 
    	       + u_glob->d[2][*selection];
    son->l_bound = 0;
    son->depth++;
    return (OK_TRUE);
   }

/*^*/
/*------------------------ U_NEXT_CHILD -------------------------*/
/* Erzeugt aus dem Knoten 'parent' den "naechsten" Sohn. Die 
 * Funktion erhaelt in 'selection' den Indexwert des letzten 
 * erzeugten Sohnes, um den darauf folgenden bestimmen zu koennen. 
 * Der Sohn wird als neuer BB-Knoten in 'son' zurueckgegeben. Der 
 * Speicherplatz fuer den Sohn wird von der rufenden Funktion 
 * bereitgestellt. In 'selection' wird der Indexwert des erzeugten 
 * Sohnes zurueckgegeben. (Zur Verwendung beim naechsten Aufruf) 
 * Wenn keine Soehne mehr zu erzeugen sind, wird OK_FALSE 
 * zurueckgegeben, ansonsten OK_TRUE. 
 */

boolean	u_next_child (u_glob, parent, son, selection)

u_glob_t	*u_glob;
u_node_t	*parent;
int		*selection;
u_node_t	*son;

   {
    int		last;

    /* Wenn keine Soehne mehr zu erzeugen: */
    if (parent->list[*selection] == -1)
	return (OK_FALSE);
    /* Alles kopieren aus Vater: */
    u_copy_node (u_glob, parent, son);
    last = *selection;
    /* Freelist-Nachfolger von alter selection wird neue selection: */
    *selection = parent->list[*selection];
    /* Aus freelist aushaengen: */
    son->list[last] = son->list[*selection];
    /* An fixed_list (vorne) dranhaengen: */
    son->list[*selection] = son->fixed;
    son->fixed = *selection;
    /* Zeiten zu fw hinzurechnen: */
    son->fw[0] += u_glob->d[0][*selection];
    son->fw[1] = MAX (parent->fw[1], son->fw[0]) 
    	       + u_glob->d[1][*selection];
    son->fw[2] = MAX (parent->fw[2], son->fw[1]) 
    	       + u_glob->d[2][*selection];
    son->l_bound = 0;
    son->depth++;
    return (OK_TRUE);
   }

/*^*/
/*------------------------ U_SOLUTION ---------------------------*/
/* Vervollstaendigt einen BB-Knoten 'node', dessen Loesungsmenge 
 * genau ein Element enthaelt, zu einer Loesung. In 'node' wird 
 * dann die vollstaendige Loesung zurueckgegeben. Ausserdem wird in 
 * 'value' der Zielfunktionswert zurueckgeliefert. Enthaelt 'node' 
 * bereits eine vollstaendige Loesung, so wird 'node' unveraendert 
 * zurueckgegeben. Die Funktion liefert U_INCOMPLETE_SOL, wenn sie 
 * mit einem Knoten ohne eindeutige Loesung aufgerufen wird, sonst 
 * OK. 
 */

int	u_solution (u_glob, node, value)

u_glob_t	*u_glob;
u_node_t	*node;
u_zf_t		*value;

   {
    /* Wenn Loesung schon bekannt: */
    if (node->free == -1)
       {
	*value = node->l_bound;
	return (OK);
       }

    /* Wenn noch genau einer in free-list: */
    if (node->free != -1 && node->list[node->free] == -1)
       {
        /* Node enthaelt vollstaendige Loesung: */
	/* Freie Ecke als letzte Ecke in fixed eintragen: */
	node->list[node->free] = node->fixed;
	node->fixed = node->free;
	/* ... und aus free austragen: */
	node->free = -1;
	/* ZF-Wert berechnen : */
	node->fw[0] += u_glob->d[0][node->fixed];
	node->fw[1] = MAX (node->fw[1], node->fw[0]) 
		    + u_glob->d[1][node->fixed];
	node->fw[2] = MAX (node->fw[2], node->fw[1]) 
		    + u_glob->d[2][node->fixed];
	*value = node->fw[2];

	/* ZF-Wert in Lower Bound merken: */
	node->l_bound = *value;
	/* Tiefe erhoehen: */
	node->depth++;
	return (OK);
       }
    else
       {
	/* Unvollstaendige Loesung: */
	*value = MAXINT;
	return (U_INCOMPLETE_SOL);
       }
   }

/*-------------------------- U_HEUR_LOES ------------------------*/
/* Berechnet eine heuristische Loesung fuer den Knoten 'node'. Es 
 * handelt sich dabei um die heuristische Vervollstaendigung der in 
 * 'node' bereits enthaltenen Teilloesung. Die Loesung wird in 
 * 'solution' zurueckgegeben, der Zielfunktionswert in 'value'. Der 
 * Rueckgabewert ist OK oder ein problemspezifischer Statuswert. 
 * 
 * Diese Funktion ist optional. Wenn auf die Verwendung einer 
 * Heuristik verzichtet wird, liefert die Funktion stets MAXINT als 
 * Zielfunktionswert und laesst den Knoten 'solution' unveraendert. 
 */

int	u_heur_loes (u_glob, node, solution, value)

u_glob_t	*u_glob;
u_node_t	*node;
u_node_t	*solution;
u_zf_t		*value;

   {
    u_glob = u_glob;
    node = node;
    solution = solution;
    *value = MAXINT;
    return (OK);
   }

/*^*/
/*------------------------ U_INIT_GLOB -------------------------*/
/* Initialisiert die Datenstruktur 'u_glob', die das zu loesende 
 * Problem vollstaendig beschreibt. Diese Daten werden an alle 
 * problemspezifischen Funktionen weitergegeben und dienen zur 
 * parametrischen Beschreibung von BB-Knoten. Ausserdem wird die 
 * globale Variable 'problem_size' gesetzt, aus der alle Funktionen 
 * die Groesse des zu loesenden Problems entnehmen koennen. 
 * Die Funktion u_init_glob () kann zur Eingabe die Funktionen 
 * read_param () und matrin () aus read.c verwenden. Fehler koennen 
 * direkt auf Standardausgabe gemeldet werden. Der Rueckgabewert 
 * ist OK oder ein problemspezifischer Statuswert bei Fehlern. 
 */

int	u_init_glob (u_glob)

u_glob_t	**u_glob;

   {
    int		zeilen;
    int		spalten;
    int		**matrix;
    int		sts;
    char	*filename;
    FILE	*indat;
    int		i;

    /* Platz fuer Datenstruktur u_glob allokieren: */
    *u_glob = (u_glob_t *) malloc (sizeof (u_glob_t));
    if (!*u_glob)
	return (U_MALLOC);

    /* Dateinamen fuer Matrix aus Parameterliste holen: */
    if (!read_param ("mat", &filename))
       {
	fprintf (stderr, "u_init_glob (): Parameter 'mat' fehlt\n");
	return (U_PARAM);
       }

    /* Matrixdatei oeffnen: */
    indat = fopen (filename, "r");
    if (!indat)
       {
	fprintf (stderr, "u_init_glob (): Kann %s nicht oeffnen\n",
		 filename);
	return (U_FILE);
       }

    /* Matrix aus Datei lesen: */
    sts = matrin (indat, &matrix, &zeilen, &spalten);
    R_ST
    fclose (indat);

    if (zeilen != 3)
       {
	fprintf (stderr, "Matrix enthaelt nicht genau 3 Spalten\n",
		 filename);
	return (READ_ERR);
       }

    (*u_glob)->d = matrix;
    (*u_glob)->size = spalten;
    (*u_glob)->sum_d0 = 0;

    /* Platz fuer d1d2 allokieren: */
    (*u_glob)->d1d2 = (int *) malloc ((*u_glob)->size * sizeof (int));
    if (!(*u_glob)->d1d2)
	return (U_MALLOC);

    for (i = 0; i < (*u_glob)->size; i++)
       {
        (*u_glob)->sum_d0 += (*u_glob)->d[0][i];
	(*u_glob)->d1d2[i] = (*u_glob)->d[1][i] + (*u_glob)->d[2][i];
       }
    return (OK);
   }

/*^*/
/*------------------------ U_IS_FEASIBLE ------------------------*/
/* Stellt fest, ob der Knoten 'node' eine zulaessige Loesung des 
 * Ausgangsproblems repraesentiert. Von solchen Knoten koennen 
 * keine Soehne mehr erzeugt werden. Der Rueckgabewert ist OK_TRUE, 
 * wenn 'node' eine zulaessige Loesung enthaelt, sonst OK_FALSE. 
 */
/* In MSP wird ueberprueft, ob die Liste der freien Ecken nur noch 
 * eine Ecke enthaelt. Ist dies der Fall, so ist die Loesung 
 * eindeutig bestimmt und der Rueckgabewert ist OK_TRUE. Der Fall, 
 * dass die freelist leer ist, wird sicherheitshalber mit 
 * ueberprueft. 
 */

boolean	u_is_feasible (u_glob, node)

u_glob_t	*u_glob;
u_node_t	*node;

   {
    u_glob = u_glob;
    /* Wenn nur noch ein Element in free-list (oder gar keins): */
    if (node->depth >= u_glob->size - 1)
	return (OK_TRUE);
    else
	return (OK_FALSE);
   }

/*^*/
/*------------------------ U_LOWER_BOUND ------------------------*/
/* Liefert die untere Schranke des Zielfunktionswertes fuer den 
 * Knoten 'node'. Diese Schranke wird nach einem 
 * problemspezifischen Verfahren berechnet. (Anm: Da die Funktion 
 * u.U. mehrmals fuer denselben Knoten aufgerufen wird, ist es 
 * zweckmaessig, den Wert der unteren Schranke im Knoten abzulegen 
 * und bei erneuten Aufrufen direkt zurueckzugeben.) Es ist 
 * zulaessig, dass 'node' dabei veraendert wird. 
 */

int	u_lower_bound (u_glob, node, l_bound)

u_glob_t	*u_glob;
u_node_t	*node;
u_zf_t		*l_bound;

   {
    int		i;
    int		lf;
    int		**d;
    int		b1;
    int		b2;
    int		b3;

    /* Wenn Bound bereits berechnet: */
    if (node->l_bound)
       {
	/* Fertige Bound zurueckgeben und aus: */
	*l_bound = node->l_bound;
	return (OK);
       } /* end if */

    /* Wenn node nicht die Wurzel ist (enthaelt Eltern-Minima): */
    if (node->fixed != -1)
       {
        /* Minima und Summen aus Erbgut aktualisieren: */
	lf = node->fixed;
	d = u_glob->d;
	node->sum_d2 -= d[2][lf];
	node->sum_d1 -= d[1][lf];
	if (node->min_d1d2 == u_glob->d1d2[lf])
	   {
	    node->min_d1d2 = node->free;
	    /* Fuer alle i aus free */
	    for (i = node->free; i != -1; i = node->list[i])
	        if (u_glob->d1d2[i] < u_glob->d1d2[node->min_d1d2])
		    node->min_d1d2 = i;
	   }
	if (node->min_d2 == u_glob->d[2][lf])
	   {
	    node->min_d2 = node->free;
	    /* Fuer alle i aus free */
	    for (i = node->free; i != -1; i = node->list[i])
	        if (u_glob->d[2][i] < u_glob->d[2][node->min_d2])
		    node->min_d2 = i;
	   }
       } /* end else */

    b1 = node->fw[2] + node->sum_d2;
    b2 = node->fw[1] + node->sum_d1 + node->min_d2;
    b3 = u_glob->sum_d0 + node->min_d1d2;

    *l_bound = MAX (b1, b2);
    *l_bound = MAX (*l_bound, b3);

    node->l_bound = *l_bound;
    return (OK);
   }

/*^*/
/*---------------------------- U_PRIORITY -----------------------*/
/* Berechnet die Prioritaet des Knotens node. Hier wird die l_bound
 * des Knotens als Prioritaet verwendet.
 */

int	u_priority (u_glob, node, para, priority)

u_glob_t	*u_glob;
u_node_t	*node;
int		para;
int		*priority;

   {
    u_zf_t	l_bound;
    int		sts;
    
    sts = u_lower_bound (u_glob, node, &l_bound);
    R_ST
    /* *priority = node->depth; */
    /* *priority = l_bound * node->depth / u_glob->size - l_bound; */
    /* *priority = -l_bound; */
    if (para < 0)
	*priority = 1000 * node->depth - l_bound;
    else if (para == 0)
	*priority = -1000 * l_bound + node->depth;
    else
	*priority = (node->depth - 1) * para * l_bound
		    / (u_glob->size - 1)
		    - 100 * l_bound;
    return (OK);
   }

/*^*/
/*------------------------ U_PRINT_SOL -------------------------*/
/* Druckt die Loesung 'sol' auf der Standardausgabe aus.
 */

int	u_print_sol (u_glob, sol)

u_glob_t	*u_glob;
u_node_t	*sol;

   {
    int		a;
    int		i;

    u_glob = u_glob;

    if (sol->free != -1)
	return (U_INCOMPLETE_SOL);

    if (sol->l_bound != MAXINT)
       {
	/* Ecken rueckwaerts aus fixed lesen: */
	/* (Platz in u_glob->d1d2 benutzen, da jetzt ungenutzt) */
	i = sol->depth - 1;
	for (a = sol->fixed; a != -1; a = sol->list[a])
	    u_glob->d1d2[i--] = a;
	for (i = 0; i < sol->depth; i++)
	    printf ("%4d", u_glob->d1d2[i]);
	printf ("\nF = %d\n", sol->l_bound);
       }
    else
	printf ("Keine zulaessige Loesung\n");
    return (OK);
   }

/*^*/
/*------------------------ U_CREATE_NODE -----------------------*/
/* Erzeugt 'anzahl' u_nodes der Groesse 'size' und legt Zeiger auf 
 * sie in dem Feld 'nodes' ab. Dort muss mindestens Platz fuer 
 * 'anzahl' Zeiger sein. Rueckgabewert gibt die Zahl der erzeugten 
 * Knoten an. 
 */

int	u_create_node (u_glob, anzahl, nodes)

u_glob_t	*u_glob;
int		anzahl;
u_node_t	**nodes;

   {
    byte	*buffer;
    int		i;

    /* Platz fuer 'anzahl' Knoten allokieren: */
    nodes[0] = (u_node_t *) malloc (anzahl * sizeof (u_node_t));
    if (!nodes[0])
	return (0);
    
    /* Jeder Knoten benoetigt 1 Array mit 'size' Elementen */
    /* Platz fuer alle benoetigten Arrays allokieren: */
    buffer = (byte *) malloc (anzahl * u_glob->size * sizeof (byte));
    if (!buffer)
	return (0);

    for (i = 0; i < anzahl; i++)
       {
	/* Knoten auf die Liste verteilen: */
	nodes[i] = ((u_node_t *) nodes[0]) + i;
	/* Jeder Knoten bekommt 1 Array: */
	nodes[i]->list = buffer;
	buffer += u_glob->size;
       } /* end for */
    return (anzahl);
   }

/*^*/
/*------------------------ U_COPY_NODE -------------------------*/
/* Kopiert den Inhalt eines u_node mit allen evtl. daranhaengenden 
 * verzeigerten Strukturen auf einen anderen u_node. 'size' gibt
 * Groesse des dargestellten Problems an.
 */

void	u_copy_node (u_glob, from, to)

u_glob_t	*u_glob;
u_node_t	*from;
u_node_t	*to;

   {
    byte	*ptr;

    /* Zeiger des Zielknotens retten: */
    ptr = to->list;
    /* Ganzen Knoten per memcpy kopieren: */
    memcpy ((char *) to, (char *) from, sizeof (u_node_t));
    to->list = ptr;
    /* Inhalt des Feldes per memcpy kopieren: */
    memcpy ((char *) to->list, (char *) from->list, 
		     u_glob->size * sizeof (byte));
   }

/*^*/
/*------------------------ U_FLAT_NODE -------------------------*/
/* Legt eine flache Kopie 'flat' des Knotens 'node' an, welcher ein 
 * Teilproblem der Groesse 'size' beschreibt. Diese flache Kopie 
 * muss durch memcpy () kopierbar sein, d.h. sie darf keine inneren 
 * Zeiger mehr enthalten. Ausserdem muss sie zwischen Maschinen 
 * austauschbar sein, darf also keine Zeiger nach aussen mehr 
 * enthalten. Die Funktion u_unflat_node () stellt die Umkehrung 
 * dieser Funktion dar. 
 * 'flat' muss auf ausreichend Speicherplatz zeigen, um einen
 * flachen Knoten aufzunehmen. Zur Bestimmung der Puffergroesse 
 * dient die Funktion u_sizeof_flat () .
 */

void	u_flat_node (u_glob, node, flat)

u_glob_t	*u_glob;
u_node_t	*node;
char		*flat;

   {
   /* Gesamten Knoten (inkl. Zeiger) kopieren: */
    memcpy (flat, (char *) node, sizeof (u_node_t));
    /* Inhalt der Felder per memcpy kopieren: */
    /* (Geht so, weil in u_create_node () zusammenhaengend !!) */
    memcpy (flat + sizeof (u_node_t), node->list,
	    u_glob->size * sizeof (byte));
   }

/*^*/
/*------------------------ U_UNFLAT_NODE -----------------------*/
/* Kopiert einen mit u_flat_node () erzeugten flachen Knoten 'flat' 
 * in einen normalen u_node 'node' zurueck. Die Problemgroesse ist 
 * 'size'. 'node' muss schon mit entsprechender Struktur vorhanden
 * sein.
 */

void	u_unflat_node (u_glob, flat, node)

u_glob_t	*u_glob;
char		*flat;
u_node_t	*node;

   {
    byte	*ptr;

    /* Zeiger des Zielknotens retten: */
    ptr = node->list;
    /* Ganzen Knoten per memcpy kopieren: */
    memcpy ((char *) node, flat, sizeof (u_node_t));
    node->list = ptr;
    /* Inhalt des Feldes per memcpy kopieren: */
    memcpy ((char *) node->list, flat + sizeof (u_node_t), 
		     u_glob->size * sizeof (byte));
   }

/*^*/
/*------------------------ U_SIZEOF_FLAT -----------------------*/
/* Bestimmt zu einer Problemgroesse 'size' die Groesse eines mit
 * u_flat_node () erzeugten flachen Knotens.
 */

int	u_sizeof_flat_node (u_glob)

u_glob_t	*u_glob;

   {
    return (sizeof (u_node_t) + u_glob->size * sizeof (byte));
   }

/*^*/
/*------------------------ U_FLAT_GLOB -------------------------*/
/* Kopiert den Inhalt der Datenstruktur u_glob in einen flachen
 * Speicherbereich flat. Flat wird von der Funktion angelegt. In size
 * wird die Groesse von flat zurueckgeliefert. 
 */ 

int	u_flat_glob (u_glob, flat, size)

u_glob_t	*u_glob;
char		**flat;
int		*size;

   {
    int		*intbuf;
    char	*charbuf;

    /* Berechne Groesse der flachen Struktur: */
    *size = (u_glob->size * 4 + 2) * sizeof (int);

    /* Lege Block fuer flache Struktur an: */
    if ((intbuf = (int *) malloc (*size)) == 0)
	return (U_MALLOC);

    /* size und sum_d0 vorne in den Block kopieren: */
    intbuf[0] = u_glob->size;
    intbuf[1] = u_glob->sum_d0;

    /* Inhalt des d-Arrays in den Block kopieren: */
    /* (Geht so, weil in matrin () zusammenhaengend gebaut) */
    charbuf = (char *) (intbuf + 2);
    memcpy (charbuf, (char *) u_glob->d[0], 
	    u_glob->size * 3 * sizeof (int));
    /* Inhalt von d1d2 in den Block kopieren: */
    charbuf += u_glob->size * 3 * sizeof (int);
    memcpy (charbuf, (char *) u_glob->d1d2, 
	    u_glob->size * sizeof(int));

    *flat = (char *) intbuf;

    return (OK);
   }

/*^*/
/*------------------------ U_UNFLAT_GLOB -----------------------*/
/* Erzeugt aus dem Inhalt des flachen Puffers flat den Inhalt der
 * Datenstruktur u_glob. U_glob wird dabei angelegt. Beim Anlegen von
 * flat muss in geeigneter Weise die Groesse mitgeliefert werden. 
 */ 

int	u_unflat_glob (flat, u_glob)

char		*flat;
u_glob_t	**u_glob;

   {
    int		*intbuf;
    char	*charbuf;
    int		*mat_buf;
    int		i;
    int		size;

    /* Platz fuer Datenstruktur u_glob allokieren: */
    *u_glob = (u_glob_t *) malloc (sizeof (u_glob_t));
    if (!*u_glob)
	return (U_MALLOC);

    /* Hole Problemgroesse aus flat heraus: */
    intbuf = (int *) flat;
    (*u_glob)->size = intbuf[0];
    (*u_glob)->sum_d0 = intbuf[1];

    /* Lege Zeilenfeld in u_glob an: */
    (*u_glob)->d = (int **) malloc (3 * sizeof (int *));
    if (!(*u_glob)->d)
	return (U_MALLOC);

    /* Lege Matrixkoerper fuer d und d1d2 an: */
    mat_buf = (int *) malloc (size = 4 * (*u_glob)->size 
			             * sizeof (int));
    if (!mat_buf)
	return (U_MALLOC);

    /* Kopiere Matrix (und d1d2 gleich mit): */
    charbuf = (char *) (intbuf + 2);
    memcpy (mat_buf, charbuf, size);
    
    /* Setze Zeilenpointer in d: */
    for (i = 0; i < 3; i++)
	(*u_glob)->d[i] = mat_buf + i * (*u_glob)->size;

    /* Setze Zeiger auf d1d2: */
    (*u_glob)->d1d2 = mat_buf + 3 * (*u_glob)->size;
    
    return (OK);
   }

/*------------------------ U_TRACE_NODE -------------------------*/

char	*u_trace_node (u_node, wp)

u_node_t	*u_node;
char		*wp;

    {
    int		a;

    /* Ecken rueckwaerts aus fixed lesen: */
    a = u_node->fixed;
#ifndef UNDEFINED
    wp += SPRINTF ((wp, "FIXED:"));
    while (a != -1)
       {
	wp += SPRINTF ((wp, "%3d", a));
	a = u_node->list[a];
       }
#else
    wp += SPRINTF ((wp, "u_node->list:"));
    for (a = 0; a < 5; a++)
	wp += SPRINTF ((wp, "%3d", u_node->list[a]));
#endif
    wp += SPRINTF ((wp, "\n"));
    wp += SPRINTF ((wp, "u_node->fw[0]      :%8d\n", 
		   u_node->fw[0]));
    wp += SPRINTF ((wp, "u_node->fw[1]      :%8d\n", 
		   u_node->fw[1]));
    wp += SPRINTF ((wp, "u_node->fw[2]      :%8d\n", 
		   u_node->fw[2]));
    wp += SPRINTF ((wp, "u_node->sum_d1     :%8d\n", 
		   u_node->sum_d1));
    wp += SPRINTF ((wp, "u_node->sum_d2     :%8d\n", 
		   u_node->sum_d2));
    wp += SPRINTF ((wp, "u_node->min_d2     :%8d\n", 
		   u_node->min_d2));
    wp += SPRINTF ((wp, "u_node->min_d1d2   :%8d\n", 
		   u_node->min_d1d2));
    wp += SPRINTF ((wp, "u_node->l_bound    :%8d\n", 
		   u_node->l_bound));
    wp += SPRINTF ((wp, "u_node->depth      :%8d\n", 
		   u_node->depth));
   }

/*------------------------ ENDE ---------------------------------*/
