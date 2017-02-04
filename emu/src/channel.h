#ifndef EMULATOR_CHANNEL_H

#include <stdio.h>

// To distinguish in and out channels
typedef enum {
    IN,
    OUT
} Direction;

// A unidirectional communication channel endpoint
typedef struct {
    char* filename;
    FILE* fp;
    Direction direction;
} Channel;

// Open a channel (called by the emulation only)
Channel* ChanOpen(const char* channelDir, int transputerId, int channelId, Direction direction);

// Send one 4-byte integer along the channel
void ChanOutInt(Channel *c, int v);

// Send 'length' bytes starting at 'addr' along the channel
void ChanOut(Channel *c, void *addr, int length);

// Receive one 4-byte integer from the channel and store it at '*iptr'
int ChanInInt(Channel *c);

// Receive 'length' bytes from the channel and store them at 'addr'
void ChanIn(Channel *c, void *addr, int length);

#define EMULATOR_CHANNEL_H
#endif // EMULATOR_CHANNEL_H

