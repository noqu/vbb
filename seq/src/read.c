/*---------------------------------------------------------------*/
/* read.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck		26.4.93 				 */
/* Version 4.0 seq						 */
/*---------------------------------------------------------------*/
/* Stellt Funktionen zur Parametereingabe ueber Kommandozeile und
 * Parameterdateien bereit, ausserdem eine Matrix-Eingabefunktion
 * und eine Eingabefunktion fuer Kantenlisten.
 */
/*------------------------ PRAEPROCESSOR ------------------------*/

#include        <stdio.h>
#include  	<stdlib.h>
#include        <string.h>
#include        <ctype.h>
#include        "error.h"
#include        "read.h"
#include        "set.h"

#define  	LINEBUF		512	/* Puffer fuer 1 Zeile in
					 * matrin () 
					 */
#define		MAX_PARAM	256	/* Max Anzahl Parameter	*/
#define		INIT_BUF_SIZE	1024	/* Anfangsgroesse Puffer fuer
					 * alle Parameter
					 */
#define		BUF_BLK_SIZE	1024	/* Blocks zum nachtraegl.
					 * Vergroessern	
					 */
#define		MIN_SPACE_LEFT	160	/* Maximale Zeilenlaenge
					 * fuer Parameterdatei
					 */

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

static char	*params[MAX_PARAM];
static char	*values[MAX_PARAM];

/*------------------------ MAKROS -------------------------------*/

#define	 FAIL(X)	{ fprintf X; return (READ_ERR); }

/*------------------------ FUNKTIONEN ---------------------------*/

#ifdef SUN_CC

public int	kl_input ();
public int	matrin ();
public int	read_param ();
public int	init_params ();
static	int	readline ();
static	int	readval ();

#else

public int	kl_input (
			FILE*		/* idat */,
			int*		/* Xsize */,
			int*		/* Ysize */,
			int**		/* values */,
			set_t***	/* pred */,
			set_t***	/* succ */);

public int	matrin (
			FILE*		/* indat */, 
			int***		/* matrix */, 
			int*		/* zeilen */, 
			int*		/* spalten */);

public int	read_param (
			char*		/* para */, 
			char**		/* val */);
	
public int	init_params (
			char**		/* buffer */,
			int		/* ac */,
			char**		/* av */);

static	int	readline (
			FILE*		/* indat */,
			char*		/* buffer */,
			int*		/* size */,
			int*		/* line */);

static	int	readval (
			char*		/* str */, 
			int		/* m */);
	
#endif

/*------------------------ KL_INPUT ----------------------------*/

int	kl_input (idat, Xsize, Ysize, values, pred, succ)

