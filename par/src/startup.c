/*---------------------------------------------------------------*/
/* startup.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		1.6.93  				 */
/* Version 4.2							 */
/*---------------------------------------------------------------*/
/* Startroutine des vbb-Kernels. Eigener Prozess auf allen Prozessoren
 * Legt Queues an, liest Eingaben, initialisiert alles und
 * startet alle anderen Prozesse. Fungiert dann als Trace-Server,
 * bis allgemeines Ende durch Fehler oder Erfolg eintritt. Beendet
 * alle Prozesse, die dies nicht selbst tun.
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

/* TRACE_ON muss hier definiert sein, wenn irgendein Modul 
 * die Tracing-Makros verwenden soll:
 */

/* #define		TRACE_ON */

#define		TRACE_CONDITION		glob.flags & TRACE_STARTUP

#include  <stdio.h>
#include  <stdlib.h>
#include  <process.h>
#include  <channel.h>
#include  <misc.h>
#include  <time.h>
#include  <string.h>
#include  "types.h"
#include  "u_types.h"
#include  "queue.h"
#include  "startup.h"
#include  "mem.h"
#include  "disp.h"
#include  "heap.h"
#include  "u_funcs.h"
#include  "expand.h"
#include  "error.h"
#include  "trace.h"
#include  "inout.h"
#include  "read.h"

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

stat_t		statistic;
int		m_finished;
int		d_finished;
int		e_finished;
int		in_finished[4];
int		out_finished[4];
int		parallel;
glob_t		glob;

/*------------------------ MAKROS -------------------------------*/
/*------------------------ FUNKTIONEN ---------------------------*/

static void	print_stat (
			int	/* cpu_nr */,
			stat_t	/* stat */);

static int	init_glob (
			glob_t*	/* glob */);

/*^*/
/*------------------------ MAIN ---------------------------------*/

int	main (ac, av)

