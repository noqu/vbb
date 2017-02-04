#include "channel.h"
#include <stdlib.h>
#include <string.h>

// Emulation of transputer channels by means of named pipes. The pipes are expected to exist.
// The transputer's emulated main() will open 4 IN and 4 OUT channels and present them as
// params. A transputer 17 will expect its out channel 2 as <channelDir>/tr17_ch2_out.

// Open a channel (called by the emulation only)
Channel* ChanOpen(const char* channelDir, int transputerId, int channelId, Direction direction) {
    // Create the channel object
    Channel* c = (Channel*) malloc(sizeof(Channel));
    if (c == NULL) {
	return NULL;
    }
    c->direction = direction;

    // Construct the file name, like 'tr_0_ch_3_in'
    c->filename = (char*) malloc(strlen(channelDir) + 32);
    if (c->filename == NULL) {
	return NULL;
    }
    sprintf(c->filename, "%s/tr_%d_ch_%d_%s", channelDir, transputerId,
	    channelId, direction == IN ? "in" : "out");

    // Note that named pipes have a very peculiar semantics regarding open: When opened only for
    // reading or only for writing, the open() call will block until a partner becomes available.
    // Very weird - this forces us to defer the actual opening until the first read/write operation.
    c->fp = NULL;

    return c;
}

// Send one 4-byte integer along the channel
void ChanOutInt(Channel *c, int v) {
    ChanOut(c, &v, sizeof(int));
}

// Send 'length' bytes starting at 'addr' along the channel
void ChanOut(Channel *c, void *addr, int length) {
    // (Deferred from ChanOpen, see there): Open the named pipe if required.
    if (c->fp == NULL) {
	c->fp = fopen(c->filename, "wb");
	if (c->fp == NULL) {
	    fprintf(stderr, "Emulation error: Cannot open named pipe %s\n", c->filename);
	    perror("Aborting");
	    exit(1);
	}
    }

    // Write the bytes
    while (fwrite(addr, length, 1, c->fp) != 1) {
	// empty
    };
    fflush(c->fp);
}

// Receive one 4-byte integer from the channel and return it
int ChanInInt(Channel *c) {
    int result = 0;
    ChanIn(c, &result, sizeof(int));
    return result;
}

// Receive 'length' bytes from the channel and store them at 'addr'
void ChanIn(Channel *c, void *addr, int length) {
    // (Deferred from ChanOpen, see there): Open the named pipe if required.
    if (c->fp == NULL) {
	c->fp = fopen(c->filename, "rb");
	if (c->fp == NULL) {
	    fprintf(stderr, "Emulation error: Cannot open named pipe %s\n", c->filename);
	    perror("Aborting");
	    exit(1);
	}
    }

    while (fread(addr, length, 1, c->fp) != 1) {
        // empty
    };
}

