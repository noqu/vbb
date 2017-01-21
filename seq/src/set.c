/*---------------------------------------------------------------*/
/* set.c							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 1							 */
/*---------------------------------------------------------------*/
/* Modul fuer Bitfeld-Implementation von Mengen
 */
/*------------------------ PRAEPROCESSOR ------------------------*/

#include  	<stdio.h>
#include  	<stdlib.h>
#include  	<string.h>
#include        "set.h"

/* #define         DEBUG */

/*------------------------ MAKROS -------------------------------*/

#define	ALIGN(X)	((((X) + sizeof (int) - 1) / sizeof (int)) \
			 * sizeof (int))

#ifdef SUN_CC
#ifdef VBB_ORIGINAL
#define SPRINTF(ARGS)	(strlen (sprintf ARGS))
#else
#define SPRINTF(ARGS)   (sprintf ARGS)
#endif
#else
#define SPRINTF(ARGS)	(sprintf ARGS)
#endif

/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

static char	bit[] = {1, 2, 4, 8, 16, 32, 64, 128};

/*------------------------ FUNKTIONEN --------------------------*/

#ifdef SUN_CC

public int	create_set ();
public void	flush_set ();
public int	copy_set ();
public int	is_in ();
public int	is_subset ();
public int	is_empty ();
public int	is_equal ();
public int	add_elem ();
public int	del_elem ();
public int	union_set ();
public int	section ();
public int	minus ();
public void	print_set ();
public int	sprint_set ();
public int	nr_of_el ();
public int	get_first ();
public int	get_next ();
public void	flat_set ();
public void	unflat_set ();
public int	sizeof_flat_set ();

#else

public int	create_set (
			int	/* start */, 
			int	/* size */, 
			set_t**	/* set */);

public void	flush_set (
			set_t*	/* set */);

public int	copy_set (
			set_t*	/* set1 */, 
			set_t*	/* set2 */);

public int	is_in (
			int	/* elem */, 
			set_t*	/* set */);

public int	is_subset (
			set_t*	/* set1 */, 
			set_t*	/* set2 */);

public int	is_empty (
			set_t*	/* set */);

public int	is_equal (
			set_t*	/* set1 */, 
			set_t*	/* set2 */);

public int	add_elem (
			int	/* elem */, 
			set_t*	/* set */, 
			set_t*	/* result */);

public int	del_elem (
			int	/* elem */, 
			set_t*	/* set */, 
			set_t*	/* result */);

public int	union_set (
			set_t*	/* set1 */, 
			set_t*	/* set2 */, 
			set_t*	/* result */);

public int	section (
			set_t*	/* set1 */, 
			set_t*	/* set2 */, 
			set_t*	/* result */);

public int	minus (
			set_t*	/* set1 */, 
			set_t*	/* set2 */, 
			set_t*	/* result */);

public void	print_set (
			FILE*	/* fp */,
			set_t*	/* set */);

public int	sprint_set (
			char*	/* buf */,
			set_t*	/* set */);

public int	nr_of_el (
			set_t*	/* set */);

public int	get_first (
			set_t*	/* set */);

public int	get_next (
			set_t*	/* set */);

public void	flat_set (
			set_t*	/* set */,
			char*	/* flat */);

public void	unflat_set (
			char*	/* flat */,
			set_t*	/* set */);

public int	sizeof_flat_set (
			set_t*	/* set */);

#endif
/*------------------------ CREATE_SET --------------------------*/
/* Legt eine neue Menge an, die maximal 'size' Elemente aus dem 
 * Intervall [start, start + size] aufnehmen kann. Speicherplatz 
 * wird allokiert, Rueckgabewert ist 0, wenn Fehler, sonst 1. 
 * Position ist in der neuen Menge undefiniert. 
 */

int	create_set (start, size, set)

int	start;
int	size;
set_t	**set;

   {
    /* Datenstruktur fuer set allokieren: */
    *set = (set_t *) malloc (sizeof (set_t));
    if (!*set)
	return (0);
    /* Bitfeld ist aus unsigned char */
    (*set)->field = (unsigned char *) calloc (size / 8 + 1, 1);
    if (!(*set)->field)
	return (0);
    (*set)->size = size;
    (*set)->start = start;
    (*set)->pos = -1;
    return (1);
   }

/*------------------------ FLUSH_SET ---------------------------*/
/* Entfernt alle Elemente aus einer Menge. Position ist danach
 * undefiniert.
 */

void	flush_set (set)