int	ac;
char	**av;

   {
    u_glob_t		*u_glob;
    node_t		*startnode;
    int			sts;
    int			type;
    node_t		*solution;
    Process		*proc_memserver;
    Process		*proc_dispatcher;
    Process		*proc_expander;
    static queue_t	*m_inqs[11];
    static queue_t	*m_outqs[10];
    static queue_t	*out_to_mem[5];
    static queue_t	*disp_to_out[5];
    static queue_t	*in_to_disp[5];
    static queue_t	*mem_to_in[5];
    static int		link_connected[5];
    queue_t		*mem_to_exp;
    queue_t		*disp_to_mem;
    queue_t		*disp_to_exp;
    queue_t		*exp_to_disp;
    int			mi = 0;
    int			mo = 0;
    static Channel	**in_channel;
    static Channel	**out_channel;
    int			nr_of_chans;
    static Process	*proc_in[4];
    static Process	*proc_out[4];
    int			i;
    static queue_t	*tracer_qs[NR_OF_PROCESSES];
    int			tt = 0;
    queue_t		*to_tracer;
    Channel		*u_glob_chan;
    char		*flat_u_glob;
    int			flat_u_size;
    int			cpu_nr;
    int			nr_of_cpus;
    char		*param_buf;
    stat_t		stat_buffer;
    static stat_t	stat_sums;
    int			nr_of_stats;
    int			timer_start;
    int			last_in_ring;
    int			ring_in;
    int			ring_out;
#ifdef TRACE_ON
    trace_t		*trace_buf;
    int			served;
#endif

    /* Hole Parameter aus icconf-Konfigurationsdatei: */
    cpu_nr = *((int *) get_param (3));
    in_channel = get_param (4);
    out_channel = get_param (5);
    nr_of_chans = *((int *) get_param (6));
    nr_of_cpus = *((int *) get_param (7));
    last_in_ring = *((int *) get_param (8));

    /* Initialisiere Statistik-Struktur: */
    memset ((char *) &statistic, 0, sizeof (stat_t));
    statistic.cpu_nr = cpu_nr;

    /* Stelle fest, ob ueberhaupt paralleler Betrieb stattfindet: */
    if (nr_of_cpus == 1)
	parallel = OK_FALSE;
    else
	parallel = OK_TRUE;

    /* Sonderfall 2 Transputer: Ring ueber 0 <-> 0: */
    ring_out = 0;
    if (nr_of_cpus == 2)
	ring_in = 0;
    else
	ring_in = 1;

    /* Wenn ROOT-Transputer: */
    if (!cpu_nr)
       {
	/* Lese Parameter aus Kommandozeile u. evtl. Datei: */
	sts = init_params (&param_buf, ac, av);
	P_ST
	/* Setze globale Kernel-Informationen: */
	sts = init_glob (&glob);
	P_ST
	/* Lese globale Daten fuer Benutzerfunktionen: */
	sts = u_init_glob (&u_glob);
	P_ST
       }

    /* Starte die Uhr: */
    timer_start = (int) clock ();

    /* Konfiguration der Links des Transputers: */
    for (i = 0; i < nr_of_chans; i++)
	link_connected[i] = OK_TRUE;

    /* Erzeuge Queues fuer Tracing: */
    for (i = 0; i < NR_OF_PROCESSES; i++)
       {
	sts = q_make_queue (TRACER_Q_SIZE, &tracer_qs[i]);
	P_ST
       }

    /* Eigener Anschluss an Tracer: */
    to_tracer = tracer_qs[tt++];

    /*---------------------------------------*/
    /* ERST AB HIER FUNKTIONIERT TRACING !!! */
    /*---------------------------------------*/

    /* Erzeuge Queues fuer Memserver: */
    sts = q_make_queue (MEM_OUT_Q_SIZE, &mem_to_exp);
    P_ST
    sts = q_make_queue (MEM_IN_Q_SIZE, &disp_to_mem);
    P_ST

    /* Erzeuge Queues fuer Dispatcher und Expander: */
    sts = q_make_queue (EXP_TO_DISP_Q_SIZE, &exp_to_disp);
    P_ST
    sts = q_make_queue (DISP_TO_EXP_Q_SIZE, &disp_to_exp);
    P_ST

    /* Erzeuge Queues fuer In und Out: */
    for (i = 0; i < 4; i++) 
       {
	/* Nur wenn Link tatsaechlich benutzt wird: */
	if (link_connected[i])
	   {
	    /* Queue von Out zum Memserver: */
	    sts = q_make_queue (MEM_IN_Q_SIZE, &out_to_mem[i]);
	    P_ST
	    /* Registrieren als Memserver-Eingangsqueue */
	    m_inqs[mi++] = out_to_mem[i];

	    /* Queue vom Dispatcher zu Out: */
	    sts = q_make_queue (DISP_TO_OUT_Q_SIZE, &disp_to_out[i]);
	    P_ST

	    /* Queue von In zu Dispatcher: */
	    sts = q_make_queue (IN_TO_DISP_Q_SIZE, &in_to_disp[i]);
	    P_ST

	    /* Queue von Memserver zu In: */
	    sts = q_make_queue (MEM_OUT_Q_SIZE, &mem_to_in[i]);
	    P_ST
	    /* Registrieren als Memserver-Ausgangsqueue: */
	    m_outqs[mo++] = mem_to_in[i];
	   }
       }
    
    /* Restliche Memserver-Ein/Ausgangsqueues registrieren: */
    m_inqs[mi++] = disp_to_mem;
    m_outqs[mo++] = mem_to_exp;
    m_inqs[mi] = 0;
    m_outqs[mo] = 0;

    /* Wenn ROOT-Transputer: */
    if (!cpu_nr)
       {
	if (parallel)
	   {
	    u_glob_chan = out_channel[ring_out];

	    /* Verschicke globale Kernel-Daten: */
	    ChanOut (u_glob_chan, (void *) &glob, sizeof (glob_t));

	    /* Klopfe u_glob flach: */
	    sts = u_flat_glob (u_glob, &flat_u_glob, &flat_u_size);
	    P_ST

	    /* Verschicke flache u_glob: */
	    ChanOutInt (u_glob_chan, flat_u_size);
	    ChanOut (u_glob_chan, flat_u_glob, flat_u_size);
	    TV ("U-glob verschickt", flat_u_size)

	    /* Flache u_glob kann weg: */
	    free (flat_u_glob);
	   }
       }
    else /* SLAVE-Transputer */
       {
	u_glob_chan = in_channel[ring_in];

	/* Lies globale Kernel-Daten: */
	ChanIn (u_glob_chan, (void *) &glob, sizeof (glob_t));

	/* Lies Groesse der flachen u_glob: */
	flat_u_size = ChanInInt (u_glob_chan);

	/* Allokiere Platz fuer flache u_glob: */
	flat_u_glob = (char *) malloc (flat_u_size);
	if (!flat_u_glob)
	    FEHLER (S_PROC_ALLOC);

	/* Lese flache u_glob von Kanal: */
	ChanIn (u_glob_chan, flat_u_glob, flat_u_size);
	TV ("U-glob empfangen", flat_u_size)

	/* Wenn nicht der letzte Transputer im Ring: */
	if (cpu_nr != last_in_ring)
	   {
	    u_glob_chan = out_channel[ring_out];
	    /* Schicke u_glob weiter: */
	    ChanOut (u_glob_chan, (void *) &glob, sizeof (glob_t));
	    ChanOutInt (u_glob_chan, flat_u_size);
	    ChanOut (u_glob_chan, flat_u_glob, flat_u_size);
	   }

	/* Rekonstruiere u_glob aus flacher Struktur: */
	sts = u_unflat_glob (flat_u_glob, &u_glob);
	if (sts > ERROR)
	    q_write (exp_to_disp, Q_ERROR, (void *) sts);

	/* Flache u_glob kann weg: */
	free (flat_u_glob);
       }

    /* Wg. Abhaengigkeiten erstmal alle Prozesse als lebend annehmen: */
    d_finished = OK_FALSE;
    e_finished = OK_FALSE;
    m_finished = OK_FALSE;
    for (i = 0; i < 4; i++)
       {
	in_finished[i] = OK_FALSE;
	out_finished[i] = OK_FALSE;
       }

    /* Starte Memserver: */
    proc_memserver = ProcAlloc (mem_server, STACK_SIZE, 6, m_inqs, 
				m_outqs, u_glob, MEM_MIN_LEVEL, 
				MEM_MAX_LEVEL,
				tracer_qs[tt++]);
    if (!proc_memserver)
       {
	m_finished = S_PROC_ALLOC;
	FEHLER (S_PROC_ALLOC)
       }
    ProcRun (proc_memserver);
    T ("Memserver started")

    /* Wenn ROOT-Transputer: */
    if (!cpu_nr && sts < ERROR)
       {
	/* Erzeuge Anfangsknoten fuer das Ausgangsproblem: */
	sts = q_read_bl (mem_to_exp, &type, (void **) &startnode, 
		         &m_finished);
	P_ST
	sts = u_mk_start_node (u_glob, startnode->u_node);
	P_ST

	/* Erzeuge Anfangs-Incumbent: */
	sts = q_read_bl (mem_to_exp, &type, (void **) &solution, 
			 &m_finished);
	P_ST
	solution->value = MAXINT;

	/* Anfangsknoten muss mit Heuristik bearbeitet werden: */
	sts = u_heur_loes (u_glob, startnode->u_node, 
			   solution->u_node, &solution->value);
	P_ST

	/* Schicke Anfangsknoten an Dispatcher: */
	sts = q_write_bl (exp_to_disp, Q_NODE, startnode, &d_finished);
	P_ST
	statistic.nodes++;
	TKN ("Startknoten", startnode);

	/* Schicke Anfangsincumbent an Dispatcher: */
	sts = q_write_bl (exp_to_disp, Q_INIT_INC, solution, 
			  &d_finished);
	P_ST
       }

    /* Starte Dispatcher: */
    proc_dispatcher = ProcAlloc (disp_server, STACK_SIZE, 7, 
				 disp_to_exp, exp_to_disp, disp_to_mem, 
				 disp_to_out, in_to_disp, cpu_nr,
				 tracer_qs[tt++]);
    if (!proc_dispatcher)
       {
	d_finished = S_PROC_ALLOC;
	FEHLER (S_PROC_ALLOC)
       }
    ProcRun (proc_dispatcher);
    T ("Dispatcher started")

    /* Starte Expander: */
    proc_expander = ProcAlloc (expander, STACK_SIZE, 5, mem_to_exp, 
			       exp_to_disp, disp_to_exp, u_glob, 
			       tracer_qs[tt++]);
    if (!proc_expander)
       {
	e_finished = S_PROC_ALLOC;
	FEHLER (S_PROC_ALLOC)
       }
    ProcRun (proc_expander);
    T ("Expander started")

    /* Starte In und Out auf allen benutzten Links: */
    for (i = 0; i < 4; i++) 
	/* Nur wenn Link tatsaechlich benutzt wird: */
	if (link_connected[i])
	   {
	    /* Starte In-Prozess: */
	    proc_in[i] = ProcAlloc (in, STACK_SIZE, 6, i, 
				    u_glob, in_channel[i], 
				    mem_to_in[i], in_to_disp[i], 
				    tracer_qs[tt++]);
	    if (!proc_in[i])
	       {
		in_finished[i] = S_PROC_ALLOC;
		FEHLER (S_PROC_ALLOC)
	       }
	    in_finished[i] = OK_FALSE;
	    ProcRun (proc_in[i]);
	    TV ("Started In", i)

	    /* Starte Out-Prozess: */
	    proc_out[i] = ProcAlloc (out, STACK_SIZE, 6, i, 
				     u_glob, out_channel[i], 
				     disp_to_out[i], out_to_mem[i], 
				     tracer_qs[tt++]);
	    if (!proc_out[i])
	       {
		out_finished[i] = S_PROC_ALLOC;
		FEHLER (S_PROC_ALLOC)
	       }
	    out_finished[i] = OK_FALSE;
	    ProcRun (proc_out[i]);
	    TV ("Started Out", i)
	   }
	else
	   {
	    /* Nicht benutzte Prozesse als tot eintragen: */
	    in_finished[i] = OK_TRUE;
	    out_finished[i] = OK_TRUE;
	   }

    /* ------------------------------------ */
    /* Jetzt sollten alle Prozesse laufen ! */
    /* ------------------------------------ */

#ifdef TRACE_ON
    /* Bis Dispatcher beendet und Tracer-Queues leer: */
    served = OK_FALSE;
    while (!d_finished || served)
       {
	served = OK_FALSE;
	/* Leere eigene Tracer-Queues: */
	for (i = 0; i < tt; i++)
	    if (q_read (tracer_qs[i], &type, (void **) &trace_buf))
	       {
		if (trace_buf)
		   {
		    fprintf (stderr, "#%d  from %d ****\n", 
			     trace_buf->time, i);
		    fprintf (stderr, trace_buf->str);
		    free (trace_buf);
		   }
		else
		    fprintf (stderr, "Gotta empty trace message\n");
		served = OK_TRUE;
	       }
	if (!served && !d_finished)
	    ProcReschedule ();
       }
#else
    /* Bis Dispatcher beendet: */
    while (!d_finished)
       {
	ProcReschedule ();
       }
#endif

    /* Uhr anhalten: (Rest zaehlt nicht zum Algorithmus) */
    statistic.time = ((int) clock ()) - timer_start;

    /* Wenn parallel und kein Fehler gewesen: */
    if (!(glob.flags & STAT_KEEP) && parallel && d_finished == D_DONE)
       {
	/* Ende von out[ring_out] abwarten, um freien Kanal zu haben: */
	while (!out_finished[ring_out])
	    ProcReschedule (); 

	/* Wenn nicht der root-Prozessor: */
	if (cpu_nr)
	   {
	    /* Lies Anzahl erwarteter Statistikbloecke: */
	    nr_of_stats = ChanInInt (in_channel[ring_in]);
	    /* Gib diese Information weiter: */
	    ChanOutInt (out_channel[ring_out], nr_of_stats + 1);
	    /* Lies und schreib alle fremden Bloecke: */
	    for (i = 0; i < nr_of_stats; i++)
	       {
		ChanIn (in_channel[ring_in], (void *) &stat_buffer, 
			sizeof (stat_t));
		ChanOut (out_channel[ring_out], (void *) &stat_buffer,
			sizeof (stat_t));
	       }
	    /* Schicke eigenen hinterher: */
	    ChanOut (out_channel[ring_out], (void *) &statistic,
		    sizeof (stat_t));
	   }
	/* Sonst (root) */
	else
	   {
	    /* Schreibe Anzahl (1): */
	    ChanOutInt (out_channel[ring_out], 1);
	    /* Warte, bis Anzahlen den Ring umrundet haben */
	    /* (Notwendig, um Deadlocks zu vermeiden) */
	    nr_of_stats = ChanInInt (in_channel[ring_in]);
	    /* Schicke eigene Statistik auf den Weg: */
	    ChanOut (out_channel[ring_out], (void *) &statistic,
		    sizeof (stat_t));
	    /* Lies alle Statistiken und gib sie aus: */
	    for (i = 0; i < nr_of_stats; i++)
	       {
		ChanIn (in_channel[ring_in], (void *) &stat_buffer, 
			sizeof (stat_t));
		if (glob.flags & STATISTIC)
		    print_stat (stat_buffer.cpu_nr, stat_buffer);
		/* Summiere Statistikinformationen auf: */
		stat_sums.nodes += stat_buffer.nodes;
		stat_sums.allocated += stat_buffer.allocated;
		stat_sums.expanded += stat_buffer.expanded;
		stat_sums.from_heap += stat_buffer.from_heap;
		stat_sums.from_lifo += stat_buffer.from_lifo;
		stat_sums.heur_solved += stat_buffer.heur_solved;
		stat_sums.elim_by_lb += stat_buffer.elim_by_lb;
		stat_sums.elim_by_ub += stat_buffer.elim_by_ub;
		stat_sums.elim_from_heap += stat_buffer.elim_from_heap;
		stat_sums.feasible += stat_buffer.feasible;
		stat_sums.improved += stat_buffer.improved;
		stat_sums.got += stat_buffer.got;
		stat_sums.sent += stat_buffer.sent;
		stat_sums.idle += stat_buffer.idle;
	       }
	    /* Bilde Durchschnittswerte, wo noetig: */
	    stat_sums.allocated /= nr_of_cpus;
	    stat_sums.from_lifo /= nr_of_cpus;
	    stat_sums.got /= nr_of_cpus;
	    stat_sums.sent /= nr_of_cpus;
	    stat_sums.idle /= nr_of_cpus;
	    if (glob.flags & STAT_SUMS)
		print_stat (-1, stat_sums);
	   }
       }
    /* Sonst (keine Statistik-Kaskade): */
    else if (glob.flags & STATISTIC)
	print_stat (0, statistic);
    else if (!parallel && glob.flags & STAT_SUMS)
	print_stat (-1, statistic);

    /* Gib Zeit aus: */
    fprintf (stderr, "TIME: %.3f\n", 
	     (float) statistic.time / (float) CLOCKS_PER_SEC);

    /* Restliche Prozesse beenden */
    /* (bis auf in[ring_in] und out[ring_out]): */
    if (!m_finished)
	m_finished = OK_TRUE;
    if (!e_finished)
	e_finished = OK_TRUE;
    for (i = 0; i < 4; i++)
       {
	if (!in_finished[i] && i != ring_in)
	    in_finished[i] = OK_TRUE;
	if (!out_finished[i] && i != ring_out)
	    out_finished[i] = OK_TRUE;
       }

    /* Wenn ordnungsgemaesses Ende */
    if (d_finished == D_DONE)
       {
	/* Beende auch in[ring_in] und out[ring_out]: */
	in_finished[ring_in] = OK_TRUE;
	out_finished[ring_out] = OK_TRUE;
	/* Wenn ROOT-Transputer */
	if (!cpu_nr)
	   {
	    if (!(glob.flags & NO_SOLUTION))
	       {
		/* Loesung ausgeben: */
		solution = h_read_incumbent ();
		sts = u_print_sol (u_glob, solution->u_node);
		P_ST
	       }
	    exit_terminate (0);
	   }
	else
	    exit (0);
       }
    /* Sonst (Abbruch nach Fehler): */
    else
       {
	/* Gib Fehlermeldung aus: */
	P_FEHLER (d_finished);
	/* Warte auf das Ende von in[ring_in]: */
	/* (Damit auch Ringvorgaenger Fehlertoken ausgeben kann) */
	while (!in_finished[ring_in])
	    ProcReschedule ();
	if (!cpu_nr)
	    exit_terminate (0);
	else
	    exit (0);
       }
   }

