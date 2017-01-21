/*---------------------------------------------------------------*/
/* u_tsp.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.2		1.6.93 					 */
/*---------------------------------------------------------------*/
/* Benutzerfunktionen fuer das Traveling Salesman Problem.
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  "u_tsp_types.h"
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
static int	getsmin ();
static int	getzmin ();
static int	getrsmin ();
static int	getrzmin ();

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

static int	getsmin (
			u_glob_t*	/* u_glob */, 
			int		/* s */, 
			byte*		/* list */, 
			int		/* anker */, 
			int		/* single */);
static int	getzmin (
			u_glob_t*	/* u_glob */, 
			int		/* z */, 
			byte*		/* list */, 
			int		/* anker */, 
			int		/* single */);

static int	getrsmin (
			u_glob_t*	/* u_glob */, 
			int		/* s */, 
			byte*		/* list */, 
			int		/* anker */, 
			int		/* single */, 
			byte*		/* zmin */);

static int	getrzmin (
			u_glob_t*	/* u_glob */, 
			int		/* z */, 
			byte*		/* list */, 
			int		/* anker */, 
			int		/* single */, 
			byte*		/* smin */);
#endif

/*^*/
/*------------------------ U_MK_STARTNODE -----------------------*/
/* Erzeugt den Knoten fuer das Ausgangsproblem als Wurzel des 
 * BB-Baums. Der Speicherplatz fuer 'startnode' wird von der 
 * rufenden Funktion bereitgestellt. Der Rueckgabewert ist OK oder 
 * ein problemspezifischer Statuswert. 
 */
/* In TSP werden alle Ecken in die Free-List eingetragen. Die Zeilen- 
 * und Spaltenminima werden komplett berechnet.
 */

int	u_mk_start_node (u_glob, startnode)

