#include "pan.h"
#include <stdio.h>
#include "breakpoint.h"
#include <math.h>

PANPOS simplepan(double position)
{
    /*
     A simple linear panner, converting a position from -1 to 1 to left and right pan amplitudes:
     
     ie:
        position = 0.75
        position * 0.5 = 0.375
        left = 0.375 - 0.5 = -1.25
        right = 0.375 + 0.5 = 0.875
     */
    PANPOS pos;
    position *= 0.5;
    pos.left = position - 0.5;
    pos.right = position + 0.5;
    return pos;
}

PANPOS constPower(double position) {
    PANPOS pos;
    double pi = 4 * atan(1.0);  // atan(1.0) gives 45 deg
    double theta = (position + 1) * pi * pow(0.5, 2.0);
    pos.left = cos(theta);
    pos.right = sin(theta);
    return pos;
}

double dynamicPanner(BREAKPOINT ** pnts, BREAKPOINT * finAddr, double time) {
    double m, pos;
    if(*pnts != finAddr) {
        // compute values here
        if(time >= (*pnts+1)->time) {
            (*pnts)++;
        }
        m = ((*pnts+1)->value - (*pnts)->value) / ((*pnts+1)->time - (*pnts)->time);
        pos = (time - (*pnts)->time) * m + (*pnts)->value;
        return pos;
    }
    else {
        return finAddr->value;
    }
}

void stereoPan(float *inBuffer, float *outBuffer, int inBuffsize, PANPOS fac)
/*
 Applies a stereo pan to a buffer.
 */
{
    int i, out_i;
    float val;
    // Go through each sample and pan across the output stereo file.
    for (i = 0, out_i = 0; i < inBuffsize; i++) {
        val = inBuffer[i];
        outBuffer[out_i++] = val * fac.left;
        outBuffer[out_i++] = val * fac.right;
    }
}
