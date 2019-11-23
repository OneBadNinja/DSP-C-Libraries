#ifndef _GEN_H_
#define _GEN_H_
#include <math.h>

typedef struct logSweep {
    double * forward;
    double * inverse;
    unsigned long size;
} ESS;

ESS * newSweep(double T, double fs, double startFreq, double endFreq);
void clearSweep(ESS ** sweep);

#endif _GEN_H_
