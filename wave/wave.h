#ifndef _WAVE_H_
#define _WAVE_H_
#define PI (4.0 * atan(1.0))

/**
 * Oscillator type enumeration for the currently supported wave types.
 */
enum {SINE, SQUARE, DSAW, USAW, TRI, PULSE};

/**
 * Bit depth enumeration for the currently supported bit-depth formats.
 */
enum {BIT16, BIT24, BIT32};

/**
 * A structure for an oscillator object.
 *
 * @param twopiovrsr - Constant 2pi / sample rate
 * @param curFreq - the instantaneous frequency for the oscillator
 * @param curPhase - the current phase for the oscillator (radians)
 * @param incr - the phase increment (curFreq * twopiovrsr)
 * @param time - not in use
 * @param mod - dynamic pulse-width-modulation value (in fractional form - 0.01 - 0.99). Only used
 * for square waveforms, where a normal square wave has a value of 0.5.
*/
typedef struct t_oscil {
    double twopiovrsr;
    double curFreq;
    double curPhase;
    double incr;
    double time;
    double mod;
} OSCIL;

/**
 * Define a function pointer for the type of waveform tick.
 *
 * @param oscil - The pointer to an OSCIL object
 * @param freq - The instantaneous frequency for the tick
 * @return the current waveform tick value as a double.
*/
typedef double (*tickfunc) (OSCIL * oscil, double freq);

/**
 * Create and initialise an OSCIL object by dynamically allocating memory on the HEAP.
 *
 * @param fs - The sample rate for the oscillator.
 * @param phase - The initial phase (radians)
 * @return a pointer to an initialised OSCIL object.
 */
OSCIL * oscil(double fs, double phase);

/**
 * Perform a sinusoidal tick for a given oscillator and instantaneous frequency.
 *
 * Utilises trigonometric functions, and is therefore less efficient than a lookup table.
 *
 * @param oscil - Pointer to an initialised OSCIL object
 * @param freq - The instantaeous frequency for the tick
 * @return the current sinusoidal tick value.
 */
double sinetick(OSCIL * oscil, double freq);

/**
 * Perform a square-wave tick for a given oscillator and instantaneous frequency.
 *
 * Uses time domain construction. Less efficient than a predefined lookup table. Prone to aliasing.
 *
 * @param oscil - Pointer to an initialised OSCIL object
 * @param freq - The instantaeous frequency for the tick
 * @return the current square tick value.
 */
double squaretick(OSCIL * oscil, double freq);

/**
 * Perform a downward-saw tick for a given oscillator and instantaneous frequency.
 *
 * Uses time domain construction. Less efficient than a lookup table. Prone to aliasing.
 *
 * @param oscil - Pointer to an initialised OSCIL object
 * @param freq - The instantaeous frequency for the tick
 * @return the current downward-saw tick value.
 */
double sawdtick(OSCIL * oscil, double freq);

/**
 * Perform an upward-saw tick for a given oscillator and instantaneous frequency.
 *
 * Uses time domain construction. Less efficient than a lookup table. Prone to aliasing.
 *
 * @param oscil - Pointer to an initialised OSCIL object
 * @param freq - The instantaeous frequency for the tick
 * @return the current upward-saw tick value.
 */
double sawutick(OSCIL * oscil, double freq);

/**
 * Perform a triangle-wave tick for a given oscillator and instantaneous frequency.
 *
 * Uses time domain construction. Less efficient than a lookup table. May be prone to aliasing.
 *
 * @param oscil - Pointer to an initialised OSCIL object
 * @param freq - The instantaeous frequency for the tick
 * @return the current triangle-wave tick value.
 */
double tritick(OSCIL * oscil, double freq);

/**
 * Performs degree to radian conversion
 *
 * @param deg - The phase value in degrees
 * @return the corresponding phase value in radians
 */
double degree2rad(double deg);

#endif
