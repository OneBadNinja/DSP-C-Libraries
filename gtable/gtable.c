#include "gtable.h"
#include <math.h>
#include <stdio.h>

/*
 Static functions can only be seen in their own source/object files.
 "A static function in C is a function that has a scope that is limited to its object file. This means that the static function is only visible in its object file."
 https://www.tutorialspoint.com/static-functions-in-c
 Static functions MUST have both declarations/prototypes and implementations with the same file.
 */

/**
 * Normalises a table by its maximum amplitude
 * @param table - pointer to a GTABLE object
 */
static void normtable(GTABLE * table);

/**
 * Creates and initialises a lookup table with a guard point
 * @param length - the size of the table
 * @return Pointer to a GTABLE object allocated on the heap
*/
static GTABLE * newtable(unsigned long length);

/**
 * Allocates memory for an initialised GTABLE and populates it with zeros for a given length. Includes a guard point.
 *  @param table - The initialised table
 *  @param length - The size of the table
 *  @return boolean integer whether population is successful or not.
*/
static int filltable(GTABLE * table, unsigned long length);

static GTABLE * newtable(unsigned long length) {
    unsigned long i;
    GTABLE * table = NULL;
    if(length <= 0) {
        return NULL;
    }
    table = (GTABLE *) malloc(sizeof(GTABLE));
    if(!table) {
        return NULL;
    }
    /* Allocate memory for waveform samples including guard point */
    table->samples = (double *) malloc(sizeof(double) * (length+1));
    if(!(table->samples)) {
        free(table);
        return NULL;
    }
    table->length = length;
    /* Initialise table with zeros - including guard point */
    for(i = 0; i <= length; i++) {
        table->samples[i] = 0.0;
    }
    return table;
}

int filltable(GTABLE * table, unsigned long length) {
    unsigned long i;
    if(table->samples) {
        return 0;
    }
    if(length <= 0) {
        return 0;
    }
    table->length = length;
    table->samples = (double *) malloc(sizeof(double) * (length + 1));
    if(!table->samples) {
        return 0;
    }
    /* Include guard point in initialisation */
    for(i = 0; i <= length; i++) {
        table->samples[i] = 0.0;
    }
    return 1;
}

static void normtable(GTABLE * table) {
    double maxamp = 0.0, amp;
    unsigned long i;
    for(i = 0; i < table->length; i++) {
        amp = fabs(table->samples[i]);
        if(amp > maxamp) {
            maxamp = amp;
        }
    }
    maxamp = 1.0/maxamp;
    for(i = 0; i < table->length; i++) {
        table->samples[i] *= maxamp;
    }
    /* Add guard point */
    table->samples[i] = table->samples[0];
}

void freeTable(GTABLE ** gTab) {
    /* -> has higher precedence than dereference * */
    if(gTab && *gTab && (*gTab)->samples) {
        /* Free the internal table memory */
        free((*gTab)->samples);
        /* Free the table generator object memory */
        free(*gTab);
        /* Pointer gTab still holds the address of the table, so make it a null pointer */
        *gTab = NULL;
    }
}

GTABLE * sinetable(unsigned long length) {
    unsigned long i;
    double step;
    GTABLE * tabG = NULL;

    tabG = newtable(length);
    if(!tabG) {
        return NULL;
    }
    
    /* Sine increment 2p/Len (max resolution) */
    step = 8.0*atan(1.0) / (double) length;
    
    /* Fill the table with the sinusoid */
    for(i = 0; i < length ; i++) {
        tabG->samples[i] = sin(i * step);
    }

    /* Add final guard point */
    tabG->samples[i] = tabG->samples[0];
    
    return tabG;
}