FILE		*idat;
int		*Xsize;
int		*Ysize;
int		**values;
set_t		***pred;
set_t		***succ;

   {
    int		i;
    char	lbuf[LINEBUF];
    int		line = 0;
    int		anz;
    char	*tok[3];
    int		all_correct = OK_TRUE;
    int		line_correct = OK_TRUE;
    int		size;
    int		*reached;
    int		from;
    int		to;
    int		wert;
    boolean 	ecke;

    /* Parameter QUELLEN lesen */
    if (!readline (idat, lbuf, &size, &line) 
	|| (sscanf (lbuf, "QUELLEN=%d", Ysize) != 1))
	FAIL ((stderr, "Parameter QUELLEN nicht gefunden\n"));

    /* Parameter SENKEN lesen */
    if (! readline (idat, lbuf, &size, &line) 
	|| (sscanf (lbuf, "SENKEN=%d", Xsize) != 1))
	FAIL ((stderr, "Parameter SENKEN nicht gefunden\n"));

    /* Platz fuer Werte allokieren */
    *values = (int *) malloc (*Ysize * sizeof (int ));
    if (!*values)
	FAIL ((stderr, "Malloc fuer Werte failed\n"));
    for (i = 0; i < *Ysize; i++)
	(*values)[i] = -1;

    /* Mengenlisten anlegen: */
    *succ = (set_t **) malloc (*Ysize * sizeof (set_t *));
    if (!*succ)
	FAIL ((stderr, "Malloc fuer Mengenliste failed\n"));
    *pred = (set_t **) malloc (*Xsize * sizeof (set_t *));
    if (!*pred)
	FAIL ((stderr, "Malloc fuer Mengenliste failed\n"));

    /* Mengen anlegen: */
    for (i = 0; i < *Ysize; i++)
       {
	if (!create_set (0, *Xsize, &((*succ)[i])))
	    FAIL ((stderr, "Malloc fuer succ[%d] failed\n", i));
       } /* end for */
    for (i = 0; i < *Xsize; i++)
       {
	if (!create_set (0, *Ysize, &((*pred)[i])))
	    FAIL ((stderr, "Malloc fuer pred[%d] failed\n", i));
       } /* end for */
    
    /* Platz fuer Tokens anlegen: */
    for (i = 0; i < 3; i++)
       {
	tok[i] = (char *) malloc (LINEBUF * sizeof (char));
	if (!tok[i])
	    FAIL ((stderr, "Malloc fuer tok[%d] failed\n", i));
       }

    reached = (int *) calloc (*Xsize, sizeof (int));
    if (!reached)
	FAIL ((stderr, "Malloc fuer reached failed\n"));

    /* Solange noch eine Zeile in der Datei: */
    while (readline (idat, lbuf, &size, &line))
       {
	if (!line_correct)
	    all_correct = OK_FALSE;
	line_correct = OK_TRUE;
	/* Tokens zuruecksetzen: */
	for (i = 0; i < 3; i++)
	    tok[i][0] = '\0';
	/* 3 Tokens aus Zeile lesen: */
	anz = sscanf (lbuf, "%s %s %s", tok[0], tok[1], tok[2]);

	/* Wenn Format <ecke> ; <wert> */
	if (anz == 3 && tok[1][0] == ';')
	   {
	    ecke = OK_TRUE;
	    if (!sscanf (tok[0], "%d", &from) 
	     || !sscanf (tok[2], "%d", &wert))
		line_correct = OK_FALSE;
	   }
	/* Sonst wenn Format <ecke1> <ecke2> */
	else if (anz == 2)
	   {
	    ecke = OK_FALSE;
	    if (!sscanf (tok[0], "%d", &from) 
	     || !sscanf (tok[1], "%d", &to))
		line_correct = OK_FALSE;
	   }
	/* Alles andere ist falsch: */
	else
	    line_correct = OK_FALSE;
	   
	/* Bei Formatfehler gleich zu naechster Zeile: */
	if (!line_correct)
	   {
	    fprintf (stderr, "Formatfehler in Zeile %d !\n", line);
	    continue;
	   }

	/* fprintf (stderr, "from %d ecke %d to %d\n", from, ecke, to); */
     
	/* Pruefe die Eckennummern auf Bereichsueberschreitung:*/
	if ((from < 0 || from >= *Ysize)
	 || (!ecke && (to < 0 || to >= *Xsize)))
	   {
	    fprintf (stderr, "Zu grosse oder zu kleine Eckennummer \
in Zeile %d !\n", line);
	    line_correct = OK_FALSE;
	    continue;
	   }
	    
	/* Wenn Ecke gefunden: */
	if (ecke)
	   {
	    if (wert < 0)
	       {
		fprintf (stderr, "Wert in Zeile %d negativ !\n",
			 line);
		line_correct = OK_FALSE;
	       }
	    if ((*values)[from] >= 0)
	       {
		fprintf (stderr, "Neudeklaration von Ecke '%d' \
in Zeile %d ignoriert\n", from, line);
	       }
	    else
		(*values)[from] = wert;
	   }
	/* Sonst (Kante gefunden): */
	else
	   {
	    add_elem (to, (*succ)[from], (*succ)[from]);
	    add_elem (from, (*pred)[to], (*pred)[to]);
	    reached[to]++;
	   }
       } /* end while */

    /* Pruefe Quellen auf vorhandenen Wert: */
    for (i = 0; i < *Ysize; i++)
       {
	if ((*values)[i] < 0)
	   {
	    fprintf (stderr, "Ecke '%d' hat keinen Wert !\n", i);
	    all_correct = OK_FALSE;
	   }
       }

    /* Pruefe Senken auf Erreichbarkeit: */
    for (i = 0; i < *Xsize; i++)
       {
	if (!reached[i])
	   {
	    fprintf (stderr, "Ecke '%d' nicht erreichbar !\n", i);
	    all_correct = OK_FALSE;
	   }
       }

    free (tok[2]);
    free (tok[1]);
    free (tok[0]);
    free (reached);

    if (all_correct)
	return (OK);
    else
	return (READ_ERR);
   }

