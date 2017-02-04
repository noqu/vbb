#ifndef EMULATOR_PROCESS_H

#include <pthread.h>
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

