#ifndef FFTPROC_H_ID
#define FFTPROC_H_ID
#include <math.h>
#include <kiss_fft.h>

//int convolve(CHANDAT * data, int NFFT, char ** errMsg);

typedef struct upols {
    unsigned long nSubs;
    unsigned long NFFT;
    unsigned long idx_FDL;
    double normalisation;
    kiss_fft_cpx * input;
    kiss_fft_cpx * output;
    kiss_fft_cpx ** subfilters;
    kiss_fft_cpx ** fdelayline;
    kiss_fft_cpx * accum;
} UPOLS;

UPOLS * new_UPOLS(double * buffer, unsigned long size, unsigned long blocksize, char ** errMsg);
int fft_convolve(double * input, double * output, UPOLS * network, unsigned long blocksize, char ** errMsg);
void clear_UPOLS(UPOLS ** process);

#endif