/*^*/
/*------------------------ MATRIN ------------------------------*/
/* Liest eine Matrix aus einer Datei ein. In der Datei stehen vorne 
 * Parameter, die die Matrix beschreiben. Die Parameter ZEILEN und 
 * SPALTEN muessen vor den eigentlichen Matrixdaten stehen. Die 
 * Matrixdaten werden zeilenweise als Integer gelesen. Das Zeichen 
 * M steht dabei fuer MAXINT. Negative Werte muessen ohne 
 * Leerzeichen zwischen Minuszeichen und Ziffer eingegeben werden. 
 * Kommentare sind nach dem Zeichen # bis Zeilenende moeglich. 
 * Die Matrix wird von der Datei 'indat' gelesen und mit malloc 
 * aufgebaut. In 'matrix' wird ein Zeiger auf die Matrix 
 * zurueckgeliefert, in 'zeilen' und 'spalten' die Dimensionen der 
 * Matrix. 
 */

int	matrin (indat, matrix, zeilen, spalten)

FILE		*indat;
int		***matrix;
int		*zeilen;
int		*spalten;

   {
    char	buffer[LINEBUF];
    int		i;
    int		j;
    char	*next;
    int		*mat_buf;
    int		size;
    int		line = 0;

    /* Parameter ZEILEN lesen */
    if (!readline (indat, buffer, &size, &line) 
	|| (sscanf (buffer, "ZEILEN=%d", zeilen) != 1))
	FAIL ((stderr, "Parameter ZEILEN nicht gefunden\n"));

    /* Parameter SPALTEN lesen */
    if (! readline (indat, buffer, &size, &line) 
	|| (sscanf (buffer, "SPALTEN=%d", spalten) != 1))
	FAIL ((stderr, "Parameter SPALTEN nicht gefunden\n"));

    /* Platz fuer Zeilen allokieren */
    *matrix = (int **) malloc (*zeilen * sizeof (int *));
    if (!*matrix)
	FAIL ((stderr, "Malloc fuer Zeilen failed\n"));
    
    /* Platz fuer Matrixkoerper allokieren (zusammenhaengend): */
    mat_buf = (int *) malloc (*zeilen * *spalten * sizeof (int));
    if (!mat_buf)
	FAIL ((stderr, "Malloc fuer Matrixkoerper failed\n"));

    /* Zeilenweise allokieren und mit Werten fuellen */
    for (i = 0; i < *zeilen; i++)
       {
	(*matrix)[i] = mat_buf + (*spalten * i);
	if (!readline (indat, buffer, &size, &line))
	    FAIL ((stderr, "Zeile %d nicht gefunden\n", i + 1));
	next = buffer;

	/* Elemente einer Zeile einzeln verarbeiten */
	for (j = 0; j < *spalten; j++)
	   {
	    /* Leerraum ueberspringen: */
	    while (next && isspace (*next))
		next++;
	    (*matrix)[i][j] = readval (next, MAXINT / *zeilen);
	    /* gelesenes Zeichen ueberspringen: */
	    while (next && !isspace (*next))
		next++;
	    if (!next && j < *spalten - 1)
		FAIL ((stderr, "Element %d/%d nicht gefunden\n",
			 i + 1, j + 1));
	   } /* end for j */
       } /* end for i */
    return (OK);
   }