u_glob_t	*u_glob;
u_node_t	*startnode;

   {
    int		i;
    int		j;

    /* Fixedlist enthaelt nur die 0: */
    startnode->fixed = 0;
    startnode->list[0] = -1;

    /* Freelist enthaelt alle anderen: */
    startnode->free = 1;
    for (i = 1; i < u_glob->size - 1; i++)
       {
	startnode->list[i] = i + 1;
       } /* end for */
    startnode->list[u_glob->size - 1] = -1;
    
    /* Zeilen- und Spaltenminima initialisieren: */
    for (i = 0; i < u_glob->size; i++)
       {
	startnode->zmin[i] = i;
	startnode->smin[i] = i;
	for (j = 0; j < u_glob->size; j++)
	   {
	    if (u_glob->d[i][j] < u_glob->d[i][startnode->zmin[i]])
		startnode->zmin[i] = j;
	    if (u_glob->d[j][i] < u_glob->d[startnode->smin[i]][i])
		startnode->smin[i] = j;
	   } /* end for */
       } /* end for */
    
    startnode->fixed_cost = 0;
    startnode->l_bound = 0;
    startnode->depth = 1;
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
/* In TSP wird der erste freie Knoten des Vaters selektiert. Der 
 * Sohn uebernimmt alle Daten vom Vater. Die 'fixed_cost' im Sohn 
 * werden aktualisiert, die Minima werden zunaechst uebernommen und 
 * erst bei der Schrankenberechnung angepasst. 
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
    /* Kosten zu fixed_cost hinzurechnen: */
    son->fixed_cost += u_glob->d[son->fixed][*selection];
    /* Aus freelist aushaengen: */
    son->free = son->list[son->free];
    /* An fixed_list (vorne) dranhaengen: */
    son->list[*selection] = son->fixed;
    son->fixed = *selection;
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
/* In TSP wird die auf 'selection' folgende freie Ecke gewaehlt und 
 * wie in u_first_child bearbeitet. 
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
    /* Kosten zu fixed_cost hinzurechnen: */
    son->fixed_cost += u_glob->d[son->fixed][*selection];
    /* Aus freelist aushaengen: */
    son->list[last] = son->list[*selection];
    /* An fixed_list (vorne) dranhaengen: */
    son->list[*selection] = son->fixed;
    son->fixed = *selection;
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
/* In TSP wird fuer Knoten mit genau einer freien Ecke die Loesung 
 * durch Eintragen dieser Ecke und Schliessen des Kreises 
 * vervollstaendigt. Dabei werden Loesungen, die mindestens eine 
 * MAXINT/n-Kante enthalten, stets mit MAXINT bewertet. Der 
 * Zielfunktionswert wird in den Knoten (anstelle der L-Bound) 
 * eingetragen, um bei erneutem Aufruf der Funktion fuer denselben 
 * Knoten die Neuberechnung zu vermeiden. 
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
	*value = /* Alter Wert: */
	         node->fixed_cost
		 /* plus Weg von vorletzter zu letzter Ecke: */
	       + u_glob->d[node->list[node->fixed]][node->fixed]
		 /* plus Rueckweg zur 0: */
	       + u_glob->d[node->fixed][0];
	/* Loesungen, die ein M enthalten, immer mit ZF = MAXINT: */
	if (*value >= MAXINT / u_glob->size)
	    *value = MAXINT;
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
/* In TSP wird die Heuristik "Naechster Nachbar" fuer das TSP
 * verwendet. Dabei werden die bereits bekannten Zeilenminima soweit
 * moeglich ausgenutzt. 
 */

int	u_heur_loes (u_glob, node, solution, value)

u_glob_t	*u_glob;
u_node_t	*node;
u_node_t	*solution;
u_zf_t		*value;

   {
    byte		*isfree;
    byte		*list;
    byte		*pos;
    int			head;
    int			next;
    int			free;
    int			i;
    int			v;

    /* U-Node fuer Loesung kopieren: */
    u_copy_node (u_glob, node, solution);

    /* Speicherplatz von zmin und smin in Loesung "umnutzen" */
    isfree = solution->zmin;
    pos = solution->smin;

    /* Kuerzere Schreibweise fuer free und list: */
    free = solution->free;
    list = solution->list;

    /* Nachschlageliste der freien Ecken aufbauen: */
    memset (isfree, 0, u_glob->size * sizeof (byte));
    for (i = free, v = -1; i != -1; v = i, i = list[i])
       {
	/* pos[i] ist die Position der Zahl i in 'list' */
	/* pos[i] == -1 steht fuer free == i */
	pos[i] = v;
	isfree[i]++;
       }

    /* Beginne mit bereits entstandenen Kosten: */
    *value = node->fixed_cost;
    head = node->fixed;

    /* Solange noch Ecken frei: */
    while (free != -1)
       {
	/* Wenn altes zmin noch gueltig: */
	if (isfree[node->zmin[head]])
	    next = node->zmin[head];
	else 
	    next = getzmin (u_glob, head, list, free, free);

	/* Aus free aushaengen: */
	/* Wenn next in free steht: */
	if (pos[next] == -1)
	   {
	    /* Free zeigt jetzt auf Nachfolger von free: */
	    free = list[free];
	    /* d.h. Nachfolger von Free steht jetzt in free: */
	    if (free != -1)
		pos[free] = -1;
	   }
	else
	   {
	    /* pos[next] war Vorgaenger von next: */
	    list[pos[next]] = list[next];
	    if (list[next] != -1)
		pos[list[next]] = pos[next];
	   }
	/* In isfree ausstreichen: */
	isfree[next]--;
	/* Eintragen in Loesung: */
	solution->list[next] = solution->fixed;
	solution->fixed = next;
	*value += u_glob->d[head][next];
	head = next;
       }

    /* Rueckweg zur 0 beruecksichtigen: */
    *value += u_glob->d[head][0];
    /* Im Loesungsknoten steht vollstaendige Loesung: */
    solution->free = -1;
    solution->l_bound = *value;
    solution->depth = u_glob->size;
    return (OK);
   }

/*^*/
/*------------------------ U_INIT_GLOB -------------------------*/
/* Initialisiert die Datenstruktur 'u_glob', die das zu loesende 
 * Problem vollstaendig beschreibt. Diese Daten werden an alle 
 * problemspezifischen Funktionen weitergegeben und dienen zur 
 * parametrischen Beschreibung von BB-Knoten. 
 * Zur Eingabe koennen Funktionen aus 'read.c' verwendet werden.
 * Da diese Funktion nur auf dem Root-Transputer eingesetzt werden
 * kann, duerfen Fehler direkt auf Standardausgabe gemeldet werden. 
 * Der Rueckgabewert ist OK oder ein problemspezifischer Statuswert 
 * bei Fehlern. 
 */
/* In TSP wird ein Paramater 'mat' mittels read_param () gelesen 
 * und die Entfernungsmatrix aus der dadurch bestimmten Datei mit 
 * Hilfe von matrin () gelesen. 
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

    (*u_glob)->d = matrix;
    (*u_glob)->size = zeilen;
    return (OK);
   }

/*^*/
/*------------------------ U_IS_FEASIBLE ------------------------*/
/* Stellt fest, ob der Knoten 'node' eine zulaessige Loesung des 
 * Ausgangsproblems repraesentiert. Von solchen Knoten koennen 
 * keine Soehne mehr erzeugt werden. Der Rueckgabewert ist OK_TRUE, 
 * wenn 'node' eine zulaessige Loesung enthaelt, sonst OK_FALSE. 
 */
/* In TSP wird ueberprueft, ob die Liste der freien Ecken nur noch 
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
/*-------------------------- GETZMIN ----------------------------*/
/* Hilfsfunktion fuer u_lower_bound (). Liefert das Minimum von 
 * Zeile z in der Entfernungsmatrix, wobei nur die Spalten, die in 
 * der durch anker und list gegebenen Indexliste, sowie die Spalte 
 * single beruecksichtigt werden. 
 * min { d[z][i] | i aus liste oder i = single }
 */

static int	getzmin (u_glob, z, list, anker, single)

int		z;
byte		*list;
int		anker;
int		single;
u_glob_t	*u_glob;

   {
    int		zmin;
    int		i;

    zmin = anker;
    for (i = anker; i != -1; i = list[i])
	if (u_glob->d[z][i] < u_glob->d[z][zmin] && i != z)
	    zmin = i;
    if (u_glob->d[z][single] < u_glob->d[z][zmin])
	return (single);
    else
	return (zmin);
   }

/*^*/
/*-------------------------- GETSMIN ----------------------------*/
/* Hilfsfunktion fuer u_lower_bound (). Liefert das Minimum von 
 * Spalte s in der Entfernungsmatrix, wobei nur die Zeilen, die in 
 * der durch anker und list gegebenen Indexliste, sowie die Zeile 
 * single beruecksichtigt werden. 
 * min { d[i][s] | i aus liste oder i = single }
 */

static	int	getsmin (u_glob, s, list, anker, single)

int		s;
byte		*list;
int		anker;
int		single;
u_glob_t	*u_glob;

   {
    int		smin;
    int		i;

    smin = anker;
    for (i = anker; i != -1; i = list[i])
	if (u_glob->d[i][s] < u_glob->d[smin][s] && i != s)
	    smin = i;
    if (u_glob->d[single][s] < u_glob->d[smin][s])
	return (single);
    else
	return (smin);
   }

/*------------------------ GETRSMIN ----------------------------*/

static int	getrsmin (u_glob, s, list, anker, single, zmin)

u_glob_t	*u_glob;
int		s;
byte		*list;
int		anker;
int		single;
byte		*zmin;

   {
    int		rsmin;
    int		i;
    int		**d = u_glob->d;

    rsmin = d[anker][s] - d[anker][zmin[anker]];
    for (i = anker; i != -1; i = list[i])
	if (d[i][s] - d[i][zmin[i]] < rsmin && i != s)
	    rsmin = d[i][s] - d[i][zmin[i]];
    if (d[single][s] - d[single][zmin[single]] < rsmin && single != s)
	rsmin = d[single][s] - d[single][zmin[single]];
    return (rsmin);
   }

/*------------------------ GETRZMIN ----------------------------*/

static int	getrzmin (u_glob, z, list, anker, single, smin)

u_glob_t	*u_glob;
int		z;
byte		*list;
int		anker;
int		single;
byte		*smin;

   {
    int		rzmin;
    int		j;
    int		**d = u_glob->d;

    rzmin = d[z][anker] - d[smin[anker]][anker];
    for (j = anker; j != -1; j = list[j])
	if (d[z][j] - d[smin[j]][j] < rzmin && j != z)
	    rzmin = d[z][j] - d[smin[j]][j];
    if (d[z][single] - d[smin[single]][single] < rzmin && single != z)
	rzmin = d[z][single] - d[smin[single]][single];
    return (rzmin);
   }

/*^*/
/*------------------------ U_LOWER_BOUND ------------------------*/
/* Liefert die untere Schranke des Zielfunktionswertes fuer den 
 * Knoten 'node'. Diese Schranke wird nach einem 
 * problemspezifischen Verfahren berechnet.  Es ist 
 * zulaessig, dass 'node' beim Aufruf der Funktion veraendert wird. 
 * (Anm: Da die Funktion u.U. mehrmals fuer denselben Knoten 
 * aufgerufen wird, ist es zweckmaessig, den Wert der unteren 
 * Schranke im Knoten abzulegen und bei erneuten Aufrufen direkt 
 * zurueckzugeben.)
 */
/* In TSP wird die reduzierte Entfernungsmatrix zur Berechnung der
 * unteren Schranke verwendet. Dabei wird sowohl eine Zeilen/Spalten-
 * als auch eine Spalten/Zeilen-Reduktion durchgefuehrt. 
 * Zur Berechnung werden die vom Vater uebernommenen Zeilen- und 
 * Spaltenminima teilweise aktualisiert (siehe Schema unten). Die 
 * aktualisierten Werte werden in node eingetragen. 
 * Die Berechnung der Schranke beruht auf der Tatsache, dass in 
 * jede Ecke genau eine Kante hinein- und hinausfuehrt und 
 * erfolgt nach folgendem Schema: 
 * Sei AUS die Menge der Ecken, aus denen noch keine Kante herausfuehrt.
 *     AUS = freelist + {Ende der fixedlist}
 * Sei IN die Menge der Ecken, in die noch keine Kante hineinfuehrt.
 *     IN = freelist + {0}
 * Sei zmin[i] der Index des Zeilenminimums der Zeile i in der 
 * Entfernungsmatrix ueber die Spalten aus IN.
 *     d[i][zmin[i]] = min {d[i][j] | j in der Menge IN}
 * Sei smin[j] der Index des Spaltenminimums der Spalte j in der 
 * Entfernungsmatrix ueber die Zeilen aus OUT.
 *     d[smin[j]][j] = min {d[i][j] | i in der Menge OUT}
 * Dann ergeben sich zwei untere Schranken zlbound und slbound, deren
 * Maximum verwendet wird:
 * Sei zlbound die Summe der minimalen Ausgangskantenlaengen von 
 * allen Ecken, aus denen noch Kanten herausfuehren muessen, zu 
 * allen Ecken, in die noch Kanten hineinfuehren muessen. 
 *     zlbound = Summe aller {d[i][zmin[i]] | i aus der Menge OUT}
 * Sei slbound die Summe der minimalen Eingangskantenlaengen zu 
 * allen Ecken, in die noch Kanten hineinfuehren muessen, von 
 * allen Ecken, aus denen noch Kanten herausfuehren muessen. 
 *     zlbound = Summe aller {d[smin[j]][j] | j aus der Menge IN}
 *
 * Die Werte fuer zmin und smin wurden bei der Erzeugung von 
 * Soehnen aus dem Vater kopiert. Vor der Berechnung der unteren 
 * Schranke muessen diese an einigen Stellen aktualisiert werden 
 * (Ausnahme: Wurzelknoten). Zur Verdeutlichung dieser Stellen 
 * diene folgende Skizze: 
 *
 *   Verdeutlichung der Minima-Neuberechnung anhand einer 
 *   Entfernungsmatrix des Vaters und des Sohnes:
 *
 *        VATER                          SOHN
 *
 *              f f f   f                      f f f   f     
 *              r r r S r                      r r r   r     
 *              e e e E e                      e e e L e     
 *              e e e L e                      e e e F e     
 *        M . . . . . . .                M . . . . . . .     
 *   LF   . M . X X X S X           VLF  . M . o o o . o     
 *        . . M . . . . .                . . M . . . . .     
 *   free X . . M X X X X           free X . . M X X o X <   
 *   free X . . X M X X X           free X . . X M X o X <   
 *   free X . . X X M X X           free X . . X X M o X <   
 *   free X . . X X X M X           LF   o . . X X X M X <   
 *   free X . . X X X X M           free X . . X X X o M <   
 *                                       ^     ^ ^ ^   ^    
 *   Legende:
 *   --------
 *   free: Nummern aus freelist
 *   SEL : Beim Vater selektiertes Element
 *   LF	 : letzter der fixedlist
 *   VLF : vorletzter der fixedlist
 *   M	 : MAXINT
 *   X	 : Fuer Minimum noch wichtig
 *   .	 : Fuer Minima nicht mehr relevant
 *   o	 : Aus Minimumsbildung ausgeschieden (im Sohn)
 *   <	 : Zeilen, die neu bewertet werden (fuer Sohn)
 *   ^	 : Spalten, die neu bewertet werden (fuer Sohn)
 */

int	u_lower_bound (u_glob, node, l_bound)

u_glob_t	*u_glob;
u_node_t	*node;
u_zf_t		*l_bound;

   {
    int		i;
    int		zlbound;
    int		slbound;
    int		lf;
    int		vlf;
    int		**d;
    static byte	*is_in_zmin;
    static byte	*is_in_smin;
    int		rlbound;
    int		j;

    /* Wenn Bound bereits berechnet: */
    if (node->l_bound)
       {
	/* Fertige Bound zurueckgeben und aus: */
	*l_bound = node->l_bound;
	return (OK);
       } /* end if */

    /* Wenn Knoten schon ein M enthaelt: */
    if (node->fixed_cost >= MAXINT / u_glob->size)
       {
	*l_bound = MAXINT;
	return (OK);
       } /* end if */
	
    /* Beim ersten Durchlauf statische Felder allokieren :*/
    if (!is_in_zmin)
       {
	is_in_zmin = (byte *) malloc (u_glob->size * sizeof (byte));
	if (!is_in_zmin)
	    return (U_MALLOC);
	is_in_smin = (byte *) malloc (u_glob->size * sizeof (byte));
	if (!is_in_zmin)
	    return (U_MALLOC);
       }

    /* Nachschlagefelder auf Null setzen: */
    memset ((char *) is_in_zmin, 0, u_glob->size * sizeof (byte));
    memset ((char *) is_in_smin, 0, u_glob->size * sizeof (byte));

    zlbound = 0;
    slbound = 0;
    rlbound = 0;
    d = u_glob->d;

    /* Wenn node die Wurzel ist: */
    if (node->fixed == 0)
       {
	for (i = 0; i < u_glob->size; i++)
	   {
	    zlbound += d[i][node->zmin[i]];
	    slbound += d[node->smin[i]][i];
	   } /* end for */
       }
    else /* node enthaelt Eltern-Minima */
       {
	lf = node->fixed;
	vlf = node->list[lf];
	/* Minima neu berechnen (dabei aufsummieren): */
	/* Fuer alle i aus free */
	for (i = node->free; i != -1; i = node->list[i])
	   {
	    /* Wenn (VLF/i) Minimum von Spalte i */
	    if (vlf == node->smin[i])
		/* Neues Minimum suchen ueber free und LF */
		node->smin[i] = getsmin (u_glob, i, node->list,
					 node->free, lf);
	    /* Wenn (i/LF) Minimum von Zeile i */
	    if (lf == node->zmin[i])
		/* Neues Minimum suchen ueber free und 0 */
		node->zmin[i] = getzmin (u_glob, i, node->list, 
					 node->free, 0);
	    zlbound += d[i][node->zmin[i]];
	    slbound += d[node->smin[i]][i];
	    is_in_zmin[node->zmin[i]] = OK_TRUE;
	    is_in_smin[node->smin[i]] = OK_TRUE;
	   }

	/* Wenn der Punkt (LF/0) Minimum von Zeile LF */
	if (0 == node->zmin[lf])
	    /* Neues Minimum suchen ueber free */
	    node->zmin[lf] = getzmin (u_glob, lf, node->list, 
				      node->free, node->free);
	/* Wenn der Punkt (LF/0) Minimum von Spalte 0 */
	if (lf == node->smin[0])
	    /* Neues Minimum suchen ueber free */
	    node->smin[0] = getsmin (u_glob, 0, node->list,
				     node->free, node->free);
	zlbound += d[lf][node->zmin[lf]];
	slbound += d[node->smin[0]][0];
	is_in_zmin[node->zmin[lf]] = OK_TRUE;
	is_in_zmin[node->smin[0]] = OK_TRUE;
       } /* end else */

    if (zlbound >= slbound)
       {
	/* T ("Reduziere zlbound"); */
	rlbound = zlbound;
	for (j = node->free; j != -1; j = node->list[j])
	   {
	    if (!is_in_zmin[j])
	       {
		rlbound += getrsmin (u_glob, j, node->list, node->free, 
				     lf, node->zmin);
	       }
	   }
	if (!is_in_zmin[0])
	    rlbound += getrsmin (u_glob, 0, node->list, node->free, 
				 node->free, node->zmin);
       }
    else
       {
	/* T ("Reduziere slbound"); */
	rlbound = slbound;
	for (i = node->free; i != -1; i = node->list[i])
	   {
	    if (!is_in_smin[i])
	       {
		rlbound += getrzmin (u_glob, i, node->list, node->free, 
				     0, node->smin);
	       }
	   }
	if (!is_in_smin[lf])
	    rlbound += getrzmin (u_glob, lf, node->list, node->free, 
				 node->free, node->smin);
       }
    /* TV ("rlbound", rlbound); */

    *l_bound = node->fixed_cost + rlbound;

    /* Wenn Lower Bound ein M enthaelt: */
    if (*l_bound >= MAXINT / u_glob->size)
	*l_bound = MAXINT;
	
    node->l_bound = *l_bound;
    return (OK);
   }

/*^*/
/*---------------------------- U_PRIORITY -----------------------*/
/* Berechnet die Prioritaet des Knotens node. Hier wird fuer alle
 * Instanzen die in der Arbeit beschriebene parametrische 
 * Prioritaetsfunktion verwendet. Fuer einen Parameter, der kleiner
 * als 0 ist, kann zusaetzlich reine Tiefensuche verlangt werden.
 * Diese Eigenschaft dient zu Testzwecken und ist im Text der Arbeit
 * nicht dokumentiert.
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
/* In TSP wird dazu die Liste der fixierten Ecken rueckwaerts
 * ausgegeben.
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
	/* (Platz in zmin benutzen, da jetzt ungenutzt) */
	i = sol->depth - 1;
	for (a = sol->fixed; a != -1; a = sol->list[a])
	    sol->zmin[i--] = a;
	for (i = 0; i < sol->depth; i++)
	    printf ("%4d", sol->zmin[i]);
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
/* Beim TSP werden dabei die 3 Arrays zusammenhaengend angelegt, um sie
 * in den flat-Funktionen mit einem memcpy kopieren zu koennen.
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
    
    /* Jeder Knoten benoetigt 3 Arrays mit je 'size' Elementen */
    /* Platz fuer alle benoetigten Arrays allokieren: */
    buffer = (byte *) malloc (anzahl * u_glob->size 
			      * 3 * sizeof (byte));
    if (!buffer)
	return (0);

    for (i = 0; i < anzahl; i++)
       {
	/* Knoten auf die Liste verteilen: */
	nodes[i] = ((u_node_t *) nodes[0]) + i;
	/* Jeder Knoten bekommt 3 Arrays: */
	nodes[i]->list = buffer;
	nodes[i]->zmin = buffer + u_glob->size;
	nodes[i]->smin = buffer + 2 * u_glob->size;
	buffer += 3 * u_glob->size;
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
    /* Alles bis auf die Zeiger normal kopieren: */
    to->fixed = from->fixed;
    to->free = from->free;
    to->fixed_cost = from->fixed_cost;
    to->l_bound = from->l_bound;
    to->depth = from->depth;
    /* Inhalt der Felder per memcpy kopieren: */
    /* (Geht so, weil in u_create_node () zusammenhaengend !!) */
    (void *) memcpy ((char *) to->list, (char *) from->list, 
		     3 * u_glob->size * sizeof (byte));
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
	    3 * u_glob->size * sizeof (byte));
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
    /* Alles bis auf die Zeiger normal kopieren: */
    node->fixed = ((u_node_t *) flat)->fixed;
    node->free = ((u_node_t *) flat)->free;
    node->fixed_cost = ((u_node_t *) flat)->fixed_cost;
    node->l_bound = ((u_node_t *) flat)->l_bound;
    node->depth = ((u_node_t *) flat)->depth;
    /* Inhalt der Felder per memcpy kopieren: */
    /* (Geht so, weil in u_create_node () zusammenhaengend !!) */
    memcpy (node->list, flat + sizeof (u_node_t),
	    3 * u_glob->size * sizeof (byte));
   }

