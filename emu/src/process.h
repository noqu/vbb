#ifndef EMULATOR_PROCESS_H

#include <pthread.h>
// OS X does not support pthread_yield(), only pthread_yield_np()
#if defined(__APPLE__) || defined(__MACH__)
#define pthread_yield() pthread_yield_np()
#endif

#include <stdarg.h>

#define MAX_ARGS 8

// A transputer process (more like a thread)
typedef struct {
    pthread_t thread;
    void* args[MAX_ARGS];
    void (*func)();
} Process;

// Create a new process, with entry point 'func' and given stacksize. The
// 'nargs' arguments are passed to 'func' upon startup.
Process *ProcAlloc (void (*func)(), int stacksize, int nargs, ...);

// Start the process 'p'
void ProcRun (Process *p);

// Yield the rest of the time-slice to another process
void ProcReschedule();

#define EMULATOR_PROCESS_H
#endif // EMULATOR_PROCESS_H

