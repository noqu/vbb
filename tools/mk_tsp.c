/*---------------------------------------------------------------*/
/* mk_tsp.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 1							 */
/*---------------------------------------------------------------*/
/* Erzeugt ein zufaelliges Traveling Salesman Problem
 */
/*------------------------ PRAEPROCESSOR ------------------------*/
#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/timeb.h>
#ifndef VBB_ORIGINAL
#include  <stdlib.h>
#endif
/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/
/*------------------------ MAKROS -------------------------------*/
#define  USAGE { fprintf (stderr,\
		     "Usage: %s [-s size] [-m maxlen] [-p prob]\n",\
		     argv[0]);\
		 exit (1); \
	       }

#define  FEHLER(STR) {fprintf ("Fehler: %s\n", STR); exit (1); }

/*------------------------ FUNKTIONEN ---------------------------*/
/*------------------------ MAIN ---------------------------------*/

main (argc, argv)

int argc;
char **argv;

{
    int			zeilen = 20;
    int			maxlen = 99;
    int			prob = 100;
    int			i;
    int			j;
    long		seed;
    struct timeb	zeit;
    int			c;
    extern int 		optind;
    extern char		*optarg;
    int			errflg = 0;

    while ((c =	getopt (argc, argv, "s:m:p:")) != EOF)
       {
	switch	(c) 
	   {
	    case 's':
		zeilen = atoi (optarg);
		break;
	    case 'm':
		maxlen = atoi (optarg);
		break;
	    case 'p':
		prob = atoi (optarg);
		break;
	    case '?':
		errflg++;
	   } /* end switch */
       } /* end while */
    if (errflg)	
	USAGE

    /* Zeit holen und Zufallsgenerator initialisieren: */
    ftime (&zeit);
    seed = zeit.millitm;
    seed = seed + 1000 * seed + 1000000 * seed + zeit.time;
    srand48 (seed);

    printf ("# TSP mit %d Knoten\n", zeilen);
    printf ("# Kantenlaengen gleichverteilt ueber [0, %d]\n", maxlen);
    printf ("# Verbindungswahrscheinlichkeit %2d %%\n", prob);
    printf ("ZEILEN=%d\n", zeilen);
    printf ("SPALTEN=%d\n", zeilen);
    for (i = 0; i < zeilen; i++)
       {
	for (j = 0; j < zeilen; j++)
	   {
	    if (i == j || (int) lrand48 () % 101 > prob)
		printf ("  M");
	    else
		printf ("%3d", (int) (lrand48 () % (maxlen + 1)));
	   } /* end for */
	printf ("\n");
       } /* end for */
}

/*------------------------ ENDE ---------------------------------*/
