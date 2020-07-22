#ifndef _BREAKPOINT_H_
#define _BREAKPOINT_H_

#include <stdio.h>

/**
 * Define the schema for a breakpoint object.
 * @param time - the instantaneous time value
 * @param value - the instantaneous value
 */
typedef struct breakpoint {
    double time;
    double value;
} BREAKPOINT;

/**
 * Define the schema for a breakpoint stream object
 * @param points - pointer to hold a BREAKPOINT array.
 * @param leftPoint,rightPoint - current and previous breakpoint (span variables)
 * @param npoints - the number of breakpoints within the stream
 * @param curpos - the current position (index) within the breakpoitn stream
 * @param incr - time increment - equal to sampling period
 * @param width - time difference dT between adjacent breakpoints
 * @param height - value difference dX between adjacent breakpoints
 * @param ileft,right - indices for the current (right) and previous breakpoint
 * @param more_points - boolean integer indicating whether the end of the breakpoint stream has been reached.
 */
typedef struct breakpoint_stream {
    BREAKPOINT * points;
    BREAKPOINT leftPoint, rightPoint;
    unsigned long npoints;
    double curpos;
    double incr;
    double width;
    double height;
    unsigned long ileft, iright;
    int more_points;
} BRKSTREAM;

/**
 * Obtain a breakpoint array from a given breakpoint file. Returns NULL if unsuccessful.
 * @param file - pointer to the breakpoint file.
 * @param psize - pointer of type unsigned long which is populated by the breakpoint length
 * @return pointer to a dynamically allocated BREAKPOINT array.
 */
BREAKPOINT * getBreakpoints(FILE * file, unsigned long *psize);

/**
 * Initialise and return a given breakpoint stream object for a given breakpoint file and sample rate.
 * @param fp - pointer to the breakpoitn file
 * @param fs - the system sample rate
 * @return pointer to a dynamically allocated BRKSTREAM object
 */
BRKSTREAM * bps_init(FILE * fp, unsigned long fs);

/**
 * Free dynamically allocated memory for the BREAKPOINT array within a BRKSTREAM object.
 * @param stream - pointer to an initialised BRKSTREAM object
 */
void bps_freepoints(BRKSTREAM * stream);

/**
 * Advance through to the next tick within the breakpoint stream.
 *
 * Either returns a breakpoint's value or a linearly interpolated value between adjacent breakpoints.
 * Holds the final value if the end of the breakpoint stream is reached.
 *
 * @param stream - pointer to an initialised BRKSTREAM object
 * @return the current BRKSTREAM tick value.
 */
double bps_tick(BRKSTREAM * stream);

/**
 * Get the minimum and maximum value within an initialised BRKSTREAM object.
 *
 *  @param stream - pointer to an initialised BRKSTREAM object
 *  @param minval - pointer to a double which is filled with the minimum value
 *  @param maxval - pointer to a double which is filled with the maximum value
 */
void bps_getminmax(BRKSTREAM * stream, double *minval, double *maxval);

/**
 * Checks whether a BREAKPOINT array is within defined limits.
 *
 * @param points - pointer to a dynamically allocated BREAKPOINT array
 * @param minVal - the lower bound (minimum) to check against within the array
 * @param maxVal - the upper bound (maximum) to check against within the array
 * @param npoints - the size of the BREAKPOINT array.
 * @return a boolean integer, 1 if the array is within bounds, 0 if the array contains values out of bounds.
 */
int inRange(const BREAKPOINT * points,
            double minVal,
            double maxVal,
            unsigned long npoints);
#endif
