#ifndef _GTABLE_H_
#define _GTABLE_H_

#include <stdlib.h>
#include "wave.h"

/**
 * Sawtooth oscillator enumeration specifying the wave direction.
 */
enum {SAW_DOWN, SAW_UP};

/**
 * A structure definition for a look-up table.
 *
 * @param samples - pointer of type double to hold the table samples
 * @param length - the length of the table (not including guard point)
 */
typedef struct gtable {
    double * samples;
    unsigned long length;
} GTABLE;

/**
 * Defines the schema for a lookup table oscillator
 *
 * @param table - pointer to a GTABLE lookup table object
 * @param osc - the underlying OSCIL oscillator object.
 * @param tablen - the length of the table
 * @param sizeovrsr - phase constant for the lookup table based on table length.
 */
typedef struct t_tab_oscil {
    GTABLE * table;
    OSCIL osc;
    double tablen;
    double sizeovrsr;
} TOSCIL;

/**
 * Define a function pointer for the type of look-up table tick (interpolated or truncated)
 * @param oscil - pointer to a TOSCIL object
 * @param freq - the instantaneous frequency of oscillation
 * @return the current oscillator value as a double.
 */
typedef double (* TABFUNC) (TOSCIL * oscil, double freq);

/**
 * Destroy a dynamically allocated GTABLE lookup table object by freeing all memory
 * @param gTab - pointer to a pointer for thhe GTABLE object to be freed.
 */
void freeTable(GTABLE ** gTab);

/**
 * Create a sine-wave lookup table for a given length.
 * @param length - the sample size of the lookup oscillator
 * @return pointer to a GTABLE object allocated on the heap.
 */
GTABLE * sinetable(unsigned long length);

/**
 * Create a triangle-wave lookup table for a given length.
 * @param length - the sample size of the lookup oscillator
 * @param nharms - the number of harmonics to construct the waveform with. Supplying a value of 1 returns a sine wave.
 * @return pointer to a GTABLE object allocated on the heap.
 */
GTABLE * tritable(unsigned long length, unsigned long nharms);

/**
 * Create a square-wave lookup table for a given length.
 * @param length - the sample size of the lookup oscillator
 * @param nharms - the number of harmonics to construct the waveform with. Supplying a value of 1 returns a sine wave.
 * @return pointer to a GTABLE object allocated on the heap.
 */
GTABLE * squaretable(unsigned long length, unsigned long nharms);

/**
 * Create a sawtooth-wave lookup table for a given length.
 * @param length - the sample size of the lookup oscillator
 * @param nharms - the number of harmonics to construct the waveform with. Supplying a value of 1 returns a sine wave.
 * @param UP - boolean integer specifying the direction of the sawtooth wave. Suggested use is with the SAW_UP and SAW_DOWN enum constants.
 * @return pointer to a GTABLE object allocated on the heap.
 */
GTABLE * sawtable(unsigned long length, unsigned long nharms, int UP);

/**
 * Create a pulse-wave lookup table for a given length.
 * @param length - the sample size of the lookup oscillator
 * @param nharms - the number of harmonics to construct the waveform with. Supplying a value of 1 returns a sine wave.
 * @return pointer to a GTABLE object allocated on the heap.
 */
GTABLE * pulsetable(unsigned long length, unsigned long nharms);

/**
 * Create a TOSCIL lookup table oscillator object for a given GTABLE lookup table containing a predefined waveform.
 * @param fs - The sample rate of the system
 * @param phase - Starting phase offset (radians)
 * @param gtable - Pointer to a GTABLE lookup table.
 * @return pointer to a GTABLE object allocated on the heap.
 */
TOSCIL * oscil_t(double fs, double phase, GTABLE * gtable);

/**
 * Performs a truncated lookup for a given TOSCIL lookup table oscillator
 * @param oscil - pointer to a lookup table oscillator with a predefined waveform
 * @param freq - the instantaneous frequency of oscillation
 * @return the instantaneous truncated oscillation value
 */
double tabtick(TOSCIL * oscil, double freq);

/**
 * Performs an interpolated lookup for a given TOSCIL lookup table oscillator
 * @param oscil - pointer to a lookup table oscillator with a predefined waveform
 * @param freq - the instantaneous frequency of oscillation
 * @return the instantaneous interpolated oscillation value 
 */
double tabitick(TOSCIL * oscil, double freq);

#endif
