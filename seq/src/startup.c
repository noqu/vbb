/*---------------------------------------------------------------*/
/* startup.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/*------------------------ PRAEPROZESSOR ------------------------*/

/* #define TRACE_ON */

#define		TRACE_CONDITION		glob.flags & TRACE_STARTUP

#include  <stdio.h>
#include  <stdlib.h>
#ifndef SUN_CC
#include <misc.h>
#endif
#include  <time.h>
#include  <string.h>
#include  "types.h"
#include  "u_types.h"
#include  "startup.h"
#include  "mem.h"
#include  "heap.h"
#include  "u_funcs.h"
#include  "expand.h"
#include  "error.h"
#include  "trace.h"
#include  "read.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

stat_t		statistic;
glob_t		glob;

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

static void	print_stat ();
static int	init_glob ();

#else

static void	print_stat (
			int	/* ring_nr */,
			stat_t	/* stat */);

static int	init_glob (
			glob_t*	/* glob */);

#endif

/*^*/
/*------------------------ MAIN ---------------------------------*/

int	main (ac, av)

int	ac;
char	**av;

   {
    u_glob_t		*u_glob;
    node_t		*startnode;
    int			sts;
    node_t		*solution;
    char		*flat_u_glob;
    int			flat_u_size;
    char		*param_buf;
    int			timer_start;

    /* Initialisiere Statistik-Struktur: */
    memset ((char *) &statistic, 0, sizeof (stat_t));

    /* Lese Parameter aus Kommandozeile u. evtl. Datei: */
    sts = init_params (&param_buf, ac, av);
    P_ST
    /* Setze globale Kernel-Informationen: */
    sts = init_glob (&glob);
    P_ST
    /* Lese globale Daten fuer Benutzerfunktionen: */
    sts = u_init_glob (&u_glob);
    P_ST

    /* Starte die Uhr: */
    timer_start = (int) clock ();

#ifndef NOT_DEFINED
    /* Klopfe u_glob flach: */
    sts = u_flat_glob (u_glob, &flat_u_glob, &flat_u_size);
    P_ST

    /* Rekonstruiere u_glob aus flacher Struktur: */
    sts = u_unflat_glob (flat_u_glob, &u_glob);
    P_ST

    /* Flache u_glob kann weg: */
    free (flat_u_glob);
#endif

    /* Erzeuge Anfangsknoten fuer das Ausgangsproblem: */
    sts = m_get_node (u_glob, &startnode);
    P_ST
    sts = u_mk_start_node (u_glob, startnode->u_node);
    P_ST

    /* Erzeuge Anfangs-Incumbent: */
    sts = m_get_node (u_glob, &solution);
    P_ST
    solution->value = MAXINT;

    /* Anfangsknoten muss mit Heuristik bearbeitet werden: */
    sts = u_heur_loes (u_glob, startnode->u_node, 
		       solution->u_node, &solution->value);
    P_ST

    /* Initialisiere den Heap: */
    h_init_heap ();

    /* Lege Anfangsknoten auf Heap: */
    h_put_on_heap (startnode);
    statistic.nodes++;
    TKN ("Startknoten", startnode);

    /* Trage Anfangsincumbent ein: */
    h_init_incumbent (solution);

    /* Starte Expansion von Knoten: */
    sts = expander (u_glob);
    P_ST

    /* Uhr anhalten: (Rest zaehlt nicht zum Algorithmus) */
    statistic.time = ((int) clock ()) - timer_start;

    if (glob.flags & STATISTIC)
	print_stat (0, statistic);

    /* Gib Zeit aus: */
    fprintf (stderr, "TIME: %d\n", statistic.time);

    if (!(glob.flags & NO_SOLUTION))
       {
	/* Loesung ausgeben: */
	solution = h_read_incumbent ();
	sts = u_print_sol (u_glob, solution->u_node);
	P_ST
       }
   }

/*------------------------ PRINT_STAT --------------------------*/

static void	print_stat (ring_nr, stat)

int	ring_nr;
stat_t	stat;

   {
    int		i;
    int		j;
    int		scale;

    if (ring_nr == 0)
       {
               /*        6        8       7       7    4   3   3 */  
	printf ("NR   exp.     erz. el(bd). el(he). heu. zl. im. ");
               /*    5      6      6      6  */                        
	printf (" all.  lifo.   sent   idle\n");
       }

    if (ring_nr == -1)
	printf ("SU ");
    else
	printf ("%2d ", ring_nr);

    printf ("%6d %8d %7d %7d %4d %3d %3d %5d %6d %6d %6d\n",
	    stat.expanded,
	    stat.nodes,
	    stat.elim_by_ub + stat.elim_by_lb,
	    stat.elim_from_heap,
	    stat.heur_solved,
	    stat.feasible,
	    stat.improved,
	    stat.allocated,
	    stat.from_lifo,
	    stat.sent,
	    stat.idle);

    /* Wenn Profil von L-Bounds gewuenscht: */
    if ((glob.flags & PROFILE) && ring_nr != -1)
       {
	/* Summiere die hoeheren Klassen auf, wenn weniger zu zeigen:*/
	for (i = glob.prof_classes; i < PROF_BUF_SIZE; i++)
	    statistic.profile[glob.prof_classes - 1] 
		+= statistic.profile[i];

	/* Skaliere fuer Bildschirmausgabe: */
	/* prof_scale ist "Anzahl Punkte im Bild insgesamt" */
	/* scale ist "Knoten pro Bildpunkt" */
	scale = statistic.expanded / glob.prof_scale;
	if (scale == 0)
	    scale = 1;

	/* Gib gewuenschte Klassen graphisch aus: */
	for (i = 0; i < glob.prof_classes; i++)
	   {
	    printf ("%4d: ", glob.prof_start + i * glob.prof_unit);
	    for (j = 0; j < statistic.profile[i] / scale; j++)
		printf ("#");
	    printf (" (%d)\n", statistic.profile[i]);
	   }
       }
   }

