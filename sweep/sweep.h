#ifndef SWEEP_H
#define SWEEP_H
#include <math.h>

typedef struct logSweep {
    double * forward;
    double * inverse;
    unsigned long size;
} ESS;

ESS * newSweep(double T, double fs, double startFreq, double endFreq);
void clearSweep(ESS ** sweep);

#endif