set_t	*set;

   {
    int		i;

    for (i = 0; i < set->size / 8 + 1; i++)
	set->field[i] = 0;
    set->pos = -1;
   }

/*------------------------ COPY_SET ----------------------------*/
/* Kopiert den Inhalt von set1 nach set2. set2 muss bereits 
 * existieren und vom gleichen Typ sein. Rueckgabewert ist 1 bei 
 * erfolgreicher Kopie, 0 bei Typfehlern. Die Position in set2 ist 
 * undefiniert, die in set1 bleibt unveraendert. 
 */

int	copy_set (set1, set2)

set_t	*set1;
set_t	*set2;

   {
    if (set1->size != set2->size)
	return (0);
    if (set1->start != set2->start)
	return (0);
    memcpy (set2->field, set1->field, set1->size / 8 + 1);
    set2->pos = -1;
    return (1);
   }

/*------------------------ IS_IN -------------------------------*/
/* Stellt fest, ob das Element 'elem' in der Menge 'set' vorkommt. 
 * Wenn das nicht der Fall ist oder wenn elem nicht in Mengen 
 * dieses Typs vorkommen kann, ist der Rueckgabewert 0, sonst 1. 
 */

int	is_in (elem, set)

int	elem;
set_t	*set;

   {
    elem -=set->start;
    if (elem < 0 || elem >= set->size)
	return (0);
    return (set->field[elem / 8] & bit[elem % 8]);
   }

/*------------------------ IS_SUBSET ---------------------------*/
/* Stellt fest, ob set1 eine Teilmenge von set2 ist. Wenn das nicht 
 * der Fall ist oder die Mengen in ihrem Typ verschieden sind, ist 
 * der Rueckgabewert 0, sonst 1. 
 */

int	is_subset (set1, set2)

set_t	*set1;
set_t	*set2;

   {
    int		i;

    if (set1->size != set2->size)
	return (0);
    if (set1->start != set2->start)
	return (0);
    /* Idee: A subset B <=> A sect B = A */
    for (i = 0; i < set1->size / 8 + 1; i++)
	if ((set1->field[i] & set2->field[i]) != set1->field[i])
	    return (0);
    return (1);
   }

/*------------------------ IS_EMPTY ----------------------------*/
/* Stellt fest, ob die Menge set leer ist. Wenn das der fall ist, 
 * ist der Rueckgabewert 1, sonst 0. 
 */

int	is_empty (set)

set_t	*set;

   {
    int		i;

    /* Alle Bytes im Bitfeld muessen leer sein: */
    for (i = 0; i < set->size / 8 + 1; i++)
	if (set->field[i])
	    return (0);
    return (1);
   }

/*------------------------ IS_EQUAL ----------------------------*/
/* Stellt fest, ob die Menge set1 die gleichen Elemente enthaelt 
 * wie set2. Wenn das nicht der Fall ist oder die Mengen in ihrem 
 * Typ voneinander abweichen, ist der Rueckgabewert 0, sonst 1. 
 */

int	is_equal (set1, set2)

set_t	*set1;
set_t	*set2;

   {
    int		i;

    if (set1->size != set2->size)
	return (0);
    if (set1->start != set2->start)
	return (0);
    /* Alle Bitfelder muessen uebereinstimmen: */
    for (i = 0; i < set1->size / 8 + 1; i++)
	if (set1->field[i] != set2->field[i])
	    return (0);
    return (1);
   }

/*------------------------ ADD_ELEM ----------------------------*/
/* Fuegt das Element 'elem' zu der Menge 'set' hinzu. Das Resultat 
 * steht in der Menge 'result'. Wenn set und result in ihrem Typ 
 * verschieden sind oder wenn elem nicht in Mengen dieses Typs 
 * vorkommen kann, so ist der Rueckgabewert 0 und result 
 * undefiniert. Ansonsten ist der Rueckgabewert 1. Die Position 
 * wird durch das Einfuegen eines Elements stets undefiniert. 
 */

int	add_elem (elem, set, result)

int	elem;
set_t	*set;
set_t	*result;

   {
    elem -= set->start;
    if (elem < 0 || elem >= set->size)
	return (0);
    /* Wenn result eigene Menge, zuerst kopieren: */
    if (set != result)
	if (!copy_set (set, result))
	    return (0);
    result->field[elem / 8] |= bit[elem % 8];
    result->pos = -1;
    return (1);
   }

