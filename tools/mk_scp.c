/*---------------------------------------------------------------*/
/* mk_scp.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 1							 */
/*---------------------------------------------------------------*/
/* Erzeugt ein zufaelliges Set Covering Problem
 */
/*------------------------ PRAEPROCESSOR ------------------------*/
#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/timeb.h>
/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/
/*------------------------ MAKROS -------------------------------*/
#define  USAGE { fprintf (stderr,\
		     "Usage: %s [-q quellen] [-s senken] [-m maxval] [-p prob]\n",\
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
    int			quellen = 10;
    int			senken = 10;
    int			maxlen = 99;
    int			prob = 30;
    int			i;
    int			j;
    long		seed;
    struct timeb	zeit;
    int			c;
    extern int 		optind;
    extern char		*optarg;
    int			errflg = 0;

    while ((c =	getopt (argc, argv, "q:s:m:p:")) != EOF)
       {
	switch	(c) 
	   {
	    case 'q':
		quellen = atoi (optarg);
		break;
	    case 's':
		senken = atoi (optarg);
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

    printf ("# SCP mit %d Quellen und %d Senken\n", quellen, senken);
    printf ("# Bewertungen gleichverteilt ueber [0, %d]\n", maxlen);
    printf ("# Verbindungswahrscheinlichkeit %2d %%\n", prob);
    printf ("QUELLEN=%d\n", quellen);
    printf ("SENKEN=%d\n", senken);
    printf ("# Ecken mit Bewertungen: \n# <ECKE> ; <WERT>\n", senken);
    for (i = 0; i < quellen; i++)
	printf ("%6d   ; %5d\n", i, (int) (lrand48 () % (maxlen + 1)));
    printf ("# Kanten: \n# <VON> <NACH>\n");
    for (i = 0; i < quellen; i++)
       {
	for (j = 0; j < senken; j++)
	   {
	    if ((int) lrand48 () % 101 <= prob)
		printf ("%6d %6d\n", i, j);
	   } /* end for */
       } /* end for */
}

/*------------------------ ENDE ---------------------------------*/
