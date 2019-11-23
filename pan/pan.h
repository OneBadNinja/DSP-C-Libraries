#ifndef _PAN_H_
#define _PAN_H_


#include "breakpoint.h"

typedef struct panpos {
    double left;
    double right;
} PANPOS;

PANPOS simplepan(double position);

double dynamicPanner(BREAKPOINT ** pnts, BREAKPOINT * finAddr, double time);

void stereoPan(float *inBuffer, float *outBuffer, int inBuffsize, PANPOS fac);

PANPOS constPower(double position);

#endif