/*------------------------ GET_PARAM ---------------------------*/
/* Liest einen Parameter aus der Liste. In 'val' wird (sofern
 * vorhanden) der Parameterwert zurueckgegeben, sonst 0. Der
 * Rueckgabewert ist TRUE, wenn der Parameter in der Liste ist,
 * sonst FALSE. Es wird der erste passende Wert aus der Liste 
 * zurueckgegeben. Voraussetzung fuer read_param () ist der voran-
 * gehende Aufruf von init_params (), um die Liste zu fuellen.
 */

int	read_param (para, val)

char	*para;
char	**val;

   {
    int		i;

    for (i = 0; params[i]; i++)
        if (strcmp (para, params[i]) == 0)
           {
            *val = values[i];
            return (OK_TRUE);
	   }
    *val = NULL;
    return (OK_FALSE);
   }

/*------------------------ READ_PARAMS -------------------------*/
/* Liest von der Kommandozeile Parameter der Form 'para=value' bzw
 * 'para'. Ein Argument der Form '@file' verweist auf eine Parameter-
 * datei, aus der Parameter wie von der Kommandozeile gelesen werden.
 * Die gelesenen Parameter werden in eine Liste eingetragen, deren
 * Strings zusammenhaengend in 'buffer' stehen. Fehler beim Oeffnen
 * von Dateien und Syntaxfehler werden direkt auf stderr gemeldet.
 * Die Reihenfolge der Parameter wird argument- und dateiweise bei-
 * behalten.
 */ 

int	init_params (buffer, ac, av)

char	**buffer;
int	ac;
char	**av;

   {
    FILE	*parfile;
    char	*w;
    int		parbufsize;
    int		space_left;
    char	*curbuf;
    int		fertig = OK_FALSE;
    int		offset;
    int		line = 0;
    int		size;
    int		anz_par = 0;
    int		in_file = OK_FALSE;
    int		curarg = 1;
    char	*filename;
    int		korrekt = OK_TRUE;


    /* Puffer fuer Strings allokieren: */
    parbufsize = INIT_BUF_SIZE;
    *buffer = (char *) malloc (parbufsize);
    if (!*buffer)
	FAIL ((stderr, "Malloc fuer Parameterpuffer failed\n"))
    space_left = parbufsize;
    curbuf = *buffer;

    while (!fertig)
       {
        /* Wenn nicht genug Platz uebrig: */
	if (space_left < MIN_SPACE_LEFT)
	   {
	    /* Puffer vergroessern: */
	    offset = curbuf - *buffer;
	    parbufsize += BUF_BLK_SIZE;
	    *buffer = (char *) realloc (*buffer, parbufsize);
	    if (!*buffer)
		FAIL ((stderr, "Malloc fuer Parameterpuffer failed\n"))
	    space_left += BUF_BLK_SIZE;
	    curbuf = *buffer + offset;
	   }

	/* Wenn in einer Parameterdatei: */
	if (in_file)
	   {
	    /* Eine neue Zeile aus der Datei lesen: */
	    if (!readline (parfile, curbuf, &size, &line))
	       {
		in_file = OK_FALSE;
		fclose (parfile);
		continue;
	       }
	   }
	/* Sonst, wenn noch Argumente zu bearbeiten: */
	else if (curarg < ac)
	   {
	    /* Wenn Argument mit @ beginnt: */
	    if (av[curarg][0] == '@')
	       {
		filename = av[curarg++] + 1;
		/* Parameterdatei oeffnen: */
		parfile = fopen (filename, "r");
		if (!parfile)
		   {
		    fprintf (stderr, "%s: Kann %s nicht oeffnen\n",
			     av[0], filename);
		    korrekt = OK_FALSE;
		    continue;
		   }
		in_file = OK_TRUE;
		line = 0;
		continue;
	       }
	    else
	       {
		/* Argument als Parameter verarbeiten: */
		strcpy (curbuf, av[curarg++]);
		size = strlen (curbuf);
		line = curarg - 1;
	       }
	   }
	/* Sonst (alle Argument bearbeitet) */
	else
	   {
	    fertig = OK_TRUE;
	    continue;
	   }

	space_left -= size;
	/* Erstes Wort als Parametername registrieren: */
	params[anz_par] = curbuf;

	/* Vorspulen bis Gleichheitszeichen oder Ende: */
	for (w = curbuf; *w != '=' && w - curbuf < size; w++)
	    /* Dabei Leerzeichen eliminieren: */
	    if (isspace (*w))
		*w = '\0';

	/* Wenn Gleichheitszeichen gefunden: */
	if (*w == '=')
	   {
	    /* Gleichheitszeichen u. folgende Leerzeichen elim.: */
	    do
		*w++ = '\0';
	    while (isspace (*w) && w - curbuf < size);

	    /* Wenn nur Space bis Ende: */
	    if (w - curbuf >= size)
	       {
		if (in_file)
		    fprintf (stderr, "%s: Wert fehlt in Zeile %d\n", 
			     filename, line);
		else
		    fprintf (stderr, "Wert fehlt bei Arg. %d\n", 
			     line);
		values[anz_par++] = NULL;
	       }
	    else
	       {
		/* Sonst Wort als Parameterwert registrieren: */
		values[anz_par++] = w;
		/* Von hinten Leerzeichen eliminieren: */
		for (w = curbuf + size - 1; isspace (*w); w--)
		    *w = '\0';
	       }
	   }
	else
	    /* Kein Parameterwert vorhanden: */
	    values[anz_par++] = NULL;

	/* Puffer fuer naechste Zeile weitersetzen: */
	curbuf = w + 2;
       }
    return (OK);
   }

