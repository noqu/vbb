
// Emulation of the auxiliary transputer infrastructure (startup and
// configuration retrieval)

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "channel.h"
#include "read.h"
#include "error.h"

// In the original implementation, get_param(i) would retrieve the following 
// pieces of information (parked in the executable during configuration and
// linking)
#define TP_SELF 3
#define TP_CHANS_IN  4
#define TP_CHANS_OUT 5
#define TP_NUM_CHANS 6
#define TP_NUM_TPS   7
#define TP_RING_LAST 8

// Global data per transputer (maintained by the emulation)
typedef struct {
    int	tp_self;
    Channel* tp_chans_in[4];
    Channel* tp_chans_out[4];
    int tp_num_chans;
    int tp_num_tps;
    int tp_ring_last;
    pid_t tp_pgid;
} tp_t;
static tp_t tp;

// Report errors occurring in the emulation
static void fail(char* msg) {
    fprintf(stderr, "Emulation error[%d]: %s\n", tp.tp_self, msg);
    exit(1);
}

// Initialize the emulation. Must be called (early) once per transputer
void emulation_init(int ac, char** av) {
    char* param_buf; // apparently not used for anything?
    int sts;
    char *param;

    // Not yet known
    tp.tp_self = -1;

    // Initialize parameters
    sts = init_params(&param_buf, ac, av);
    if (sts != OK) {
        fail("Parameter initialization failed");
    }

    // TP_SELF
    if (!read_param("TP_SELF", &param)) {
        fail("Missing parameter TP_SELF");
    }
    tp.tp_self = atoi(param);
    if (tp.tp_self < 0) {
        fail("Invalid parameter TP_SELF");
    }

    // TP_NUM_CHANS
    if (!read_param("TP_NUM_CHANS", &param)) {
        fail("Missing parameter TP_NUM_CHANS");
    }
    tp.tp_num_chans = atoi(param);
    if (tp.tp_num_chans < 1 || tp.tp_num_chans > 4) {
        fail("Invalid parameter TP_NUM_CHANS");
    }

    // TP_NUM_TPS
    if (!read_param("TP_NUM_TPS", &param)) {
        fail("Missing parameter TP_NUM_TPS");
    }
    tp.tp_num_tps = atoi(param);
    if (tp.tp_num_tps < 0) {
        fail("Invalid parameter TP_NUM_TPS");
    }

    // TP_RING_LAST
    if (!read_param("TP_RING_LAST", &param)) {
        fail("Missing parameter TP_RING_LAST");
    }
    tp.tp_ring_last = atoi(param);
    if (tp.tp_ring_last < 0 || tp.tp_ring_last >= tp.tp_num_tps) {
        fail("Invalid parameter TP_RING_LAST");
    }

    // TP_PGID
    if (!read_param("TP_PGID", &param)) {
        fail("Missing parameter TP_PGID");
    }
    tp.tp_pgid = (pid_t) atoi(param);

    // TP_PIPE_DIR
    char* pipe_dir;
    if (!read_param("TP_PIPE_DIR", &pipe_dir)) {
        fail("Missing parameter TP_PIPE_DIR");
    }

    // IN-channels
    int i;
    for (i = 0; i < tp.tp_num_chans; i++) {
        tp.tp_chans_in[i] = ChanOpen(pipe_dir, tp.tp_self, i, IN);
	if (tp.tp_chans_in[i] == NULL) {
	    fail("Could not open IN channel");
	}
    }

    // OUT-channels
    for (i = 0; i < tp.tp_num_chans; i++) {
        tp.tp_chans_out[i] = ChanOpen(pipe_dir, tp.tp_self, i, OUT);
	if (tp.tp_chans_out[i] == NULL) {
	    fail("Could not open OUT channel");
	}
    }
}

// Retrieve a parameter (originally from the transputer configuration, now from
// the emulation)
void* get_param(int index) {
    switch(index) {
	case TP_SELF:
	    return &tp.tp_self;
	case TP_CHANS_IN:
	    return &tp.tp_chans_in[0];
	case TP_CHANS_OUT:
	    return &tp.tp_chans_out[0];
	case TP_NUM_CHANS:
	    return &tp.tp_num_chans;
	case TP_NUM_TPS:
	    return &tp.tp_num_tps;
	case TP_RING_LAST:
	    return &tp.tp_ring_last;
	default:
	    return NULL;
    }
}

// Exit the currently executing transputer program abruptly (originally also
// making the server exit with status 'n', but we don't bother emulating that)
void exit_terminate(int n) {
    fprintf(stderr, "exit_terminate called on TP %d\n", tp.tp_self);
    killpg(tp.tp_pgid, SIGTERM);
}