GTABLE * tritable(unsigned long length, unsigned long nharms) {
    unsigned long i, j, harmonic=1;
    double step, amp;
    GTABLE * tabG = NULL;
    /* Last condition to check for aliasing. Since we are using odd harmonics,
     we need to ensure that the harmonic factor (2k-1) < length/2 */
    if(nharms <= 0 || (2*nharms-1) >= length/2) {
        return NULL;
    }
    /* If a single harmonic, simply return a sinusoid */
    if(nharms == 1) {
        return sinetable(length);
    }
    
    tabG = newtable(length);
    if(!tabG) {
        return NULL;
    }

    /* TWOPI/Length step - this is the fundamental frequency resolution */
    step = 8.0*atan(1.0)/(double) length;
    
    /* Create triangle wave */
    for (i = 0; i <= nharms; i++) {
        amp = 1.0/(double) (harmonic * harmonic);
        for(j = 0; j < length; j++) {
            /* Harmonic is simply a frequency multiple here. fs is not yet being
             taken into account - fs is taken into account for the oscillator. This is
             simply a table of N values with M harmonics. the rate of oscillation is
             irrelevant (where fs comes into play)
             The harmonic frequency here is in the form of an odd multiple factor
             which advances at a faster rate than the fundamental frequency (harm = 1) */
            tabG->samples[j] += amp*cos(step*j*harmonic);
        }
        /* We are concerned only with odd harmonics - simple efficient sum rather than use formula
         (2k - 1) which has two operations (subtraction & multiplication) */
        harmonic += 2;
    }
    /* Normalise table and add guard point */
    normtable(tabG);
    return tabG;
}

GTABLE * squaretable(unsigned long length, unsigned long nharms) {
    unsigned long i, j, harmonic = 1;
    double amp, step;
    GTABLE * table = NULL;
    if(nharms <= 0 || (2*nharms - 1) >= length/2) {
        return NULL;
    }
    /* Create and initialise GTABLE object/structure */
    table = newtable(length);
    if(!table) {
        return NULL;
    }
    step = 8.0*atan(1.0)/(double) length;
    for(i = 0; i < nharms; i++) {
        amp = 1.0/(double) harmonic;
        for(j = 0; j < length; j++) {
            table->samples[j] += amp * sin(step * j * harmonic);
        }
        harmonic += 2;
    }
    
    /* normalise & include guard point */
    normtable(table);
    return table;
}

GTABLE * sawtable(unsigned long length, unsigned long nharms, int UP) {
    unsigned long i, j, harmonic=1;
    double amp, step;
    double fac = 1.0;
    GTABLE * table = NULL;
    if(nharms <= 0 || nharms >= length / 2) {
        return NULL;
    }
    table = newtable(length);
    if(!table) {
        return NULL;
    }
    if(UP) {
        fac = -1.0;
    }
    step = 8.0*atan(1.0)/(double) length;
    /* Fill in values */
    for (i = 0; i < nharms; i++) {
        amp = fac/(double) harmonic;
        for(j = 0; j < length; j++) {
            table->samples[j] += amp * sin(step * harmonic * j);
        }
        harmonic++;
    }
    
    normtable(table);
    return table;
}

GTABLE * pulsetable(unsigned long length, unsigned long nharms) {
    unsigned long i, j, harmonic = 1;
    double step;
    GTABLE * ptab = NULL;
    if(nharms <= 0 || nharms >= length / 2) {
        return NULL;
    }
    ptab = newtable(length);
    if(!ptab) {
        return NULL;
    }
    step = 8.0*atan(1.0)/(double) length;
    for (i = 0; i < nharms; i++) {
        for (j = 0; j < length; j++) {
            ptab->samples[j] += sin(step * j * harmonic);
        }
        harmonic ++;
    }
    normtable(ptab);
    return ptab;
}