/*------------------------ PRINT_STAT --------------------------*/

static void	print_stat (cpu_nr, stat)

int	cpu_nr;
stat_t	stat;

   {
    int		i;
    int		j;
    int		scale;
    char	*name;
    int		serie = OK_FALSE;

    if (cpu_nr == 0)
       {
               /*        6        8       7       7    4   3   3 */  
	printf ("NR   exp.     erz. el(bd). el(he). heu. zl. im. ");
               /*    5      6      6      6  */                        
	printf (" all.  lifo.   sent   idle\n");
       }

    if (cpu_nr == -1)
       {
	if (read_param ("NAME", &name))
	   {
	    printf ("@ %s\t", name);
	    serie = OK_TRUE;
	   }
	else
	    printf ("SU ");
       }
    else
	printf ("%2d ", cpu_nr);

    if (serie)
	printf ("%.3f\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
		(float) statistic.time / (float) CLOCKS_PER_SEC,
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
    else
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
    if ((glob.flags & PROFILE) && cpu_nr != -1)
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
/* Initialisiert die datenstruktur 'glob', die alle Prozessoren
 * zu Beginn geschickt erhalten. Darin sind viele Parameter, die
 * auf der Kommandozeile angegeben wurden, enthalten
 */

static int	init_glob (glob)

glob_t		*glob;

   {
    char	*parvalue;

    glob->flags = 0;
    if (read_param ("TRACE_DISP", &parvalue))
	glob->flags |= TRACE_DISP;
    if (read_param ("TRACE_EXP", &parvalue))
	glob->flags |= TRACE_EXP;
    if (read_param ("TRACE_HEAP", &parvalue))
	glob->flags |= TRACE_HEAP;
    if (read_param ("TRACE_INOUT", &parvalue))
	glob->flags |= TRACE_INOUT;
    if (read_param ("TRACE_MEM", &parvalue))
	glob->flags |= TRACE_MEM;
    if (read_param ("TRACE_STARTUP", &parvalue))
	glob->flags |= TRACE_STARTUP;
    if (read_param ("ELIM_BY_HEUR", &parvalue))
	glob->flags |= ELIM_BY_HEUR;

    if (read_param ("DIST_LOW", &parvalue))
	glob->dist_low = atoi (parvalue);
    else
	glob->dist_low = DEF_DIST_LOW;

    if (read_param ("DIST_HIGH", &parvalue))
	glob->dist_high = atoi (parvalue);
    else
	glob->dist_high = DEF_DIST_HIGH;

    if (read_param ("DIST_IDLE", &parvalue))
	glob->dist_idle = atoi (parvalue);
    else
	glob->dist_idle = DEF_DIST_IDLE;

    if (read_param ("DIST_COUNT_FATHERS", &parvalue))
	glob->flags |= DIST_COUNT_FATHERS;

    if (read_param ("PRIO_PAR", &parvalue))
	glob->prio_par = atoi (parvalue);
    else
	glob->prio_par = DEF_PRIO_PAR;

    if (read_param ("STAT_SUMS", &parvalue))
	glob->flags |= STAT_SUMS;

    if (read_param ("STAT_KEEP", &parvalue))
	glob->flags |= STAT_KEEP;

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
/* Gibt fuer den Fehler 'sts; auf Prozessor 'p' eine Fehlermeldung 
 * auf stderr aus. 
 */

int	print_error (p, sts)

int		p; 
int		sts; 
 
   { 
    /* Benutzerfehler mit aufgenommen, soweit bekannt: */
    fprintf (stderr, "\n*** FEHLER\n*** Prozessor %d: ", p);
    if (sts > U_MALLOC)
	fprintf (stderr, "Fehler in Benutzermodul: %d\n", sts);
    else
	switch (sts)
	   {
	    case U_INCOMPLETE_SOL:
		fprintf (stderr, "Unvollstaendige Loesung\n");
		break;
	    case U_PARAM:
		fprintf (stderr, "Falsche oder fehlende Parameter\n");
		break;
	    case U_FILE:
		fprintf (stderr, "Fehler bei Dateizugriff\n");
		break;
	    case U_MALLOC:
		fprintf (stderr, 
			 "Zu wenig Speicher fuer Instanzfkt.\n");
		break;
	    case D_DONE:
		fprintf (stderr, "Dispatcher unerwartet beendet\n");
		break;
	    case IO_MALLOC:
		fprintf (stderr, "Zu wenig Speicher fuer In/Out\n");
		break;
	    case M_INIT_MEM:
		fprintf (stderr, "Zu wenig Speicher fuer Knoten\n");
		break;
	    case Q_MAKE_QUEUE:
		fprintf (stderr, "Kann Queue nicht anlegen\n");
		break;
	    case Q_WRONG_TYPE:
		fprintf (stderr, "Queue enthaelt falschen Typ\n");
		break;
	    case Q_PARTNER_DEAD:
		fprintf (stderr, "Lesender Prozess fuer Queue ");
		fprintf (stderr, "unerwartet beendet\n");
		break;
	    case S_PROC_ALLOC:
		fprintf (stderr, "Kann Prozess nicht allokieren\n");
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

/*------------------------ ENDE ---------------------------------*/
