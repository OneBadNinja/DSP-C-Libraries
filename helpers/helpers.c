#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Supports argument parsing for key-value pairs in the form of [-param val] or non-value parameters (ie -T)
 argv - pointer to an array of strings which is traversed
 argend - pointer to a termination string
 key - character pointer to store the key
 value - string pointer to store the corresponding value.
 */
int argParse(char *** argv, char ** argend, char * key, char ** value) {
    if(*argv <= argend) {
        /* Default empty value (no key-value) */
        *value = "";
        if((**argv)[0] == '-') {
            /* Obtain the key */
            *key = (**argv)[1];
            /* Memory access check */
            if((++(*argv)) <= argend) {
                if(**argv[0] != '-') {
                    *value = *(*argv)++;
                }
            }
            return 1;
        }
        return 0;
    }
    return 0;
}

void argCheck(char * value, char key) {
    if(!strcmp(value, "")) {
        char msg[50] = "";
        sprintf(msg, "Unspecified value for option '%c'", key);
        quit(msg);
    }
}

/*
 * Checks whether a string is entirely numeric, returning a boolean value. *val is populated by the numeric value.
 */
int is_num(char * str, float * val) {
    char * cont;
    *val = strtod(str, &cont);
    if(*cont) {
        return 0;
    }
    return 1;
}

void quit(char * msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
