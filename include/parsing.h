#ifndef __PARSING_H__
#define __PARSING_H__

/* include statements */
#include "main.h"

/* macro definitions */
//Variables for parsing
const int INPUT_LENGTH = 20;  // length of input buffer
extern char input[INPUT_LENGTH];     // buffer to store input
extern char tests[INPUT_LENGTH], location[INPUT_LENGTH];    // tests and location
extern int tests_index, location_index; //index of tests, local
extern bool in_tests; // to know if we are parsing tests or location
extern String parsingStr;
extern bool outputSent;
/* struct/enum/union definitions */

/* function prototypes */
void parsing();
#endif /* __PARSING_H__ */
