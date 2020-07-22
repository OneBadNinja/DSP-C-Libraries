#include "wave.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* Constructor */
OSCIL * oscil( double fs, double phase) {
    OSCIL * osc = (OSCIL *) malloc(sizeof(OSCIL));
    if(!osc) {
        return NULL;
    }
    osc->twopiovrsr = 2 * PI / fs;
    osc->curPhase = phase;
    osc->curFreq = 0.0;
    osc->incr = 0.0;
    osc->mod = 0.5;
    return osc;
}

double sinetick(OSCIL * oscil, double freq) {
    double val = sin(oscil->curPhase);
    // Update oscil object.
    if(freq != oscil->curFreq) {
        oscil->incr = oscil->twopiovrsr * freq;
        oscil->curFreq = freq;
    }
    oscil->curPhase += oscil->incr;
    return val;
}

double squaretick(OSCIL * oscil, double freq) {
    /* */
    double val;
    // Update oscil object.
    if(freq != oscil->curFreq) {
        oscil->incr = oscil->twopiovrsr * freq;
        oscil->curFreq = freq;
    }
    if(oscil->curPhase >= 2 * PI) {
        oscil->curPhase -= 2 * PI;
    }
    if(oscil->curPhase < 0.0) {
        oscil->curPhase += 2 * PI;
    }
    val = (oscil->curPhase <= 2 * PI * oscil->mod) ? 1.0 : -1.0;
    oscil->curPhase += oscil->incr;
    return val;
}

double sawdtick(OSCIL * oscil, double freq) {
    double val;
    // Update oscil object.
    if(freq != oscil->curFreq) {
        oscil->incr = oscil->twopiovrsr * freq;
        oscil->curFreq = freq;
    }
    if(oscil->curPhase >= 2 * PI) {
        oscil->curPhase -= 2 * PI;
    }
    if(oscil->curPhase < 0.0) {
        oscil->curPhase += 2 * PI;
    }
    /* Linear formula - negative gradient */
    val = 1.0 - 2.0 * (oscil->curPhase * (1.0/(2 * PI)));
    oscil->curPhase += oscil->incr;
    return val;
}

double sawutick(OSCIL * oscil, double freq) {
    return -1.0*sawdtick(oscil, freq);
}

double tritick(OSCIL * oscil, double freq) {
    double val;
    // Update oscil object.
    freq *= .5;
    if(freq != oscil->curFreq) {
        oscil->incr = oscil->twopiovrsr * freq;
        oscil->curFreq = freq;
    }
    if(oscil->curPhase >= 2 * PI) {
        oscil->curPhase -= 2 * PI;
    }
    if(oscil->curPhase < 0.0) {
        oscil->curPhase += 2 * PI;
    }
    /* rectified sawtooth */
    val = sawutick(oscil, freq);
    if(val < 0.0) {
        val *= -1;
    }
    val = 2.0 * (val - 0.5);
    oscil->curPhase += oscil->incr;
    return val;
}

double degree2rad(double deg) {
    return PI * deg/180.0;
}
