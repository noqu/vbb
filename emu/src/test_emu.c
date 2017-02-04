#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "channel.h"
#include "process.h"
#include "misc.h"

// Thread to receive and print integers until it gets a 0
void receiver(Channel* c, intptr_t cpu_nr, int* finished) {
    while (!(*finished)) {
       int i = ChanInInt(c);
       fprintf(stderr, "[%d] Thread received %d\n", (int) cpu_nr, i);
       if (i == 0) {
           *finished = 1;
       }
    }
}

// Small test (expecting 2 transputers connected via link 0)

int main(int ac, char **av) {
    static Channel	**in_channel;
    static Channel	**out_channel;
    int			nr_of_chans;
    int			cpu_nr;
    int			nr_of_cpus;
    int			last_in_ring;

    // Initialize emulation
    emulation_init(ac, av);

    // Get parameters
    cpu_nr = *((int *) get_param (3));
    in_channel = get_param (4);
    out_channel = get_param (5);
    nr_of_chans = *((int *) get_param (6));
    nr_of_cpus = *((int *) get_param (7));
    last_in_ring = *((int *) get_param (8));

    // Root first sends a byte to partner, then receives the reply
    char i = 42;
    int len = 1;
    if (cpu_nr == 0) {
        fprintf(stderr, "[%d] Waiting to send\n", cpu_nr);
        ChanOut(out_channel[0], &i, len);
        fprintf(stderr, "[%d] Sent %d\n", cpu_nr, (int) i);
        fprintf(stderr, "[%d] Waiting to receive\n", cpu_nr);
        ChanIn(in_channel[0], &i, len);
        fprintf(stderr, "[%d] Received %d\n", cpu_nr, (int) i);
    } else {
        fprintf(stderr, "[%d] Waiting to receive\n", cpu_nr);
        ChanIn(in_channel[0], &i, len);
        fprintf(stderr, "[%d] Received %d\n", cpu_nr, (int) i);
        i++;
        fprintf(stderr, "[%d] Waiting to send\n", cpu_nr);
        ChanOut(out_channel[0], &i, len);
        fprintf(stderr, "[%d] Sent %d\n", cpu_nr, (int) i);
    }

    // Start receiver thread
    volatile int receiverFinished = 0;
    Process *p = ProcAlloc(receiver, 1024, 3, in_channel[0], (intptr_t) cpu_nr, &receiverFinished);
    if (p == NULL) {
        fprintf(stderr, "[%d] ProcAlloc failed\n", cpu_nr);
	exit(1);
    }
    ProcRun(p);
    fprintf(stderr, "[%d] Receiver started\n", cpu_nr);

    // Send a few integers to the partner (ending with 0)
    int n = 100 + 10 * cpu_nr;
    ChanOutInt(out_channel[0], n);
    fprintf(stderr, "[%d] Sent %d\n", cpu_nr, n);
    n++;
    ChanOutInt(out_channel[0], n);
    fprintf(stderr, "[%d] Sent %d\n", cpu_nr, n);
    n++;
    ChanOutInt(out_channel[0], n);
    fprintf(stderr, "[%d] Sent %d\n", cpu_nr, n);
    n++;
    ChanOutInt(out_channel[0], n);
    fprintf(stderr, "[%d] Sent %d\n", cpu_nr, n);
    n = 0;
    ChanOutInt(out_channel[0], n);
    fprintf(stderr, "[%d] Sent %d\n", cpu_nr, n);

    // Wait for receiver to finish
    while (!receiverFinished) {
        ProcReschedule();
    }
    fprintf(stderr, "[%d] Receiver finished\n", cpu_nr);
}
