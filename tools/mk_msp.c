/*---------------------------------------------------------------*/
/* mk_msp.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 1							 */
/*---------------------------------------------------------------*/
/* Erzeugt ein Maschinenbelegungsproblem mit 3 Maschinen.
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
		     "Usage: %s [-s size] [-m maxtime]\n",\
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
    int			spalten = 20;
    int			maxlen = 99;
    int			i;
    int			j;
    long		seed;
    struct timeb	zeit;
    int			c;
    extern int 		optind;
    extern char		*optarg;
    int			errflg = 0;

    while ((c =	getopt (argc, argv, "s:m:")) != EOF)
       {
	switch	(c) 
	   {
	    case 's':
		spalten = atoi (optarg);
		break;
	    case 'm':
		maxlen = atoi (optarg);
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

    printf ("# MSP mit %d Auftraegen\n", spalten);
    printf ("# Auftragsdauern gleichverteilt ueber [0, %d]\n", maxlen);
    printf ("ZEILEN=3\n");
    printf ("SPALTEN=%d\n", spalten);
    for (i = 0; i < 3; i++)
       {
	for (j = 0; j < spalten; j++)
	    printf ("%3d", (int) (lrand48 () % (maxlen + 1)));
	printf ("\n");
       } /* end for */
}

/*------------------------ ENDE ---------------------------------*/
