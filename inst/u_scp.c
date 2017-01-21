/*---------------------------------------------------------------*/
/* u_scp.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.2		1.6.93 					 */
/*---------------------------------------------------------------*/
/*  Benutzerfunktionen fuer das Set Covering Problem
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

/* TRACE_ON nur mit sequentiellem Kern moeglich: */
/* #define         TRACE_ON */
/* REAL_HEURISTIC schaltet die verwendung der Heuristik ein.  
 * Ist diese Konstante nicht definiert, wird eine Dummy-Heuristik
 * verwendet, die keine Berechnung durchfuehrt.
 */
#define		REAL_HEURISTIC

#include  <stdio.h>
/* stdlib auf Sun nicht vorhanden, daher hier ausgeschaltet, damit
 * Instanzfunktionen auch auf der Sun kompilierbar. Reine Kosmetik.
 */
#ifndef SUN_CC
/* #include  <stdlib.h> */
#endif
#include  <string.h>
#include  "u_scp_types.h"
#include  "u_funcs.h"
#include  "read.h"
#include  "set.h"
#include  "error.h"

#ifdef TRACE_ON
#include trace.h
#else
#define  T(STR)		{ ; /* nix */ }
#define  TV(STR, VAL)	{ ; /* nix */ }
#define  TA(ARR, LEN)	{ ; /* nix */ }
#define  TKN(STR, NODE)	{ ; /* nix */ }
#define  TS(STR, NODE)	{ ; /* nix */ }
#endif

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

int	problem_size;

/*------------------------ MAKROS -------------------------------*/

#define  MAX(A, B)	((A > B) ? A : B)
#define  MIN(A, B)	((A < B) ? A : B)

/*------------------------ FUNKTIONEN ---------------------------*/

/* Compiler auf der Sun versteht keine Prototypen, daher fuer Sun
 * ausgeblendet
 */
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
static int	compare_val ();
static int	make_consistent ();

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

public void	u_copy_node (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* from */, 
			u_node_t*	/* to */);

public int	u_sizeof_flat_node (
			u_glob_t*	/* u_glob */);

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

static int	compare_val (
			int*		/* p */,
			int*		/* q */);

static int	make_consistent (
			u_glob_t*	/* u_glob */,
			u_node_t*	/* node */,
			set_t*		/* X */,
			set_t*		/* Y */);

#endif


/* TC- Library-Deklarationen unvertraeglich mit compare_val: */
extern void	*malloc ();
extern void	qsort ();

/*^*/
/*------------------------ U_MK_STARTNODE -----------------------*/
/* Erzeugt den Knoten fuer das Ausgangsproblem als Wurzel des 
 * BB-Baums. Der Speicherplatz fuer 'startnode' wird von der 
 * rufenden Funktion bereitgestellt. Der Rueckgabewert ist OK oder 
 * ein problemspezifischer Statuswert. 
 */
/* Fuer das Ueberdeckungsproblem wird ein Knoten mit leeren Mengen
 * A und B angelegt. Dieser Knoten wird dann mit den Dominanzkriterien
 * in 'make_consistent()' bearbeitet. Fehler koennen hierbei nicht
 * auftreten.
 */

int	u_mk_start_node (u_glob, startnode)