/*------------------------ DEL_ELEM ----------------------------*/
/* Loescht das Element 'elem' aus der Menge 'set'. Das Resultat 
 * steht in der Menge 'result'. Wenn set und result in ihrem Typ 
 * verschieden sind oder wenn elem nicht in Mengen dieses Typs 
 * vorkommen kann, so ist der Rueckgabewert 0 und result 
 * undefiniert. Ansonsten ist der Rueckgabewert 1. Die Position 
 * wird durch das Loeschen eines Elements stets undefiniert. 
 */

int	del_elem (elem, set, result)

int	elem;
set_t	*set;
set_t	*result;

   {
    elem -=set->start;
    if (elem < 0 || elem >= set->size)
	return (0);
    /* Wenn result eigene Menge, zuerst kopieren: */
    if (set != result)
	if (!copy_set (set, result))
	    return (0);
    result->field[elem / 8] &= ~(bit[elem % 8]);
    result->pos = -1;
    return (1);
   }

/*------------------------ UNION_SET ---------------------------*/
/* Bildet die Vereinigungsmenge aus set1 und set2. Das Resultat 
 * steht in der Menge 'result'. Wenn set1, set2, oder result in 
 * ihrem Typ verschieden sind, so ist der Rueckgabewert 0 und 
 * result undefiniert. Ansonsten ist der Rueckgabewert 1. Die 
 * Position in result ist undefiniert, in set1 und set2 
 * unveraendert. 
 */

int	union_set (set1, set2, result)

set_t	*set1;
set_t	*set2;
set_t	*result;

   {
    int		i;

    if (set1->size != set2->size || result->size != set2->size)
	return (0);
    if (set1->start != set2->start || result->start != set2->start)
	return (0);
    for (i = 0; i < set1->size / 8 + 1; i++)
	result->field[i] = set1->field[i] | set2->field[i];
    result->pos = -1;
    return (1);
   }

/*------------------------ SECTION -----------------------------*/
/* Bildet die Schnittmenge aus set1 und set2. Das Resultat steht in 
 * der Menge 'result'. Wenn set1, set2, oder result in ihrem Typ 
 * verschieden sind, so ist der Rueckgabewert 0 und result 
 * undefiniert. Ansonsten ist der Rueckgabewert 1. Die Position in 
 * result ist undefiniert, in set1 und set2 unveraendert. 
 */

int	section (set1, set2, result)

set_t	*set1;
set_t	*set2;
set_t	*result;

   {
    int		i;

    if (set1->size != set2->size || result->size != set2->size)
	return (0);
    if (set1->start != set2->start || result->start != set2->start)
	return (0);
    for (i = 0; i < set1->size / 8 + 1; i++)
	result->field[i] = set1->field[i] & set2->field[i];
    result->pos = -1;
    return (1);
   }

/*------------------------ MINUS -------------------------------*/
/* Bildet die Restmenge aus set1 minus set2. Das Resultat steht in 
 * der Menge 'result'. Wenn set1, set2, oder result in ihrem Typ 
 * verschieden sind, so ist der Rueckgabewert 0 und result 
 * undefiniert. Ansonsten ist der Rueckgabewert 1. Die Position in 
 * result ist undefiniert, in set1 und set2 bleibt sie 
 * unveraendert. 
 */

int	minus (set1, set2, result)

set_t	*set1;
set_t	*set2;
set_t	*result;

   {
    int		i;

    if (set1->size != set2->size || result->size != set2->size)
	return (0);
    if (set1->start != set2->start || result->start != set2->start)
	return (0);
    for (i = 0; i < set1->size / 8 + 1; i++)
	result->field[i] = set1->field[i] & ~(set2->field[i]);
    result->pos = -1;
    return (1);
   }

/*------------------------ PRINT_SET ---------------------------*/
/* Gibt die Elemente der Menge set auf der geoeffneten Datei fp aus.
 */

void	print_set (fp, set)

FILE*	fp;
set_t	*set;

   {
    int		i;

#ifdef DEBUG
    fprintf (fp, "size: %4d  start: %4d  field: ", 
	     set->size, set->start);
    for (i = 0; i < set->size / 8 + 1; i++)
	fprintf (fp, "%4d", set->field[i]);
    fprintf (fp, "\n");
#endif

    for (i = 0; i < set->size; i++)
        if (set->field[i / 8] & bit[i % 8])
	    fprintf (fp, "%4d", i + set->start);
    fprintf (fp, "\n");
   }

