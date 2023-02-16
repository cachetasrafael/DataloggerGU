#ifndef __DEBOUNCE_H__
#define __DEBOUNCE_H__

/* include statements */
#include "main.h"
/* macro definitions */

/* struct/enum/union definitions */
extern int buttonState;     // current state of the button
extern int lastButtonState;     // previous state of the button
extern unsigned long lastDebounceTime;  // the last time the output pin was toggled
extern unsigned long debounceDelay;    // the debounce time; increase if the output flickers
extern bool debouncing;
extern String switchNAMEStr;
extern int flagWRITE_HTTP;
extern int ledState_SWITCH;

/* function prototypes */
void debounce();

#endif /* __DEBOUNCE_H__ */
