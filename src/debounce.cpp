#include "debounce.h"

int buttonState = LOW ;     // current state of the button
int lastButtonState  = LOW;     // previous state of the button
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
bool debouncing = false;
int flagWRITE_HTTP=0;
int ledState_SWITCH=0;


void debounce(){
    // read the state of the switch into a local variable:
  int reading = digitalRead(button_pin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    if (debouncing == false){
      lastDebounceTime = millis();
    }
    debouncing = true;
    
  } else {
    debouncing = false;
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        getTIME();
        Serial.println(switchNAME[0]);
        appendFile(SD, "/data.txt", switchNAME[0]);
        appendFile(SD, "/data.txt", "\n");
        digitalWrite(LED_SWITCH, HIGH);   // turn the LED on (HIGH is the voltage level)
        ledState_SWITCH=1;
        flagWRITE_HTTP=1;
      }
      if (buttonState == HIGH){
        digitalWrite(LED_SWITCH, LOW);   // turn the LED on (HIGH is the voltage level)
        ledState_SWITCH=0;
      }            
    }
  }
}