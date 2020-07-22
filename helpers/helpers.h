#ifndef HELPERS_H_ID
#define HELPERS_H_ID


/**
 * Supports argument parsing for key-value pairs in the form of [-param val] or non-value parameters (ie -T)
 * @param argv - pointer to an array of strings which is traversed
 * @param argend - pointer to a termination string
 * @param key - character pointer to store the key
 * @param value - string pointer to store the corresponding value.
 * @return boolean integer specifiying whether an argument has been parsed (0 if end of argument list)
 */
int argParse(char *** argv, char ** argend, char * key, char ** value);

/**
 * Checks whether a key-value argument has an empty value or not, terminating the program if the value is empty.
 * @param value - the value string for the corresponding key
 * @param key -  the character  key for the argument
 */
void argCheck(char * value, char key);

/**
 * Checks whether a string is entirely numeric, returning a boolean value, and populating a supplied float pointer with the numeric value true.
 * @param str - The string to check
 * @param val - Pointer to a float which is populated by the parsed numeric value from the string
 * @return boolean integer specifying whether the string is numeric (and hence whether val was populated with a valid numeric value).
 */
int is_num(char * str, float * val);

/**
 * Terminates the program with a given error message. Convenience "shortcut" function.
 * @param msg - Error message string to send to stderr.
 */
void quit(char * msg);
#endif
