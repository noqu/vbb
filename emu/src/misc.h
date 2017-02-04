#ifndef EMULATOR_MISC_H

// Initialize the emulation. Must be called (early) once per transputer
void emulation_init(int ac, char** av);

// Retrieves a parameter from the transputer configuration
void* get_param(int index);

// Exit the currently executing transputer program and make the server exit with status 'n'
void exit_terminate(int n);

#define EMULATOR_MISC_H
#endif // EMULATOR_MISC_H