/*^*/
/*------------------------ READVAL -----------------------------*/
/* Liest einen Wert aus dem String str und liefert ihn zurueck.
 * Fuer das Zeichen M im String wird der Wert m zurueckgegeben.
 */

static	int	readval (str, m)

char		*str;
int		m;

   {
    if (str[0] == 'M')
	return (m);
    if (str[0] == '-' && str[1] == 'M')
	return (- m);
    return (atoi (str));
   }

/*------------------------ READLINE ----------------------------*/
/* Liest eine Zeile von der Eingabedatei 'indat' in den Puffer 
 * 'buffer'. Kommentare werden nach Shell-Konvention ignoriert. 
 * Fuehrende Leerzeichen werden ebenfalls ignoriert. In 'size' wird 
 * die Laenge der gelesenen Zeile zurueckgegeben, 'line' wird 
 * aufgrund der gelesenen Zeilen weitergefuehrt. Rueckgabewert ist
 * TRUE, wenn etwas gelesen wurde, sonst FALSE.
 */

static int	readline (indat, buffer, size, line)

FILE	*indat;
char	*buffer;
int	*size;
int	*line;

   {
    char	*next = buffer;
    int		c;

    *size = 0;

    /* Lies Zeichen, bis EOF erreicht: */
    while ((c = getc (indat)) != EOF)
       {
        /* Wenn ein Newline gelesen: */
        if (c == '\n')
           {
            (*line)++;
            /* Wenn Zeile noch leer: */
            if (next == buffer)
                continue;
	    else
		break;
	   }

        /* Wenn Kommentar beginnt: */
	if (c == '#')
	   {
	    /* Alles schlucken bis Newline oder EOF: */
	    while ((c = getc (indat)) != '\n' && c != EOF)
	        ; /* nix */
	    /* Newline wieder zurueckstellen: */
	    ungetc (c, indat);
	    continue;
	   }

        /* Wenn Zeile noch leer und Leerzeichen gelesen: */
        if (next == buffer && isspace (c))
	    continue;

	/* Sonst als normales Zeichen akzeptieren: */
	*next++ = c;
	(*size)++;
       }

    /* Wenn Zeile noch leer: */
    if (next == buffer)
	return (OK_FALSE);
    else
       {
        /* String mit Null abschliessen: */
        *next = '\0';
        return (OK_TRUE);
       }
   }

/*------------------------ ENDE --------------------------------*/
