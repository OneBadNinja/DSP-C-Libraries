#include "fftproc.h"
#include <kiss_fft.h>
#include <stdlib.h>
#include <string.h>

/*
 Three steps:
 
 1. Filter Processing
 2. Stream/Input Processing
 3. Resolution
 
 ------
 
 Input is of type SIGNAL and int(NFFT)

 Init
 a) check NFFT for radix-2
 b) obtain NSubs = SIGNAL / (NFFT/2) if SIGNAL % (NFFT/2) == 0 else (SIGNAL / NFFT / 2) + 1
 c) malloc subfilter signal array of size NFFT, initialise to zero
 d) malloc array of kiss_fft_cpx of size NFFT with size NSubs for sub filters - set to complex zero
 e) malloc array of kiss_fft_cpx of size NNFT with size NSubs for input stream - set to complex zero
 f) malloc array of kiss_fft_cpx of size NFFT for output FFT block
 h) malloc SIGNAL array of size NFFT for output time domain block
 i) malloc for final output block?
 
 
 1.
 a) Populate subfilter array with filter blocks in a for loop for Nsubs (initialise to zero each time or only at the end). Within loop compute R2CFFT and store into each kiss_fft_cpx array.

 2.
 a) Reset the subfilter array to zeros, populate right side with block B (NFFT/2) length of signal
 For the sliding window, either use memmove or employ a sliding index (either discard left or right side of
 output buffer --- the better solution) -- is this really commutative though? Check maths.
 b) For loop - take FFT of each subfilter array population, insert into a circular Frequency-domain delay line buffer (part e in init). This delay line buffer index is independent of the sub-filter buffer index.
 c) Perform frequency domain multiplications - accumulate results of each array into output block (zero init this for each loop iteration)
 d) Take IFFT of the final spectral sum and store into output buffer (init.h above). Slice the right hand side for the final output buffer (or slice based on sliding index if applicable)
 
 3. Free buffers init c - i.
 
 */

static int allocateFFTBuffer(kiss_fft_cpx ** buffer, unsigned long size);
static int resetFFTBuffer(kiss_fft_cpx ** buffer, unsigned long size);
static int init_UPOLS(UPOLS * process, unsigned long size, unsigned long blocksize, char **errMsg);
static kiss_fft_cpx complexMultiply(kiss_fft_cpx a, kiss_fft_cpx b);


int fft_convolve(double * input, double * output, UPOLS * network, unsigned long blocksize, char ** errMsg) {
    unsigned long idx;
    kiss_fft_cfg cfg;
    if(ceil(log2(blocksize)) != floor(log2(blocksize))) {
        *errMsg = "Blocksize must be of power 2";
        return 0;
    }
    else if(blocksize * 2 != network->NFFT) {
        *errMsg = "Blocksize must be half of the UPOLS NFFT";
        return 0;
    }
    /* Shift previous input samples to the LHS */
    memcpy(network->input, network->input + blocksize, blocksize * sizeof(kiss_fft_cpx));
    
    /* Take input block and populate RHS of input buffer - also normalise by NFFT */
    for(idx = 0; idx < blocksize; idx++) {
        network->input[blocksize + idx].r = input[idx] / network->normalisation;
        network->input[blocksize + idx].i = 0.0; //temp
    }
    
    /* Take FFT of input block, insert into FDL at the correct index */
    cfg = kiss_fft_alloc(network->NFFT, 0, 0, 0);
    kiss_fft(cfg, network->input, network->fdelayline[network->idx_FDL]);
    kiss_fft_free(cfg);
    
    /* Complex multiply FDL with sub filters, push results into accumulator */
    for (idx = 0; idx < network->nSubs; idx++) {
        /* The current FDL index must multiply with the idx-th subfilter */
        long fdl_idx = (network->idx_FDL + idx) % network->nSubs;
        unsigned long j;
        kiss_fft_cpx res;
//        fprintf(stdout, "Subfilter: %lu FDLmod: %lu, FDL: %lu\n", idx, fdl_idx, network->idx_FDL);
        for (j = 0; j < network->NFFT; j++) {
            res = complexMultiply(network->subfilters[idx][j], network->fdelayline[fdl_idx][j]);
            /* Include normalisation */
            network->accum[j].r += res.r;
            network->accum[j].i += res.i;
        }
    }
    
    /* Take IFFT of accumulator, populate output buffer */
    cfg = kiss_fft_alloc(network->NFFT, 1, 0, 0);
    kiss_fft(cfg, network->accum, network->output);
    kiss_fft_free(cfg);

    /* Populate output block with RHS of output buffer */
    for(idx = 0; idx < blocksize; idx++) {
        output[idx] = network->output[idx + blocksize].r;
    }

    /* Reset accumulator to zero */
    resetFFTBuffer(&network->accum, network->NFFT);

    /* Update indices */
    if(!network->idx_FDL) {
        network->idx_FDL += network->nSubs;
    }
    network->idx_FDL = (network->idx_FDL - 1) % network->nSubs;

    return 1;
}