/*^*/
/*------------------------ U_SIZEOF_FLAT -----------------------*/
/* Bestimmt zu einer Problemgroesse 'size' die Groesse eines mit
 * u_flat_node () erzeugten flachen Knotens.
 */

int	u_sizeof_flat_node (u_glob)

u_glob_t	*u_glob;

   {
    return (sizeof (u_node_t) + 3 * u_glob->size * sizeof (byte));
   }

/*^*/
/*------------------------ U_FLAT_GLOB -------------------------*/
/* Kopiert den Inhalt der Datenstruktur u_glob in einen flachen
 * Speicherbereich flat. Flat wird von der Funktion angelegt. In size
 * wird die Groesse von flat zurueckgeliefert. 
 */ 
/* Beim TSP wird die gesamte Entfernungsmatrix in einem memcpy kopiert.
 * Dies ist moeglich, weil in matrin() ein zusammenhaengender
 * Speicherblock verwendet wurde.
 */

int	u_flat_glob (u_glob, flat, size)

u_glob_t	*u_glob;
char		**flat;
int		*size;

   {
    int		*intbuf;
    char	*charbuf;

    /* Berechne Groesse der flachen Struktur: */
    *size = (u_glob->size * u_glob->size + 1) * sizeof (int);

    /* Lege Block fuer flache Struktur an: */
    if ((intbuf = (int *) malloc (*size)) == 0)
	return (U_MALLOC);

    /* u_glob->size vorne in den Block kopieren: */
    intbuf[0] = u_glob->size;

    /* Inhalt des d-Arrays in den Block kopieren: */
    /* (Geht so, weil in matrin () zusammenhaengend gebaut) */
    charbuf = (char *) (intbuf + 1);
    memcpy (charbuf, u_glob->d[0], *size - sizeof (int));

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

    /* Lege Zeilenfeld in u_glob an: */
    (*u_glob)->d = (int **) malloc ((*u_glob)->size * sizeof (int *));
    if (!(*u_glob)->d)
	return (U_MALLOC);

    /* Lege Matrixkoerper fuer u_glob an: */
    mat_buf = (int *) malloc (size = (*u_glob)->size * (*u_glob)->size 
			             * sizeof (int));
    if (!mat_buf)
	return (U_MALLOC);

    /* Kopiere Matrix: */
    charbuf = (char *) (intbuf + 1);
    memcpy (mat_buf, charbuf, size);
    
    /* Setze Zeilenpointer in u_glob: */
    for (i = 0; i < (*u_glob)->size; i++)
	(*u_glob)->d[i] = mat_buf + i * (*u_glob)->size;
    
    return (OK);
   }

/*------------------------ U_TRACE_NODE -------------------------*/
/* Gibt den Inhalt eines u_nodes in einen String aus, der dann als
 * Trace-Information ausgegeben werden kann. Wird von den
 * Funktionen in trace.c aufgerufen, wenn Tracing eingeschaltet ist.
 */

char	*u_trace_node (u_node, wp)

u_node_t	*u_node;
char		*wp;

   {
    int		a;

    /* Ecken rueckwaerts aus fixed lesen: */
    a = u_node->fixed;
    wp += SPRINTF ((wp, "FIXED:"));
    while (a != -1)
       {
	wp += SPRINTF ((wp, "%3d", a));
	a = u_node->list[a];
       }
    wp += SPRINTF ((wp, "\n"));
    wp += SPRINTF ((wp, "u_node->fixed_cost :%8d\n", 
		   u_node->fixed_cost));
    wp += SPRINTF ((wp, "u_node->l_bound    :%8d\n", 
		   u_node->l_bound));
    wp += SPRINTF ((wp, "u_node->depth      :%8d\n", 
		   u_node->depth));
    return (wp);
   }

/*------------------------ ENDE ---------------------------------*/
