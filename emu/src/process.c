#include "process.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <memory.h>

// Emulation of transputer "Processes" by means of pthreads (rather
// straightforward). 
// The only slight complication is the varargs pass-through from ProcAlloc to
// the started thread, which is not generically implementable in C without
// resorting to hackish tricks (the original transputer implementation must
// either have relied on everything being 4-byte sized or assembled the stack
// frame by hand).
// We solve it simply by adapting the callers to always pass 64-bit sized args.

// Create a new Process, with entry point 'func' and given stacksize. The
// 'nargs' pointer-sized arguments (at most MAX_ARGS) are passed to 'func' upon
// startup.
Process *ProcAlloc (void (*func)(), int stacksize, int nargs, ...) {
    // At most MAX_ARGS args
    if (nargs > MAX_ARGS) {
        fprintf(stderr, "ProcAlloc: too many arguments: %d\n", nargs);
	return NULL;
    }

    // Ignore stack size, values from transputer era are too small anyway

    // Create the Process object
    Process* p = (Process*) malloc(sizeof(Process));
    if (p == NULL) {
        perror("ProcAlloc: malloc failed");
	return NULL;
    }

    // Keep function and variable args there
    p->func = func;
    memset(&p->args, 0, MAX_ARGS * sizeof(void*));
    va_list varArgs;
    va_start(varArgs, nargs);
    int i;
    for (i = 0; i < nargs; i++) {
        p->args[i] = va_arg(varArgs, void*);
    }
    va_end(varArgs);

    return p;
}

// This is what the newly started thread will call as its main function
void* invokeMainFunction(void *_p) {
    Process* p = (Process*) _p;
    // FIXME: Somewhat ugly: Simply call the process's main function with MAX_ARGS
    // pointer size args - it does not harm to push more into the stack frame than
    // the callee will use
    p->func(p, p->args[0], p->args[1], p->args[2], p->args[3],
            p->args[4], p->args[5], p->args[6], p->args[7]);
}

// Start the Process 'p'
void ProcRun (Process *p) {
    // Plain vanilla thread attributes
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0) {
        perror("ProcAlloc: pthread_attr_init failed");
	free(p);
        return;
    }

    // Create and run a pthread for the Process object
    if (pthread_create(&p->thread, &attr, invokeMainFunction, p) != 0) {
        perror("ProcAlloc: pthread_create failed");
        return;
    }
}

// Yield the rest of the time-slice to another Process (i.e., thread)
void ProcReschedule() {
    pthread_yield();
}
