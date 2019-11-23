#include "breakpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


BREAKPOINT * getBreakpoints(FILE *fp, unsigned long *psize) {
    int got;
    unsigned long npoints = 0, size=64; /* Line counter (0-indexed), Blocksize for each malloc call */
    double lasttime = 0.0;
    /* The breakpoint array that will be returned */
    BREAKPOINT *points = NULL;
    char line[80]; /* maximum string characters per line (sufficient to read data) */
    
    /* Check if file pointer is valid ie check for NULL - return NULL pointer if so */
    if(!fp) {
        return NULL;
    }
    
    /* Allocate first block of memory on the heap */
    points = (BREAKPOINT *) malloc(sizeof(BREAKPOINT) * size);
    /* Check for NULL */
    if(!points) {
        return NULL;
    }
    
    /* Run loop for getting data - fgets returns NULL at EOF */
    while(fgets(line, sizeof(line), fp)) {
        /* long float (double) needed in string format */
        got = sscanf(line, "%lf %lf", &points[npoints].time, &points[npoints].value);
        if(got < 0) { /* Line is empty - go to next line -- eof returned which is = -1 */
            continue;
        }
        else if(got == 0) {
            printf("Line %lu has non-numeric data\n", npoints + 1);
            break;
        }
        else if(got == 1) {
            printf("Incomplete breakpoint found at line %lu\n", npoints + 1);
            break;
        }
        if(points[npoints].time < lasttime) {
            printf("Data error at line %lu: time not increasing\n", npoints + 1);
            break;
        }
        lasttime = points[npoints].time;
        /* Check if the next line goes over the size of the current array */
        if(++npoints == size) {
            BREAKPOINT * tmp;
            size += npoints;
            /* Fill newly allocated memory with previous array and space for another block */
            /* NOTE: realloc frees the old block if necessary!! */
            tmp = (BREAKPOINT * ) realloc(points, sizeof(BREAKPOINT) * size);
            /* check for null */
            if(!tmp) {
                /* Release memory and return null to the caller */
                npoints = 0;
                free(points);
                points = NULL;
                /* Break from while loop */
                break;
            }
            points = tmp;
        }
    }
    if(npoints) {
        /* Update array size returned */
        *psize = npoints;
    }
    /* Return BREAKPOINT array address */
    return points;
}


BRKSTREAM * bps_init(FILE * fp, unsigned long fs) {
    BRKSTREAM * brkstream;
    BREAKPOINT * points;
    
    if(fs <= 0) {
        fprintf(stderr, "Sample rate must be positive\n");
        return NULL;
    }
    /* Allocate HEAP memory for breakstream object */
    brkstream = (BRKSTREAM *) malloc(sizeof(BRKSTREAM));
    if(!brkstream) {
        fprintf(stderr, "Cannot allocate memory for breakstream object\n");
        return NULL;
    }

    /* Read breakpoints */
    if(!(points = getBreakpoints(fp, &brkstream->npoints))) {
        free(brkstream);
        fprintf(stderr, "Cannot read breakpoints from file!\n");
        return NULL;
    }

    if(brkstream->npoints < 2) {
        fprintf(stderr, "Breakpoint file is too small - at least two points required");
        free(brkstream);
        return NULL;
    }
    
    /* initialise the streaming object */
    brkstream->curpos = 0;
    brkstream->ileft = 0;
    brkstream->iright = 1;
    brkstream->more_points = brkstream->npoints > 2;
    brkstream->leftPoint = points[brkstream->ileft];
    brkstream->rightPoint = points[brkstream->iright];
    brkstream->width = brkstream->rightPoint.time - brkstream->leftPoint.time;
    brkstream->height = brkstream->rightPoint.value - brkstream->leftPoint.value;
    brkstream->incr = 1.0/fs;  /* time increment per breakpoint time */
    brkstream->points = points;
    
    return brkstream;
}

void bps_freepoints(BRKSTREAM * stream) {
    if(stream && stream->points) {
        free(stream->points);
        /* Must now reassign pointer to NULL since reference to it still exists
         This prevents the function from crashing if it is called repeatedly on the same
         BRKSTREAM object. */
        stream->points = NULL;
    }
}

double bps_tick(BRKSTREAM * stream) {
    /* Returns a tick from a breakpoint stream, linearly interpolating between points where necessary */
    double thisval, frac;
    /* Keep returning last value if no more points are available in the breakstream */
    if(!stream->more_points) {
        return stream->rightPoint.value;
    }
    /* For discontinuities at the same time point, return the leftmost value */
    if(!stream->width) {
        thisval = stream->leftPoint.value;
    }
    else {
        /* get value from the current span using linear interpolation) - y = mx + c
         The gradient here is stream->height/stream->width, whereas the difference is
         the offset (x)
         */
        frac = (stream->curpos - stream->leftPoint.time)/stream->width;
        thisval = stream->leftPoint.value + (stream->height * frac);
    }
    /* Go to next time point */
    stream->curpos += stream->incr;
    /* Do we need to move to the next span? */
    if(stream->curpos > stream->rightPoint.time) {
        stream->ileft++;
        stream->iright++;
        if(stream->iright < stream->npoints) {
            /* Update span variables */
            stream->leftPoint = stream->rightPoint;
            stream->rightPoint = stream->points[stream->iright];
            stream->height = stream->rightPoint.value - stream->leftPoint.value;
            stream->width = stream->rightPoint.time - stream->leftPoint.time;
        }
        else {
            stream->more_points = 0;
        }
    }
    return thisval;
}

void bps_getminmax(BRKSTREAM * stream, double *minval, double *maxval) {
    double min, max;
    min = max = stream->points[0].value;
    int i = 1;
    for (; i < stream->npoints; i++) {
        if(stream->points[i].value > max) {
            max = stream->points[i].value;
        }
        if(stream->points[i].value < min) {
            min = stream->points[i].value;
        }
    }
    *minval = min;
    *maxval = max;
}

int inRange(const BREAKPOINT * points,
            double minVal,
            double maxVal,
            unsigned long npoints) {
    unsigned long i;
    int range_OK = 1;
    for (i = 0; i < npoints; i++) {
        if(points[i].value < minVal || points[i].value > maxVal) {
            range_OK = 0;
            break;
        }
    }
    return range_OK;
}
