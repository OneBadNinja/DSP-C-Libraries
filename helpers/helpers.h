#ifndef HELPERS_H_ID
#define HELPERS_H_ID

int argParse(char *** argv, char ** argend, char * key, char ** value);

void argCheck(char * value, char key);

int is_num(char * str, float * val);

void quit(char * msg);
#endif
