#ifndef _GTABLE_H_
#define _GTABLE_H_

#include <stdlib.h>
#include "wave.h"

/* Enum for sawtooth */
enum {SAW_DOWN, SAW_UP};

typedef struct gtable {
    double * samples; /* Ptr to array containing the waveform */
    unsigned long length; /* Excluding the guard point */
} GTABLE;

typedef struct t_tab_oscil {
    GTABLE * table;
    OSCIL osc;
    double tablen;
    double sizeovrsr;
} TOSCIL;

/* Defining a function pointer type */
typedef double (* TABFUNC) (TOSCIL * oscil, double freq);

void freeTable(GTABLE ** gTab);

GTABLE * sinetable(unsigned long length);
GTABLE * tritable(unsigned long length, unsigned long nharms);
GTABLE * squaretable(unsigned long length, unsigned long nharms);
GTABLE * sawtable(unsigned long length, unsigned long nharms, int UP);
GTABLE * pulsetable(unsigned long length, unsigned long nharms);

TOSCIL * oscil_t(double fs, double phase, GTABLE * gtable);
double tabtick(TOSCIL * oscil, double freq);
double tabitick(TOSCIL * oscil, double freq);

#endif