u_glob_t	*u_glob;	/* IN: Parameter des Problems	*/
u_node_t	*startnode;	/* OUT: BB-Knoten fuer das 
				 * Ausgangsproblem 
				 */
   {
    /* Erzeuge Menge A und B als leere Mengen: */
    flush_set (startnode->A);
    flush_set (startnode->B);
    flush_set (startnode->succA);
    startnode->fixed_cost = 0;
    startnode->l_bound = 0;
    startnode->depth = 1;
    /* Mache Startknoten konsistent: */
    T("Startknoten")
    make_consistent (u_glob, startnode, u_glob->X, u_glob->Y);
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
/* Fuer das Ueberdeckungsproblem wird zunaechst die Menge der 
 * freien Ecken Y_A_B durch Differenzbildung bestimmt. Aus dieser 
 * Menge wird diejenige mit dem guenstigsten Quotienten aus Zahl 
 * der neu ueberdeckten Ecken und Wert ausgewaehlt. Diese Ecke wird 
 * beim Sohn zu A hinzugefuegt, nachdem alle Daten des Vaters 
 * kopiert worden sind. Der Sohn wird mit den Dominanzkriterien in 
 * 'make_consistent()' behandelt. Alle Werte im Sohn werden 
 * entsprechen aktualisiert. Die gewaehlte Ecke wird in 'selection'
 * zurueckgegeben.
 */

boolean	u_first_child (u_glob, parent, son, selection)

u_glob_t	*u_glob;
u_node_t	*parent;
u_node_t	*son;
int		*selection;

   {
    static set_t	*Y_A_B;
    static set_t	*succy_succA;
    float		maxvalue;
    float		value;
    int			max;
    int			y;

    /* Beim ersten Mal Y_A_B und succy_succA allokieren: */
    if (!Y_A_B)
       {
	if (!create_set (parent->A->start, parent->A->size, &Y_A_B))
	    return (U_MALLOC);
	if (!create_set (parent->succA->start, parent->succA->size, 
			 &succy_succA))
	    return (U_MALLOC);
       }

    T ("u_first_child")

    /* Y - A - B bestimmen: */
    minus (u_glob->Y, parent->A, Y_A_B);
    minus (Y_A_B, parent->B, Y_A_B);

    TS ("Y-A-B", Y_A_B);

    /* Waehle geeignete Ecke y aus Y - A - B aus: */
    /* Setze Maximum auf 0 */
    maxvalue = 0;
    max = get_first (Y_A_B);
    /* Fuer alle y aus Y - A - B: */
    for (y = get_first (Y_A_B); y != -1; y = get_next (Y_A_B))
       {
	TV ("y", y)
	if (u_glob->c[y] == 0)
	   { 
	    max = y;
	    maxvalue = MAXINT;
	    continue;
	   }
	/* Berechne |succ(y) - succ(A)| / c(y): */
	minus (u_glob->succ[y], parent->succA, succy_succA);
	value = (float) nr_of_el (succy_succA) / u_glob->c[y];
	TV ("value", (int) (value * 1000));
	/* Wenn Quotient groesser als Maximum: y nehmen */
	if (value > maxvalue)
	   {
	    max = y;
	    maxvalue = value;
	   }
       }

    /* Selektiert wurde y: */
    y = max;

    TV ("Selected for A", y)

    /* Kopiere Vater auf Sohn: */
    u_copy_node (u_glob, parent, son);
    /* Bilde (A + y, B) im Sohn: */
    add_elem (y, son->A, son->A);
    /* Aktualisiere succ(A) im Sohn: */
    union_set (son->succA, u_glob->succ[y], son->succA);
    /* Addiere c(y) zu fixed_cost: */
    son->fixed_cost += u_glob->c[y];
    /* Mache Sohn konsistent: */
    make_consistent (u_glob, son, u_glob->X, u_glob->Y);
    /* Selection ist y: */
    *selection = y;
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
/* Fuer das Ueberdeckungsproblem entspricht diese Funktion der
 * Erzeugung des ersten Sohnes. Die Ecke wird jedoch nicht ausgewaehlt,
 * sondern in 'selection' uebergeben. Um fuer den naechsten Aufruf zu
 * vermerken, dass alle Soehne bereits erzeugt worden sind, wird
 * 'selection' auf -1 gesetzt.
 */

boolean	u_next_child (u_glob, parent, son, selection)

u_glob_t	*u_glob;
u_node_t	*parent;
int		*selection;
u_node_t	*son;

   {
    /* Wenn schon 2 Soehne erzeugt: */
    if (*selection == -1)
	return (OK_FALSE);

    T("u_next_son")
    TV ("Selected for B", *selection)

    /* Kopiere Vater auf Sohn: */
    u_copy_node (u_glob, parent, son);
    /* Bilde (A, B + selection) im Sohn: */
    add_elem (*selection, son->B, son->B);
    /* Mache Sohn konsistent: */
    make_consistent (u_glob, son, u_glob->X, u_glob->Y);
    /* Selection zeigt an, dass schon 2 Soehne: */
    *selection = -1;
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
/* Fuer das Ueberdeckungsproblem ist hier keine Berechnung noetig. Es
 * wird lediglich ueberprueft, ob es sich tatsaechlich um einen
 * zulaessigen Knoten handelt. Der Loesungswert wird separat
 * zurueckgegeben.
 */

int	u_solution (u_glob, node, value)

u_glob_t	*u_glob;
u_node_t	*node;
u_zf_t		*value;

   {
    if (!is_equal (node->succA, u_glob->X))
	return (U_INCOMPLETE_SOL);
    *value = node->fixed_cost;
    return (OK);
   }

#ifdef REAL_HEURISTIC

/*-------------------------- COMPARE_VAL ------------------------*/
/* Dient als Vergleichsfunktion zum Sortieren einer Liste von Ecken
 * mit Hilfe von qsort(). Liefert -1, 0, 1 je nach Ergebnis des 
 * Vergleichs der zwei Ecken. Um Zugriff auf u_glob zu erhalten,
 * erhaelt die Funktion beim ersten Aufruf zwei Zeiger auf u_glob
 * uebergeben (Dirty trick...)
 * Diese Funktion wird nur von qsort() in u_heur_loes() aufgerufen.
 */

static int	compare_val (p, q)

int	*p;
int	*q;

   {
    static u_glob_t	*u_glob;

    /* Beim ersten Aufruf u_glob merken: */
    if (!u_glob)
       {
        u_glob = (u_glob_t *) p;
	return (0);
       }

    /* Wenn hoehere Kosten: */
    if (u_glob->c[*p] < u_glob->c[*q])
	return (-1);
    else if (u_glob->c[*p] > u_glob->c[*q])
	return (1);
    /* Sonst wenn hoehere Anzahl ueberdeckter Ecken: */
    else if (nr_of_el (u_glob->succ[*p]) > nr_of_el (u_glob->succ[*q]))
	return (-1);
    else if (nr_of_el (u_glob->succ[*p]) < nr_of_el (u_glob->succ[*q]))
	return (1);
    else 
	return (0);
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
/* Fuer das Ueberdeckungsproblem wurde eine sehr einfache Heuristik
 * implementiert. Zunaechst wird die Liste der freioen Ecken durch
 * Differenzbildung erzeugt. Dann werden diese freien Ecken nach Wert
 * und Anzahl ueberdeckter Ecken sortiert. Aus der sortierten Liste
 * werden solange Ecken ausgewaehlt, bis alle Clients ueberdeckt sind.
 */

int	u_heur_loes (u_glob, node, solution, value)

u_glob_t	*u_glob;
u_node_t	*node;
u_node_t	*solution;
u_zf_t		*value;

   {
    static set_t	*Y_A_B;
    static int		*free;
    int			y;
    int			i;

    /* Beim ersten Mal Y_A_B und free allokieren: */
    if (!Y_A_B)
       {
	if (!create_set (node->A->start, node->A->size, &Y_A_B))
	    return (U_MALLOC);
	free = (int *) malloc (node->A->size * sizeof (int));
	if (!free)
	    return (U_MALLOC);
	/* Dirty trick: u_glob an compare_val() uebergeben: */
	compare_val ((int *) u_glob, (int *) u_glob);
       }

    T ("U_HEUR_LOES");

    /* Y - A - B bestimmen: */
    minus (u_glob->Y, node->A, Y_A_B);
    minus (Y_A_B, node->B, Y_A_B);

    TS ("A", node->A);
    TS ("Y-A-B", Y_A_B);

    /* Kopiere Knoten auf Loesung: */
    u_copy_node (u_glob, node, solution);

    /* Baue Liste freier Ecken auf: */
    for (i = 0, y = get_first (Y_A_B); y != -1; y = get_next (Y_A_B))
        free[i++] = y;
    /* Sortiere Ecken nach Wert (und Anzahl ueberdeckter) */
    qsort ((void *) free, (size_t) i, sizeof (int), compare_val);

    T ("Freie Ecken");
    TA (free, nr_of_el (Y_A_B));

    i = 0;
    /* Bis zulaessige Loesung entstanden: */
    while (!is_equal (solution->succA, u_glob->X))
       {
        /* Nimm naechstbillige Ecke y: */
        y = free[i++];
	/* Fuege diese zur Loesung hinzu: */
	add_elem (y, solution->A, solution->A);
	/* Aktualisiere succ(A) in der Loesung: */
	union_set (solution->succA, u_glob->succ[y], solution->succA);
	/* Addiere c(y) zu fixed_cost: */
	solution->fixed_cost += u_glob->c[y];
       }
    *value = solution->fixed_cost;
    solution->l_bound = *value;
    solution->depth = 0;

    TS ("Loesung", solution->A);

    return (OK);
   }

#else

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
/* Diese Variante wird als Dummy verwendet, wenn ueberhaupt keine
 * Heuristik erwuenscht ist. (Auswahl ueber REAL_HEURISTIC)
 */

int	u_heur_loes (u_glob, node, solution, value)

u_glob_t	*u_glob;
u_node_t	*node;
u_node_t	*solution;
u_zf_t		*value;

   {
    u_glob = u_glob;
    node = node;
    *value = MAXINT;
    solution->fixed_cost = MAXINT;
    solution->l_bound = MAXINT;
    solution->depth = 0;
    return (OK);
   }

#endif

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
/* Fuer das Ueberdeckungsproblem wird eine Kantenliste aus einer Datei
 * gelesen. Der Dateiname wird dem Parameter 'kl' aus der Kommandozeile
 * entnommen. Das Einlesen der Kantenliste wird durch die Funktion
 * kl_input() aus 'read.c' uebernommen. In u_glob werden die Mengen
 * pred und succ fuer jede Ecke angelegt, ausserdem die Liste der
 * Bewertungen c. Zwei Mengen X und Y werden ebenfalls (fuer
 * Vergleiche) angelegt. Diese Mengen stellen die Mengen der Server-
 * und Client-Ecken dar.
 */

int	u_init_glob (u_glob)

u_glob_t	**u_glob;

   {
    int		sts;
    char	*filename;
    FILE	*indat;
    int		i;

    /* Platz fuer Datenstruktur u_glob allokieren: */
    *u_glob = (u_glob_t *) malloc (sizeof (u_glob_t));
    if (!*u_glob)
	return (U_MALLOC);

    /* Dateinamen fuer Matrix aus Parameterliste holen: */
    if (!read_param ("kl", &filename))
       {
	fprintf (stderr, "u_init_glob (): Parameter 'kl' fehlt\n");
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

    /* Kantenliste aus Datei lesen: */
    sts = kl_input (indat, &(*u_glob)->Xsize, &(*u_glob)->Ysize,
		    &(*u_glob)->c, &(*u_glob)->pred, &(*u_glob)->succ);
    R_ST

    fclose (indat);

    /* X und Y anlegen und fuellen: */
    if (!create_set (0, (*u_glob)->Xsize, &(*u_glob)->X))
	return (U_MALLOC);
    if (!create_set (0, (*u_glob)->Ysize, &(*u_glob)->Y))
	return (U_MALLOC);
    for (i = 0; i < (*u_glob)->Ysize; i++)
	add_elem (i, (*u_glob)->Y, (*u_glob)->Y);
    for (i = 0; i < (*u_glob)->Xsize; i++)
	add_elem (i, (*u_glob)->X, (*u_glob)->X);

#ifdef TRACE_ON
    fprintf (stderr, "U-GLOB:\n");
    TS ("X", (*u_glob)->X);
    TS ("Y", (*u_glob)->Y);
    T ("c: ");
    TA ((*u_glob)->c, (*u_glob)->Ysize);
    for (i = 0; i < (*u_glob)->Xsize; i++)
       {
	fprintf (stderr, "pred[%1d]: ", i);
	TS ("", (*u_glob)->pred[i]);
       } /* end for */
    for (i = 0; i < (*u_glob)->Ysize; i++)
       {
	fprintf (stderr, "succ[%1d]: ", i);
	TS ("", (*u_glob)->succ[i]);
       } /* end for */
#endif

    return (OK);
   }

/*^*/
/*------------------------ U_IS_FEASIBLE ------------------------*/
/* Stellt fest, ob der Knoten 'node' eine zulaessige Loesung des 
 * Ausgangsproblems repraesentiert. Von solchen Knoten koennen 
 * keine Soehne mehr erzeugt werden. Der Rueckgabewert ist OK_TRUE, 
 * wenn 'node' eine zulaessige Loesung enthaelt, sonst OK_FALSE. 
 */
/* Fuer das Ueberdeckungsproblem gilt, dass solche Knoten zulaessig
 * sind, bei denen die Menge succA der ueberdeckten Clients gleich der
 * Menge der Clients X ist.
 */

boolean	u_is_feasible (u_glob, node)

u_glob_t	*u_glob;
u_node_t	*node;

   {
    /* Wenn succ(A) == X): */
    if (is_equal (node->succA, u_glob->X))
	return (OK_TRUE);
    else
	return (OK_FALSE);
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
/* Fuer das Ueberdeckungsproblem wird zunaechst die Menge der freien
 * Ecken bestimmt. Daraus wird diejenige mit dem gerinsten Wert
 * ausgewaehlt. Dieser Wert wird zu den Kosten der bereits fixierten
 * Knoten addiert. Ist der Schrankenwert einmal berechnet, wird er im
 * Knoten gespeichert und bei nachfolgenden Aufrufen direkt
 * zurueckgegeben.
 */

int	u_lower_bound (u_glob, node, l_bound)

u_glob_t	*u_glob;
u_node_t	*node;
u_zf_t		*l_bound;

   {
    static set_t	*Y_A_B;
    int			minvalue;
    int			min;
    int			y;

    /* Beim ersten Mal Y_A_B allokieren: */
    if (!Y_A_B)
       {
	if (!create_set (node->A->start, node->A->size, &Y_A_B))
	    return (U_MALLOC);
       }

    /* Wenn Bound bereits bekannt, direkt zurueckgeben: */
    if (node->l_bound)
       {
	*l_bound = node->l_bound;
	return (OK);
       }

    T("u_lower_bound")

    /* Y - A - B bestimmen: */
    minus (u_glob->Y, node->A, Y_A_B);
    minus (Y_A_B, node->B, Y_A_B);

    TS ("Y-A-B", Y_A_B);

    /* Waehle kleinstes y aus Y - A - B: */
    /* Setze Minimum auf MAXINT */
    minvalue = MAXINT;
    min = get_first (Y_A_B);
    /* Fuer alle y aus Y - A - B: */
    for (y = get_first (Y_A_B); y != -1; y = get_next (Y_A_B))
       {
	if (u_glob->c[y] < minvalue)
	   {
	    min = y;
	    minvalue = u_glob->c[y];
	   }
       }

    TV ("Kleinstes y in Y-A-B:", min)
    /* Schrankenwert ist fixed_cost + c (kleinstes y): */
    node->l_bound = node->fixed_cost + minvalue;
    *l_bound = node->l_bound;
    TV ("Calculated L-Bound", *l_bound)
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
    else
	*priority = (node->depth - 1) * para * l_bound
		    / (u_glob->Ysize - 1)
		    - 100 * l_bound;
    return (OK);
   }

/*^*/
/*------------------------ U_PRINT_SOL -------------------------*/
/* Druckt die Loesung 'sol' auf der Standardausgabe aus.
 */
/* Fuer das Ueberdeckungsproblem wird der Inhalt der Menge A als
 * Loesung ausgegeben. 
 */

int	u_print_sol (u_glob, sol)

u_glob_t	*u_glob;
u_node_t	*sol;

   {
    print_set (stdout, sol->A);
    printf ("F = %d\n", sol->fixed_cost);
    return (OK);
    u_glob = u_glob;
   }

/*------------------------ MAKE_CONSISTENT ---------------------*/
/* Bearbeitet beim Ueberdeckungsproblem einen Knoten mit zwei
 * Dominanzregeln. Zunaechst wird die Menge der freien Ecken bestimmt.
 * Alle Ecken daraus, die nichts neues zur Menge der schon ueberdeckten
 * Ecken beitragen koennen, werden nach B gelegt. (Kriterium 1).
 * Ausserdem werden alle unbedeckten Ecken aus X darauf untersucht, ob
 * es fuer sie nur noch eine freie ueberdeckende Ecke gibt. Ist dies
 * der Fall, wird die ueberdeckende Ecke in A aufgenommen. (Kriterium 2)
 * Danach wird noch einmal Kriterium 1 angewendet, um evtl. neu
 * hinzugekommene Ecken in A mit zu erfassen.
 */

static int	make_consistent (u_glob, node, X, Y)

u_glob_t	*u_glob;
u_node_t	*node;
set_t		*X;
set_t		*Y;

   {
    static set_t	*Y_A_B;
    static set_t	*X_succA;
    static set_t	*predx_B;
    int			y;
    int			x;

    T ("make_consistent:")

    /* Beim ersten Mal Y_A_B, X_succA und predx_B allokieren: */
    if (!Y_A_B)
       {
	if (!create_set (node->A->start, node->A->size, &Y_A_B))
	    return (U_MALLOC);
	if (!create_set (node->succA->start, node->succA->size, 
			 &X_succA))
	    return (U_MALLOC);
	if (!create_set (node->A->start, node->A->size, &predx_B))
	    return (U_MALLOC);
       }

    /* Y - A - B bestimmen: */
    minus (Y, node->A, Y_A_B);
    minus (Y_A_B, node->B, Y_A_B);

    TS ("Y-A-B", Y_A_B);
    TS ("succA", node->succA);

    /* Fuer alle y in Y - A - B: */
    for (y = get_first (Y_A_B); y != -1; y = get_next (Y_A_B))
       {
	/* Wenn succ(y) Teilmenge von succ(A): */
	if (is_subset (u_glob->succ[y], node->succA))
	   {
	    /* Fuege y zu B hinzu: */
	    add_elem (y, node->B, node->B);
	    TV ("added to B", y)
	   }
       }

    /* Berechne unbedeckte x: X - succ(A): */
    minus (X, node->succA, X_succA);

    TS ("X-succA", X_succA);

    /* Fuer alle unbedeckten x: */
    for (x = get_first (X_succA); x != -1; x = get_next (X_succA))
       {
	/* Berechne pred(x) - B fuer dieses x: */
	minus (u_glob->pred[x], node->B, predx_B);
	/* Wenn nur noch eine Chance: pred(x) - B hat nur 1 Element: */

	/* TV ("x", x) */
	/* TS ("predX-B", predx_B); */

	if (nr_of_el (predx_B) <= 1)
	   {
	    /* Bestimme dieses Element: */
	    y = get_first (predx_B);
	    TV ("To save", x)
	    TV ("...last chance added to A", y)
	    /* Fuege dieses Element zu A hinzu: */
	    add_elem (y, node->A, node->A);
	    /* Aktualisiere succ(A): */
	    union_set (node->succA, u_glob->succ[y], node->succA);
	    /* Zaehle c(element) zu fixed_cost hinzu: */
	    node->fixed_cost += u_glob->c[y];
	   }
       }

    /* Y - A - B bestimmen: (once again) */
    minus (Y, node->A, Y_A_B);
    minus (Y_A_B, node->B, Y_A_B);

    TS ("succA (2nd)", node->succA);
    TS ("Y-A-B (2nd)", Y_A_B);

    /* Fuer alle y in Y - A - B: (once again) */
    for (y = get_first (Y_A_B); y != -1; y = get_next (Y_A_B))
       {
	/* Wenn succ(y) Teilmenge von succ(A): */
	if (is_subset (u_glob->succ[y], node->succA))
	   {
	    /* Fuege y zu B hinzu: */
	    add_elem (y, node->B, node->B);
	    TV ("added to B", y)
	   }
       }
   }

/*------------------------ U_CREATE_NODE -----------------------*/
/* Erzeugt 'anzahl' u_nodes der Groesse 'size' und legt Zeiger auf 
 * sie in dem Feld 'nodes' ab. Dort muss mindestens Platz fuer 
 * 'anzahl' Zeiger sein. Rueckgabewert gibt die Zahl der erzeugten 
 * Knoten an. 
 */
/* Beim Ueberdeckungsproblem werden die Knoten erzeugt und jeweils mit
 * den notwendigen Mengen versehen, die durch das 'set'-Modul erzeugt
 * werden.
 */

int	u_create_node (u_glob, anzahl, nodes)

u_glob_t	*u_glob;
int		anzahl;
u_node_t	**nodes;

   {
    int		i;

    /* Platz fuer 'anzahl' Knoten allokieren: */
    nodes[0] = (u_node_t *) malloc (anzahl * sizeof (u_node_t));
    if (!nodes[0])
	return (0);
    
    for (i = 0; i < anzahl; i++)
       {
	/* Knoten auf die Liste verteilen: */
	nodes[i] = ((u_node_t *) nodes[0]) + i;
	/* Jeder Knoten benoetigt 3 Mengen: */
	if (!create_set (0, u_glob->Ysize, &nodes[i]->A))
	    return (0);
	if (!create_set (0, u_glob->Ysize, &nodes[i]->B))
	    return (0);
	if (!create_set (0, u_glob->Xsize, &nodes[i]->succA))
	    return (0);
       } /* end for */
    return (anzahl);
   }

/*^*/
/*------------------------ U_COPY_NODE -------------------------*/
/* Kopiert den Inhalt eines u_node mit allen evtl. daranhaengenden 
 * verzeigerten Strukturen auf einen anderen u_node. 'size' gibt
 * Groesse des dargestellten Problems an.
 */
/* Beim Ueberdeckungsproblem werden die enthaltenen Mengen durch 
 * set-Funktionen kopiert.
 */

void	u_copy_node (u_glob, from, to)

u_glob_t	*u_glob;
u_node_t	*from;
u_node_t	*to;

   {
    to->fixed_cost = from->fixed_cost;
    to->l_bound = from->l_bound;
    to->depth = from->depth;
    /* Mengen extra kopieren: */
    copy_set (from->A, to->A);
    copy_set (from->B, to->B);
    copy_set (from->succA, to->succA);
    return;
    u_glob = u_glob;
   }

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
/* Beim Ueberdeckungsproblem werden die enthaltenen Mengen
 * nacheinander durch set-Funktionen in eine flache Struktur kopiert.
 */

void	u_flat_node (u_glob, node, flat)

u_glob_t	*u_glob;
u_node_t	*node;
char		*flat;

   {
    int		*intbuf;
    char	*charbuf;

    intbuf = (int *) flat;
    charbuf = (char *)(intbuf + 3);

    intbuf[0] = node->fixed_cost;
    intbuf[1] = node->l_bound;
    intbuf[2] = node->depth;
    flat_set (node->A, charbuf);
    flat_set (node->B, charbuf + sizeof_flat_set (node->A));
    flat_set (node->succA, charbuf + 2 * sizeof_flat_set (node->A));
    return;
    u_glob = u_glob;
   }

/*^*/
/*------------------------ U_UNFLAT_NODE -----------------------*/
/* Kopiert einen mit u_flat_node () erzeugten flachen Knoten 'flat' 
 * in einen normalen u_node 'node' zurueck. Die Problemgroesse ist 
 * 'size'. 'node' muss schon mit entsprechender Struktur vorhanden
 * sein.
 */
/* Beim Ueberdeckungsproblem werden mit Hilfe von set-Funktionen aus
 * dem flachen Block wider 3 Mengen erzeugt.
 */

void	u_unflat_node (u_glob, flat, node)

u_glob_t	*u_glob;
char		*flat;
u_node_t	*node;

   {
    int		*intbuf;
    char	*charbuf;

    intbuf = (int *) flat;
    charbuf = (char *)(intbuf + 3);

    node->fixed_cost = intbuf[0];
    node->l_bound = intbuf[1];
    node->depth = intbuf[2];
    unflat_set (charbuf, node->A);
    unflat_set (charbuf + sizeof_flat_set (node->A), node->B);
    unflat_set (charbuf + 2 * sizeof_flat_set (node->A), node->succA);
    return;
    u_glob = u_glob;
   }

/*^*/
/*------------------------ U_SIZEOF_FLAT_NODE ------------------*/
/* Bestimmt zu einer Problemgroesse 'size' die Groesse eines mit
 * u_flat_node () erzeugten flachen Knotens.
 */

int	u_sizeof_flat_node (u_glob)

u_glob_t	*u_glob;

   {
    return (3 * sizeof (int) + 2 * sizeof_flat_set (u_glob->Y)
	    + sizeof_flat_set (u_glob->X));
   }

/*^*/
/*------------------------ U_FLAT_GLOB -------------------------*/
/* Kopiert den Inhalt der Datenstruktur u_glob in einen flachen
 * Speicherbereich flat. Flat wird von der Funktion angelegt. In size
 * wird die Groesse von flat zurueckgeliefert. 
 */ 
/* Beim Ueberdeckungsproblem werden mit Hilfe von set-Funktionen
 * nacheinander alle pred- und succ-Mengen in einen flachen Block
 * kopiert. Die Integerwerte aus c und die Groessenangaben werden
 * einzeln dazukopiert.
 */

int	u_flat_glob (u_glob, flat, size)

u_glob_t	*u_glob;
char		**flat;
int		*size;

   {
    int		*intbuf;
    char	*charbuf;
    int		i;
    int		set_size;

    /* Berechne Groesse der flachen Struktur: */
    *size = (2 + u_glob->Ysize) * sizeof (int)
	    + u_glob->Ysize * sizeof_flat_set (u_glob->X)
	    + u_glob->Xsize * sizeof_flat_set (u_glob->Y);

    /* Lege Block fuer flache Struktur an: */
    intbuf = (int *) malloc (*size);
    if (!intbuf)
	return (U_MALLOC);

    /* Integerwerte aus u_glob zuerst kopieren: */
    intbuf[0] = u_glob->Xsize;
    intbuf[1] = u_glob->Ysize;
    for (i = 0; i < u_glob->Ysize; i++)
	intbuf[2 + i] = u_glob->c[i];

    /* Inhalt der Mengen in den Block kopieren: */
    /* X und Y werden nicht kopiert, sondern am Ziel rekonstruiert */
    charbuf = (char *) ((int *) intbuf + 2 + u_glob->Ysize);
    set_size = sizeof_flat_set (u_glob->X);
    for (i = 0; i < u_glob->Ysize; i++)
       {
	flat_set (u_glob->succ[i], charbuf);
	charbuf += set_size;
       } /* end for */
    set_size = sizeof_flat_set (u_glob->Y);
    for (i = 0; i < u_glob->Xsize; i++)
       {
	flat_set (u_glob->pred[i], charbuf);
	charbuf += set_size;
       } /* end for */

    *flat = (char *) intbuf;
    return (OK);
   }

/*^*/
/*------------------------ U_UNFLAT_GLOB -----------------------*/
/* Erzeugt aus dem Inhalt des flachen Puffers flat den Inhalt der
 * Datenstruktur u_glob. U_glob wird dabei angelegt. Beim Anlegen von
 * flat muss in geeigneter Weise die Groesse mitgeliefert werden. 
 */ 
/* Beim Ueberdeckungsproblem werden die pred- und succ-Mengen mit Hilfe
 * von set-Funktionen einzeln in Mengen zurueckverwandelt. Die
 * Integerwerte aus c werden direkt kopiert.
 */


int	u_unflat_glob (flat, u_glob)

char		*flat;
u_glob_t	**u_glob;

   {
    int		*intbuf;
    char	*charbuf;
    int		i;
    int		set_size;

    /* Platz fuer Datenstruktur u_glob allokieren: */
    *u_glob = (u_glob_t *) malloc (sizeof (u_glob_t));
    if (!*u_glob)
	return (U_MALLOC);

    intbuf = (int *) flat;

    /* Groessen fuer u_glob zuerst lesen: */
    (*u_glob)->Xsize = intbuf[0];
    (*u_glob)->Ysize = intbuf[1];

    /* Feld fuer Bewertungen anlegen: */
    (*u_glob)->c = (int *) malloc ((*u_glob)->Ysize * sizeof (int));
    if (!(*u_glob)->c)
	return (U_MALLOC);

    /* Bewertungen lesen: */
    for (i = 0; i < (*u_glob)->Ysize; i++)
	(*u_glob)->c[i] = intbuf[2 + i];

    /* X und Y anlegen und fuellen: */
    if (!create_set (0, (*u_glob)->Xsize, &(*u_glob)->X))
	return (U_MALLOC);
    if (!create_set (0, (*u_glob)->Ysize, &(*u_glob)->Y))
	return (U_MALLOC);
    for (i = 0; i < (*u_glob)->Ysize; i++)
	add_elem (i, (*u_glob)->Y, (*u_glob)->Y);
    for (i = 0; i < (*u_glob)->Xsize; i++)
	add_elem (i, (*u_glob)->X, (*u_glob)->X);

    /* Listen fuer pred und succ anlegen: */
    (*u_glob)->succ = (set_t **) malloc (sizeof (set_t *) 
				      * (*u_glob)->Ysize);
    if (!(*u_glob)->succ)
	return (U_MALLOC);
    (*u_glob)->pred = (set_t **) malloc (sizeof (set_t *) 
				      * (*u_glob)->Xsize);
    if (!(*u_glob)->pred)
	return (U_MALLOC);

    /* Inhalt der Mengen lesen: */
    charbuf = (char *) (intbuf + 2 + (*u_glob)->Ysize);
    set_size = sizeof_flat_set ((*u_glob)->X);
    for (i = 0; i < (*u_glob)->Ysize; i++)
       {
	if (!create_set (0, (*u_glob)->Xsize, &(*u_glob)->succ[i]))
	    return (U_MALLOC);
	unflat_set (charbuf, (*u_glob)->succ[i]);
	charbuf += set_size;
       } /* end for */
    set_size = sizeof_flat_set ((*u_glob)->Y);
    for (i = 0; i < (*u_glob)->Xsize; i++)
       {
	if (!create_set (0, (*u_glob)->Ysize, &(*u_glob)->pred[i]))
	    return (U_MALLOC);
	unflat_set (charbuf, (*u_glob)->pred[i]);
	charbuf += set_size;
       } /* end for */
    
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
    wp += SPRINTF ((wp, "A: "));
    wp += sprint_set (wp, u_node->A);
    wp += SPRINTF ((wp, "B: "));
    wp += sprint_set (wp, u_node->B);
    wp += SPRINTF ((wp, "succA: "));
    wp += sprint_set (wp, u_node->succA);
    wp += SPRINTF ((wp, "u_node->l_bound:\t%2d\n", u_node->l_bound));
    wp += SPRINTF ((wp, "u_node->depth:\t%2d\n", u_node->depth));
   }

/*------------------------ ENDE ---------------------------------*/
