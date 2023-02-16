#ifndef __MAIN_H__
#define __MAIN_H__

/* include statements */
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <Arduino.h>
#include <NTPClient.h>
#include <RTClib.h>
#include "FS.h"
#include "SPI.h"
#include "SD.h"
#include "string.h"
#include "esp_wifi.h"
#include "debounce.h"
#include "parsing.h"

/* macro definitions */
//OFFSET SERVER NTP
#define TIME_OFFSET 0 // Eastern Time (US & Canada)

//WiFi SSID and PW
#define WIFI_SSID "DataloggerAAM"
#define WIFI_PASSWORD "datalogger321"

//Pins
const int SD_CS = 5; // chip select pin for SD card
const int button_pin=25; // button pin

const int LED_SWITCH=32;  // led ativação do switch
const int LED_CARTAO=27; // led criar ficheiro no cartao
const int LED_WIFI=33; // led conexao wifi




/* struct/enum/union definitions */

//Array for tests, location and switch
extern char testNAME[5][30];
extern char locationNAME[5][30];
extern char switchNAME[2][20];

extern int flagNTPTime;
extern int ledSD_State;


extern unsigned long previousMillis, interval, previousMillis_WiFi, interval_WiFi;


/* function prototypes */
void getTIME();
void appendFile(fs::FS &fs, const char *, const char *);
void writeFile(fs::FS &fs, const char *, const char *);


#endif /* __MAIN_H__ */
