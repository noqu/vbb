/*---------------------------------------------------------------*/
/* set.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 1							 */
/*---------------------------------------------------------------*/
/* Deklarationen fuer set.c
 */
/*------------------------ GLOBALE VEREINBARUNGEN ---------------*/

#ifndef SET_H
#define SET_H

#ifndef public
#define         public
#endif

typedef struct set_st
   {
    unsigned char	*field;
    int			size;
    int			start;
    int			pos;
   } set_t;

/*------------------------ FUNKTIONEN ---------------------------*/
#ifdef SUN_CC

extern int	create_set ();
extern void	flush_set ();
extern int	copy_set ();
extern int	is_in ();
extern int	is_subset ();
extern int	is_empty ();
extern int	is_equal ();
extern int	add_elem ();
extern int	del_elem ();
extern int	union_set ();
extern int	section ();
extern int	minus ();
extern void	print_set ();
extern int	sprint_set ();
extern int	nr_of_el ();
extern int	get_first ();
extern int	get_next ();
extern void	flat_set ();
extern void	unflat_set ();
extern int	sizeof_flat_set ();

#else

extern int	create_set (
			int	/* start */, 
			int	/* size */, 
			set_t**	/* set */);

extern void	flush_set (
			set_t*	/* set */);

extern int	copy_set (
			set_t*	/* set1 */, 
			set_t*	/* set2 */);

extern int	is_in (
			int	/* elem */, 
			set_t*	/* set */);

extern int	is_subset (
			set_t*	/* set1 */, 
			set_t*	/* set2 */);

extern int	is_empty (
			set_t*	/* set */);

extern int	is_equal (
			set_t*	/* set1 */, 
			set_t*	/* set2 */);

extern int	add_elem (
			int	/* elem */, 
			set_t*	/* set */, 
			set_t*	/* result */);

extern int	del_elem (
			int	/* elem */, 
			set_t*	/* set */, 
			set_t*	/* result */);

extern int	union_set (
			set_t*	/* set1 */, 
			set_t*	/* set2 */, 
			set_t*	/* result */);

extern int	section (
			set_t*	/* set1 */, 
			set_t*	/* set2 */, 
			set_t*	/* result */);

extern int	minus (
			set_t*	/* set1 */, 
			set_t*	/* set2 */, 
			set_t*	/* result */);

extern void	print_set (
			FILE*	/* fp */,
			set_t*	/* set */);

extern int	sprint_set (
			char*	/* buf */,
			set_t*	/* set */);

extern int	nr_of_el (
			set_t*	/* set */);

extern int	get_first (
			set_t*	/* set */);

extern int	get_next (
			set_t*	/* set */);

extern void	flat_set (
			set_t*	/* set */,
			char*	/* flat */);

extern void	unflat_set (
			char*	/* flat */,
			set_t*	/* set */);

extern int	sizeof_flat_set (
			set_t*	/* set */);
#endif

#endif /* SET_H */
/*------------------------ ENDE ---------------------------------*/
