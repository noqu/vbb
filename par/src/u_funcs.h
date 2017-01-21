/*---------------------------------------------------------------*/
/* u_funcs.h							 */
/*---------------------------------------------------------------*/
/* Norbert Kuck							 */
/* Version 4.2		1.6.93 					 */
/*---------------------------------------------------------------*/
/* Vereinbarungen von Fehlernummern und Funktionstypen fuer
 * die Benutzerfunktionen. Generische Beschreibung, die von allen
 * Instanzen included werden muss.
 */
/*------------------------ PRAEPROZESSOR ------------------------*/

#ifndef  U_FUNCS_H
#define  U_FUNCS_H

#include  "error.h"

#define  U_INCOMPLETE_SOL	U_FUNCS_ERR + 1
#define  U_PARAM		U_FUNCS_ERR + 2
#define  U_FILE			U_FUNCS_ERR + 3
#define  U_MALLOC		U_FUNCS_ERR + 4

/*------------------------ FUNKTIONEN ---------------------------*/
#ifdef SUN_CC

extern boolean	u_first_child ();
extern boolean	u_is_feasible ();
extern boolean	u_next_child ();
extern int	u_heur_loes ();
extern int	u_init_glob ();
extern int	u_lower_bound ();
extern int	u_mk_start_node ();
extern int	u_print_sol ();
extern int	u_priority ();
extern int	u_solution ();
extern int	u_create_node ();
extern int	u_sizeof_flat_node ();
extern void	u_copy_node ();
extern void	u_flat_node ();
extern void	u_unflat_node ();
extern int	u_flat_glob ();
extern int	u_unflat_glob ();
extern char	*u_trace_node ();

#else

extern boolean	u_first_child (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* parent */, 
			u_node_t*	/* son */, 
			int*		/* selection */);

extern boolean	u_is_feasible (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */);

extern boolean	u_next_child (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* parent */, 
			u_node_t*	/* son */, 
			int*		/* selection */);

extern int	u_heur_loes (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */, 
		        u_node_t*	/* solution */, 
		        u_zf_t*		/* value */);

extern int	u_init_glob (
			u_glob_t**	/* u_glob */);

extern int	u_lower_bound (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */, 
			u_zf_t*		/* l_bound */);

extern int	u_mk_start_node (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* startnode */);

extern int	u_print_sol (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* sol */);

extern int	u_priority (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */, 
			int		/* para */,
			int*		/* priority */);

extern int	u_solution (
			u_glob_t*	/* u_glob */, 
			u_node_t*	/* node */, 
			u_zf_t*		/* value */);

extern int	u_create_node (
			u_glob_t*	/* u_glob */,
			int		/* anzahl */, 
			u_node_t**	/* nodes */);

extern int	u_sizeof_flat_node (
			u_glob_t*	/* u_glob */);

extern void	u_copy_node (
			u_glob_t*	/* u_glob */,
			u_node_t*	/* from */, 
			u_node_t*	/* to */);

extern void	u_flat_node (
			u_glob_t*	/* u_glob */,
			u_node_t*	/* node */, 
			char*		/* flat */);

extern void	u_unflat_node (
			u_glob_t*	/* u_glob */,
			char*		/* flat */, 
			u_node_t*	/* node */);

extern int	u_flat_glob (
			u_glob_t*	/* u_glob */,
			char**		/* flat */,
			int*		/* size */);

extern int	u_unflat_glob (
			char*		/* flat */,
			u_glob_t**	/* u_glob */);

extern char	*u_trace_node (
			u_node_t*	/* u_node */,
			char*		/* wp */);

#endif
#endif /* U_FUNCS_H */
/*------------------------ ENDE ---------------------------------*/
