/*---------------------------------------------------------------*/
/* u_gen.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.2		1.6.93 					 */
/*---------------------------------------------------------------*/
/* Deklarationen und Funktionsruempfe als Vorlage fuer die
 * Entwicklung neuer Benutzerfunktionen
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

int	u_mk_start_node (u_glob, startnode)

u_glob_t	*u_glob;
u_node_t	*startnode;

   {
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
    return (OK);
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

int	u_init_glob (u_glob)

u_glob_t	**u_glob;

   {
    return (OK);
   }

/*^*/
/*------------------------ U_IS_FEASIBLE ------------------------*/
/* Stellt fest, ob der Knoten 'node' eine zulaessige Loesung des 
 * Ausgangsproblems repraesentiert. Von solchen Knoten koennen 
 * keine Soehne mehr erzeugt werden. Der Rueckgabewert ist OK_TRUE, 
 * wenn 'node' eine zulaessige Loesung enthaelt, sonst OK_FALSE. 
 */

boolean	u_is_feasible (u_glob, node)

u_glob_t	*u_glob;
u_node_t	*node;

   {
    return (OK_TRUE);
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

int	u_lower_bound (u_glob, node, l_bound)

u_glob_t	*u_glob;
u_node_t	*node;
u_zf_t		*l_bound;

   {
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
   }

/*------------------------ ENDE ---------------------------------*/