/*----------------------- SPRINT_SET ---------------------------*/
/* Gibt die Elemente der Menge set auf einen String aus.
 */

int	sprint_set (buf, set)

char*	buf;
set_t	*set;

   {
    int		i;
    char	*p;

    p = buf;

#ifdef DEBUG
    p += SPRINTF ((p, "size: %4d  start: %4d  field: ", 
	          set->size, set->start));
    for (i = 0; i < set->size / 8 + 1; i++)
	p += SPRINTF ((p, "%4d", set->field[i]));
    p += SPRINTF ((p, "\n"));
#endif

    for (i = 0; i < set->size; i++)
        if (set->field[i / 8] & bit[i % 8])
	    p += SPRINTF ((p, "%4d", i + set->start));
    p += SPRINTF ((p, "\n"));
    return (p - buf);
   }

/*------------------------ NR_OF_EL ----------------------------*/
/* Bestimmt die Anzahl Elemente in der Menge set.
 */

int	nr_of_el (set)

set_t	*set;

   {
    int		i;
    int		b;
    int		nr = 0;

    for (i = 0; i < set->size / 8 + 1; i++)
	/* Idee: b &= (b -1) loescht least significant 1 in b */
	for (b = set->field[i]; b != 0; b &= (b - 1))
	    nr++;
    return (nr);
   }

/*------------------------ GET_FIRST ---------------------------*/
/* Liefert das erste Element aus der Menge set. Das ist stets das 
 * Element mit dem kleinsten Index. Die Position innerhalb der 
 * Menge wird vermerkt, so dass mit get_next() das jeweils naechste 
 * Element gelesen werden kann. Wenn kein Element in der Menge ist, 
 * wird -1 zurueckgegeben und die Position ist undefiniert. 
 */

int	get_first (set)

set_t	*set;

   {
    int		b = 0;

    while (b < set->size)
       {
	/* Byte ueberspringen, wenn ganz leer: */
	if (!set->field[b / 8])
	   {
	    b += 8;
	    continue;
	   }
	if (set->field[b / 8] & bit[b % 8])
	   {
	    set->pos = b;
	    return (set->start + b);
	   }
	else
	    b++;
       }
    set->pos = -1;
    return (-1);
   }

/*------------------------ GET_NEXT ----------------------------*/
/* Liefert das naechste Element aus der Menge set. Voraussetzung 
 * ist, dass die Position in der Menge durch vorherigen Aufruf von 
 * get_first() (und evtl. mehrmaligem get_next()) definiert ist. 
 * Insbesondere wird durch den Aufruf von Funktionen, die den 
 * Mengeninhalt veraendern, die Position undefiniert. Ruechgabewert 
 * ist das naechste Element, oder -1, wenn die Position vorher 
 * undefiniert war. 
 */

int	get_next (set)

set_t	*set;

   {
    int		b;

    if (set->pos == -1)
	return (-1);
    b = set->pos + 1;
    while (b < set->size)
       {
	/* Byte ueberspringen, wenn ganz leer: */
	if (!set->field[b / 8])
	   {
	    b += 8;
	    continue;
	   }
	if (set->field[b / 8] & bit[b % 8])
	   {
	    set->pos = b;
	    return (set->start + b);
	   }
	else
	    b++;
       }
    set->pos = -1;
    return (-1);
   }

/*------------------------ FLAT_SET ----------------------------*/

void	flat_set (set, flat)

set_t	*set;
char	*flat;

   {
    int		*intbuf;
    char	*charbuf;

    intbuf = (int *) flat;
    charbuf = (char *) (intbuf + 3);
    intbuf[0] = set->start;
    intbuf[1] = set->size;
    intbuf[2] = set->pos;
    memcpy (charbuf, set->field, set->size / 8 + 1);
   }

/*------------------------ UNFLAT_SET --------------------------*/

void	unflat_set (flat, set)

char	*flat;
set_t	*set;

   {
    int		*intbuf;
    char	*charbuf;

    intbuf = (int *) flat;
    charbuf = (char *) (intbuf + 3);
    set->start = intbuf[0];
    set->size = intbuf[1];
    set->pos = intbuf[2];
    memcpy (set->field, charbuf, set->size / 8 + 1);
   }

/*------------------------ SIZEOF_FLAT_SET ---------------------*/

int	sizeof_flat_set (set)

set_t	*set;

   {
    return (ALIGN (3 * sizeof (int) + set->size / 8 + 1));
   }

/*------------------------ ENDE ---------------------------------*/
