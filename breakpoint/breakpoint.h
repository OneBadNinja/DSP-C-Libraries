#ifndef _BREAKPOINT_H_
#define _BREAKPOINT_H_

#include <stdio.h>

typedef struct breakpoint {
    double time;
    double value;
} BREAKPOINT;

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

BREAKPOINT * getBreakpoints(FILE * file, unsigned long *psize);
BRKSTREAM * bps_init(FILE * fp, unsigned long fs);
void bps_freepoints(BRKSTREAM * stream);
double bps_tick(BRKSTREAM * stream);
void bps_getminmax(BRKSTREAM * stream, double *minval, double *maxval);

int inRange(const BREAKPOINT * points,
            double minVal,
            double maxVal,
            unsigned long npoints);
#endif