/*------------------------ INIT_GLOB ---------------------------*/
/*  */

static int	init_glob (glob)

glob_t		*glob;

   {
    char	*parvalue;

    glob->flags = 0;
    if (read_param ("TRACE_EXP", &parvalue))
	glob->flags |= TRACE_EXP;
    if (read_param ("TRACE_HEAP", &parvalue))
	glob->flags |= TRACE_HEAP;
    if (read_param ("TRACE_MEM", &parvalue))
	glob->flags |= TRACE_MEM;
    if (read_param ("TRACE_STARTUP", &parvalue))
	glob->flags |= TRACE_STARTUP;
    if (read_param ("ELIM_BY_HEUR", &parvalue))
	glob->flags |= ELIM_BY_HEUR;
    if (read_param ("START_HEUR", &parvalue))
	glob->flags |= START_HEUR;

    if (read_param ("PRIO_PAR", &parvalue))
	glob->prio_par = atoi (parvalue);
    else
	glob->prio_par = DEF_PRIO_PAR;

    if (read_param ("STATISTIC", &parvalue))
	glob->flags |= STATISTIC;

    if (read_param ("NO_SOLUTION", &parvalue))
	glob->flags |= NO_SOLUTION;

    if (read_param ("MAX_HEAP_SIZE", &parvalue))
	glob->max_heap_size = atoi (parvalue);
    else
	glob->max_heap_size = DEF_MAX_HEAP_SIZE;

    if (read_param ("MIN_HEAP_SIZE", &parvalue))
	glob->min_heap_size = atoi (parvalue);
    else
	glob->min_heap_size = DEF_MIN_HEAP_SIZE;

    if (read_param ("RESERVE_SIZE", &parvalue))
	glob->reserve_size = atoi (parvalue);
    else
	glob->reserve_size = DEF_RESERVE_SIZE;

    if (read_param ("PROFILE", &parvalue))
       {
	glob->flags |= PROFILE;

	if (read_param ("PROF_START", &parvalue))
	    glob->prof_start = atoi (parvalue);
	else
	    glob->prof_start = DEF_PROF_START;

	if (read_param ("PROF_UNIT", &parvalue))
	    glob->prof_unit = atoi (parvalue);
	else
	    glob->prof_unit = DEF_PROF_UNIT;

	/* Anzahl Profilklassen aus Parameter lesen (oder Default) */
	if (read_param ("PROF_CLASSES", &parvalue))
	   {
	    glob->prof_classes = atoi (parvalue);
	    /* Klassen fuer Unter/Ueberschreitung dazu: */
	    glob->prof_classes += 2;
	   }
	else
	    glob->prof_classes = PROF_BUF_SIZE;

	/* Hoechstens so viele Klassen wie gesampled wurden: */
	if (glob->prof_classes > PROF_BUF_SIZE)
	    glob->prof_classes = PROF_BUF_SIZE;

	/* Skalierungsfaktor fuer grafische Darstellung lesen: */
	if (read_param ("PROF_SCALE", &parvalue))
	    glob->prof_scale = atoi (parvalue);
	else
	    glob->prof_scale = DEF_PROF_SCALE;
       }
    return (OK);
   }

/*------------------------ PRINT_ERROR -------------------------*/

int	print_error (p, sts)

int		p; 
int		sts; 
 
   { 
    fprintf (stderr, "\n*** FEHLER\n*** Prozessor %d: ", p);
    if (sts >= U_FUNCS_ERR)
	fprintf (stderr, "Fehler in Benutzermodul: %d\n", sts);
    else
	switch (sts)
	   {
	    case M_INIT_MEM:
		fprintf (stderr, "Zu wenig Speicher fuer Knoten\n");
		break;
	    case READ_ERR:
		fprintf (stderr, "Fehler beim Lesen von Eingaben\n");
		break;
	    default:
		fprintf (stderr, "Unbekannter Fehler: %d\n", sts);
		break;
	   }
    fprintf (stderr, "*** exiting...\n");
   }

#ifdef SUN_CC
exit_terminate ()
   {
#ifdef VBB_ORIGINAL
    exit ();
#else
    exit (0);
#endif
   }
#endif

/*------------------------ ENDE ---------------------------------*/
