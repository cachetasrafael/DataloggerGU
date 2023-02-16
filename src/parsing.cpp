#include "parsing.h"

//Variables for parsing
char input[INPUT_LENGTH];     // buffer to store input
char tests[INPUT_LENGTH], location[INPUT_LENGTH];    // tests and location
int tests_index = 0, location_index = 0; //index of tests, local
bool in_tests = true; // to know if we are parsing tests or location
String parsingStr = "";
bool outputSent = false;

void parsing(){

    // check if data is available  
    char c = Serial.read();

    if (c == ',') { //if we find a comma we know parameter (TYPE OF TESTS) has ended
      in_tests = false;
      tests[tests_index] = '\0'; // mark the end of tests
    }
    else if (c == '\n' || c == '\r') { //if we find a new line character we know that both inputs are ended
      location[location_index] = '\0'; // mark the end of location
      in_tests = true;
      tests_index = 0;
      location_index = 0;

      // check tests
      if (strcmp(tests, "PRE") == 0) {
        getTIME();
        Serial.print(F("PRE-TESTS AT "));
        parsingStr += "PRE-TESTS AT "; 
        outputSent = true; 
        appendFile(SD, "/data.txt", testNAME[0]);     
      } else if (strcmp(tests, "BENCH") == 0) {
        getTIME();
        Serial.print(F("BENCH TESTS AT "));
        parsingStr += "BENCH TESTS AT ";
        outputSent = true;  
        appendFile(SD, "/data.txt", testNAME[1]);      
      }
      else if (strcmp(tests, "GROUND") == 0) {
        getTIME();
        Serial.print(F("GROUND TESTS AT "));
        parsingStr += "GROUND TESTS AT ";
        outputSent = true;  
        appendFile(SD, "/data.txt", testNAME[2]);       
      }
      else if (strcmp(tests, "FLIGHT") == 0) {
        getTIME();
        Serial.print(F("FLIGHT TESTS AT "));
        parsingStr += "FLIGHT TESTS AT ";
        outputSent = true; 
        appendFile(SD, "/data.txt", testNAME[3]);        
      }
      else if (strcmp(tests, "END") == 0) {
        getTIME();
        Serial.print(F("END-TO-END TESTS AT "));
        parsingStr += "END-TO-END TESTS AT ";
        outputSent = true;  
        appendFile(SD, "/data.txt", testNAME[4]);         
      }
      // check location
      if (strcmp(location, " A") == 0) {
        Serial.println("POINT A");
        parsingStr += "POINT A"; 
        outputSent = true; 
        appendFile(SD, "/data.txt", locationNAME[0]);
        appendFile(SD, "/data.txt", "\n");  
      } else if (strcmp(location, " B") == 0) {
        Serial.println("POINT B");
        parsingStr += "POINT B";
        outputSent = true; 
        appendFile(SD, "/data.txt", locationNAME[1]);
        appendFile(SD, "/data.txt", "\n");  
      }
      else if (strcmp(location, " C") == 0) {
        Serial.println("POINT C");
        parsingStr += "POINT C";
        outputSent = true; 
        appendFile(SD, "/data.txt", locationNAME[2]);
        appendFile(SD, "/data.txt", "\n");    
      }
      else if (strcmp(location, " D") == 0) {
        Serial.println("POINT D");
        parsingStr += "POINT D";
        outputSent = true;  
        appendFile(SD, "/data.txt", locationNAME[3]);
        appendFile(SD, "/data.txt", "\n"); 
      }
      else if (strcmp(location, " T") == 0) {
        Serial.println("CEiiA");
        parsingStr += "CEiiA";
        outputSent = true;
        appendFile(SD, "/data.txt", locationNAME[4]);
        appendFile(SD, "/data.txt", "\n"); 
      }
    } else {
      if (in_tests) {
        tests[tests_index++] = c;
      } else {
        location[location_index++] = c;
      }
    }
}