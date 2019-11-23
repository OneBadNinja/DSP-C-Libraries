#ifndef _WAVE_H_
#define _WAVE_H_
#define PI (4.0 * atan(1.0))

typedef struct t_oscil {
    double twopiovrsr;
    double curFreq;
    double curPhase;
    double incr;
    double time;
    double mod;
} OSCIL;

/* Defining a function pointer type */
typedef double (*tickfunc) (OSCIL *, double);

OSCIL * oscil(double fs, double phase);
double sinetick(OSCIL * oscil, double freq);
double squaretick(OSCIL * oscil, double freq);
double sawdtick(OSCIL * oscil, double freq);
double sawutick(OSCIL * oscil, double freq);
double tritick(OSCIL * oscil, double freq);
double degree2rad(double deg);

enum {SINE, SQUARE, DSAW, USAW, TRI, PULSE};
enum {BIT16, BIT24, BIT32};
#endif