UPOLS * new_UPOLS(double * buffer, unsigned long size, unsigned long blocksize, char ** errMsg) {
    unsigned int i;
    kiss_fft_cfg cfg;
    unsigned long remainder = size % blocksize;
    UPOLS * process;

    /* Allocate memory for UPOLS network */
    process = (UPOLS *) malloc(sizeof(UPOLS));
    if(!process) {
        *errMsg = "Could not allocate memory for UPOLS structure.";
        return NULL;
    }
    
    /* Initialise all UPOLS buffers to zero */
    if(!init_UPOLS(process, size, blocksize, errMsg)) {
        *errMsg = "Could not initialise UPOLS";
        return NULL;
    }

    /* Create forward FFT configuration */
    cfg = kiss_fft_alloc(process->NFFT, 0, 0, 0);

    /* Dissect the FIR filter */
    for(i = 0; i < process->nSubs; i++) {
        unsigned int j;
        if(remainder && i == process->nSubs - 1) {
            blocksize = remainder;
        }
        /* Populate temporary input buffer for FFT, normalise by scaling factor */
        for(j = 0; j < blocksize; j++) {
            process->input[j].r = buffer[i * process->NFFT/2 + j] / process->normalisation;
            process->input[j].i = 0;
        }

        /* Compute FFT of padded filter blocks */
        kiss_fft(cfg, process->input, process->subfilters[i]);

        /* Reset the temporary input buffer */
        if(!resetFFTBuffer(&process->input, process->NFFT)) {
            /* Clear current buffer */
            clear_UPOLS(&process);
            *errMsg = "Could not reset temporary input buffer";
            return NULL;
        }
    }

    /* Free the configuration */
    kiss_fft_free(cfg);

    return process;
}

int init_UPOLS(UPOLS * process, unsigned long size, unsigned long blocksize, char **errMsg) {
    unsigned long i;
    process->nSubs = (size/blocksize) + ((size % blocksize) ? 1 : 0);
    process->idx_FDL = 0;
    process->NFFT = 2 * blocksize;
    process->normalisation = (double) process->NFFT;

    /* Allocate accumulator HEAP memory */
    if(!allocateFFTBuffer(&process->accum, process->NFFT)) {
        *errMsg = "Could not allocate storage for accumulator";
        return 0;
    }
    /* Allocate IO buffer HEAP memory */
    if(!allocateFFTBuffer(&process->input, process->NFFT) ||
       !allocateFFTBuffer(&process->output, process->NFFT)) {
        clear_UPOLS(&process);
        *errMsg = "Could not allocate IO buffers";
        return 0;
    }
    /* Allocate frequency delay line buffer */
    process->fdelayline = (kiss_fft_cpx **) malloc(sizeof(kiss_fft_cpx *) * process->nSubs);
    if(!process->fdelayline) {
        clear_UPOLS(&process);
        *errMsg = "Could not allocate storage for delay line array";
        return 0;
    }
    /* Allocate subfilter bank HEAP memory */
    process->subfilters = (kiss_fft_cpx **) malloc(sizeof(kiss_fft_cpx *) * process->nSubs);
    if(!process->subfilters) {
        clear_UPOLS(&process);
        *errMsg = "Could not allocate storage for sub filter array";
        return 0;
    }
    for(i = 0; i < process->nSubs; i++) {
        /* allocate buffers */
        if(!allocateFFTBuffer(&process->fdelayline[i], process->NFFT) ||
           !allocateFFTBuffer(&process->subfilters[i], process->NFFT)) {
            clear_UPOLS(&process);
            *errMsg = "Could not allocate storage for FFT arrays";
            return 0;
            break;
        }
    }
    return 1;
}

void clear_UPOLS(UPOLS ** process) {
    int i;
    if((*process)->accum) {
        free((*process)->accum);
    }
    if((*process)->input) {
        free((*process)->input);
    }
    if((*process)->output) {
        free((*process)->output);
    }
    if((*process)->fdelayline) {
        for(i = 0; i < (*process)->nSubs; i++) {
            if((*process)->fdelayline[i]) {
                free((*process)->fdelayline[i]);
            }
        }
        free((*process)->fdelayline);
    }
    if((*process)->subfilters) {
        for(i = 0; i < (*process)->nSubs; i++) {
            if((*process)->subfilters[i]) {
                free((*process)->subfilters[i]);
            }
        }
        free((*process)->subfilters);
    }
    free(*process);
    process = NULL;
}

static int allocateFFTBuffer(kiss_fft_cpx ** buffer, unsigned long size) {
    *buffer = (kiss_fft_cpx *) malloc(sizeof(kiss_fft_cpx) * size);
    if(!(*buffer)) {
        *buffer = NULL;
        return 0;
    }
    return resetFFTBuffer(buffer, size);
}

static int resetFFTBuffer(kiss_fft_cpx ** buffer, unsigned long size) {
    unsigned long i;
    if(!(*buffer)) {
        return 0;
    }
    for (i = 0; i < size; i++) {
        (*buffer)[i].i = 0;
        (*buffer)[i].r = 0;
    }
    return 1;
}

static kiss_fft_cpx complexMultiply(kiss_fft_cpx x, kiss_fft_cpx y) {
    kiss_fft_cpx res;
    res.r = (x.r * y.r) - (x.i * y.i);
    res.i = (x.r * y.i) + (x.i * y.r);
    return res;
}
