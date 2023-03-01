#include "main.h"

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org", TIME_OFFSET);

AsyncWebServer server(80);
WebSocketsServer webSocket(81);

//Array for tests, location and switch output strings
char testNAME[5][30]={"PRE-TESTS AT ", "BENCH TESTS AT ", "GROUND TESTS AT ", "FLIGHT TESTS AT ", "END TESTS AT "};
char locationNAME[5][30]={"POINT A", "POINT B", "POINT C", "POINT D", "POINT CEiiA"};
char switchNAME[2][20]={"SWITCH ON", "SWITCH OFF"};

//Buffer to store the time and date
char time_buffer[25];

//Time and date string
String timeString;

//RTC Object
RTC_DS3231 RTC;

int flagNTPTime, ledSD_State = 0;
int WiFiLost = 0;

//time variables for SD Card LED when an append happens
unsigned long previousMillis = 0;
unsigned long interval = 1000;



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // this function is called when a websocket message is received
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Message received: %s\n", num, payload);
      break;
  }
}

void setup() {
  //Start UART
  Serial.begin(9600);

  //Setup SWITCH pin as an input with pullup resistor (better debounce performance)
  pinMode(button_pin, INPUT_PULLUP);
  
  // initialize digital pin LED_SWITCH, LED_CARTAO, LED_WIFI as an output.
  pinMode(LED_SWITCH, OUTPUT);
  pinMode(LED_CARTAO,OUTPUT);
  pinMode(LED_WIFI,OUTPUT);
  
  //Setup SD Module
  SD.begin(SD_CS); 
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return; // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "TESTING CAMPAIGN");
    writeFile(SD, "/data.txt", "\n");
    digitalWrite(LED_CARTAO, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(3000);                       // wait for a second
    digitalWrite(LED_CARTAO, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);                      // wait for a second 
  }
  else {
    Serial.println("File already exists");
    digitalWrite(LED_CARTAO, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(3000);                       // wait for a second
    digitalWrite(LED_CARTAO, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);                      // wait for a second 
  }
  file.close();

  //Start WiFi connection
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(F("Connecting to WiFi..."));
  }
  Serial.println("WiFi connected.");
  digitalWrite(LED_WIFI, HIGH);
  // Print local IP address
  Serial.print("IP address: "); 
  Serial.println(WiFi.localIP());

  // Initialize NTP client
  timeClient.begin();

  // Create HTML page with the respective sockets being sent
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200,"text/html", 
    "<html>\
      <head>\
        <title>Data Logger Ground Unit Terminal</title>\
      </head>\
      <body>\
        <h1>Data Logger Ground Unit</h1>\
        <pre id='output'></pre>\
        <script>\
          const socket = new WebSocket('ws://' + window.location.hostname + ':81');\
          socket.onmessage = function (event) {\
            console.log(event.data);\
            document.getElementById('output').innerHTML += event.data + '\\n';\
          };\
          socket.onclose = function (event) {\
            console.error('WebSocket closed', event);\
          };\
        </script>\
      </body>\
    </html>"
  );
  if(outputSent){
      request->send(200, "text/plain", parsingStr);
    }
});

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.begin();
  Serial.println("HTTP server started");

  Serial.println(F("INSERT THE TYPE OF TESTS AND ITS LOCATION: "));
}



/*********************************FUNCTION TO GET THE CURRENT TIME FROM THE NTP SERVER*****************************************/
void getTIME(){
  timeClient.update();
  
  unsigned long currentTime = timeClient.getEpochTime(); //retrieves the current time from the NTP server in epoch time (number of seconds elapsed since January 1, 1970)

  //converts the number of seconds to a date structure
  time_t now = (time_t)currentTime;
  struct tm *timeinfo = localtime(&now);

  //converts the data structure to char for printing on the SD card
  strftime (time_buffer,20,"%Y-%m-%d %H:%M:%S",timeinfo);

  Serial.print(time_buffer);
  Serial.print("  ");
  appendFile(SD, "/data.txt", time_buffer);
  appendFile(SD, "/data.txt", "  ");

  //convert char to string for sending the data to the webserver
  timeString = String(time_buffer);

  //if this flag is 1 then this means the Wi-Fi is still up and will be continuously updating the RTC
  flagNTPTime = 1;
}

void lostWiFi(){
  DateTime nowRTC;
  char bufferRTC[20];


  if(flagNTPTime){
    timeClient.update();
    RTC.adjust(DateTime(timeClient.getEpochTime()));
    nowRTC=RTC.now();
    
    flagNTPTime = 0;
  }

  //if the flag is 0 then from now on the RTC time will be printed with the last saved NTP server time
  else{
    sprintf(bufferRTC, "%04d/%02d/%02d %02d:%02d:02d", nowRTC.year(), nowRTC.month(), nowRTC.day(), nowRTC.hour(), nowRTC. minute(), nowRTC.second());
    Serial.print(bufferRTC);
    Serial.print(" ");
    appendFile(SD, "/data.txt", bufferRTC);
    appendFile(SD, "/data.txt", " ");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){ //in case there's data to be read on the terminal
    parsing();
    if(outputSent){
      outputSent = false;
      webSocket.broadcastTXT(parsingStr.c_str(), parsingStr.length());
      parsingStr.remove(0, parsingStr.length());
    }
  }
  debounce();
  if(flagWRITE_HTTP){
    flagWRITE_HTTP=0;
    String switchNAMEStr = String(switchNAME[0]);
    String combinedString = timeString + " " + switchNAMEStr;
    webSocket.broadcastTXT(combinedString.c_str(), combinedString.length());
  }
  webSocket.loop();

  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(LED_WIFI, HIGH);   // turn the LED on (HIGH is the voltage level)    
  }
  else{
    digitalWrite(LED_WIFI, LOW);   // turn the LED on (HIGH is the voltage level)    
  }
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    return;
  }
  if(file.print(message)) {
  } else {
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {

  unsigned long currentMillis = millis();


  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    return;
  }
  if(file.print(message)) {
    if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledSD_State == 0) {
      ledSD_State = HIGH;
    } else {
      ledSD_State = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_CARTAO, ledSD_State);
    }
  } else {

  }
  file.close();
}