TOSCIL * oscil_t(double fs, double phase, GTABLE * gtable) {
    /* Comprised of a lookup table and an oscillator object. */
    TOSCIL * oscil;
    
    if(!(gtable && gtable->samples && gtable->length > 0)) {
        return NULL;
    }
    
    oscil = (TOSCIL *) malloc(sizeof(TOSCIL));
    if(!oscil) {
        return NULL;
    }
    

    /* Assign internal oscillator parameters */
    oscil->osc.curFreq = 0.0;
    oscil->tablen = (double) gtable->length;
    /* Phase here expressed as a proportion of length */
    /*
     This is valid since we are setting the offset phase for a
     sinusoid of any frequency (The rate of cycling through is
     independent of the initial phase)
     
     Phase is hence in table length units
     */
    oscil->osc.curPhase = degree2rad(phase) * gtable->length / (2*PI);
    /* Wrap around if necessary - phase is truncated for truncated lookup */
    oscil->osc.curPhase = fmod(oscil->osc.curPhase, oscil->tablen);
    if(oscil->osc.curPhase < 0) {
        oscil->osc.curPhase += oscil->tablen;
    }
    oscil->osc.incr = 0.0;
    
    /* Gtable specifics */
    oscil->table = gtable;
    oscil->sizeovrsr = oscil->tablen/fs;
    return oscil;
}

double tabtick(TOSCIL * oscil, double freq) {
    /* Truncate phase index - equivalent to floor() in the positive direction
     for negative numbers
     floor(4.5) = (int)4.5 = 4
     floor(4.7) = (int)4.7 = 4
     floor(-4.5) = -5
     (int)(-4.5) = -4
     */
    unsigned long idx = (unsigned long) oscil->osc.curPhase; /* Phase is in table length units */

    /* Update frequency */
    if(oscil->osc.curFreq != freq) {
        /* Lowest frequency for increment at fundamental frequency resolution is obtained
         from the unit index increment TABLEN * freq/fs =  1 hence f0 = fs / length
         which is the same as FFT resolution (fs/NFFT)
         */
        oscil->osc.curFreq = freq;
        /* Update the sample_increment */
        oscil->osc.incr = oscil->sizeovrsr * oscil->osc.curFreq;
    }
    /* Remember, phase is in terms of proportion of length */
    oscil->osc.curPhase += oscil->osc.incr;
    /*
    THIS IS NOT AS FAST AS USING WHILE LOOP -- even for larger phases??
    oscil->osc.curPhase = fmod(oscil->osc.curPhase, oscil->tablen);
    if(oscil->osc.curPhase < 0) {
        fprintf(stdout, "YES\n");
        oscil->osc.curPhase += oscil->tablen;
    } */
    while(oscil->osc.curPhase >= oscil->tablen) {
        oscil->osc.curPhase -= oscil->tablen;
    }
    while(oscil->osc.curPhase < 0) {
        oscil->osc.curPhase += oscil->tablen;
    }

    /* Return the table value at the current index - obtained at the start prior to update */
    return oscil->table->samples[idx];
}

double tabitick(TOSCIL * oscil, double freq) {
    /* Retrieve the current index (truncated) - floor for positive, ceil for negative (latter
     not applicable here since we wrap between 0 < x < 2pi) */
    unsigned long base_idx = (unsigned int) oscil->osc.curPhase;
    double frac, slope, val;

    if(oscil->osc.curFreq != freq) {
        /* Update frequency and increment */
        oscil->osc.curFreq = freq;
        oscil->osc.incr = oscil->osc.curFreq * oscil->sizeovrsr;
    }
    /* Retrieve truncated value */
    val = oscil->table->samples[base_idx];

    /* proportion for interpolation (will be < 1 since indices are units) */
    frac = oscil->osc.curPhase - (double) base_idx;
    /* gradient over unit width - takes into account the guard point too */
    slope = oscil->table->samples[base_idx + 1] - val;
    /* Add the interpolated amount to the truncated value */
    val += frac * slope;
    
    /* Add another constant increment (which is fractional)
     Remember phase incr has no units here, it is a proportion of
     table length (fractional here)
     */
    oscil->osc.curPhase += oscil->osc.incr;
    while (oscil->osc.curPhase >= oscil->tablen) {
        oscil->osc.curPhase -= oscil->tablen;
    }
    while(oscil->osc.curPhase < 0) {
        oscil->osc.curPhase += oscil->tablen;
    }
    return val;
}
