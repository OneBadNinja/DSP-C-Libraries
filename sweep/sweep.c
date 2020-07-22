#include "sweep.h"
#include <math.h>
#include <stdlib.h>

ESS * newSweep(double T, double fs, double startFreq, double endFreq) {
    unsigned int i;
    double pi = 4.0 * atan(1.0);
    double dT = 1.0/fs;
    double Ka, K, L;
    ESS * sweep;
    
    /* Allocate memory for sweep object */
    sweep = (ESS *) malloc(sizeof(ESS));
    
    sweep->size = (unsigned long) T * fs;

    /* Allocate memory for sweep buffers */
    sweep->forward = (double *) malloc(sizeof(double) * (sweep->size));
    sweep->inverse = (double *) malloc(sizeof(double) * (sweep->size));

    /* Constant definitions */
    K = (T * 2 * pi * startFreq) / log(endFreq/startFreq);
    L = (T / log(endFreq/startFreq));
    Ka = log(pow(10.0, -6*log2(endFreq/startFreq)/20));
    
    /* Compute forward sweep */
    for (i = 0; i < sweep->size; i++) {
        sweep->forward[i] = sin(K * (exp(i*dT/L) - 1));
    }

    /* Compute inverse sweep */
    for (i = 0; i < sweep->size; i++) {
        double amp = exp(i * dT * Ka/T);
        sweep->inverse[i] = amp * sweep->forward[sweep->size-1-i];
    }

    return sweep;
}

void clearSweep(ESS ** sweep) {
    if(*sweep) {
        free((*sweep)->forward);
        free((*sweep)->inverse);
        free(*sweep);
    }
}
