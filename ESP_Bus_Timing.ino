// Wemos D1 mini: Lolin (Wemos) D1 mini, CPU 80 MHz, Flash 4 MB, Upload 962100

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>

#include <Ticker.h>


#ifndef CHARBUFF
#include "char_buff.h"
#endif

// -------- WIFI -------------------------------
// Get passwords.h
#ifndef STASSID
#include "passwords.h"
#endif

// Wifi connection credentials
const char* ssid = STASSID;
const char* password = STAPSK;
const char* api_key = API_KEY;

// -------- WIFI -------------------------------

ESP8266WiFiMulti WiFiMulti;

// -------- TIME -------------------------------

#include <time.h>

int timezone = 8;
int dst = 0;

Ticker blinker;
 
#define LED 13
 
//=======================================================================
void changeState()
{
  digitalWrite(LED, !(digitalRead(LED)));  //Invert Current State of LED  
}
//=======================================================================



void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  //Initialize Ticker every 0.5s
  blinker.attach_ms(2, changeState); //Use <strong>attach_ms</strong> if you need time in ms

  // Empty the buffer
  char_buff_empty();
}


void loop() {
  unsigned long last_time = millis();
  unsigned long time1;
  unsigned long time2;
  
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    // Read current time from time server
    configTime(timezone * 3600, dst, "sg.pool.ntp.org", "time.nist.gov");
    while (!time(nullptr)) {
      Serial.print(".");
      delay(1000);
    }

    time1 = millis();
    blinker.detach(); 
    GetBusData();
    time2 = millis();
    blinker.attach_ms(2, changeState); //Use <strong>attach_ms</strong> if you need time in ms
  }

  Serial.println(time1 - last_time);
  Serial.println(time2 - time1);
  
  delay(10000);
}

void GetBusData() {
  // Empty the buffer
  char_buff_empty();

  WiFiClient client;
  HTTPClient http;
  
  time_t now = time(nullptr);
  Serial.println(ctime(&now));

  Serial.print("[HTTP] begin...\n");
  if (http.begin(client, "http://datamall2.mytransport.sg/ltaodataservice/BusArrivalv2?BusStopCode=59281")) {  // HTTP
    http.addHeader("AccountKey", api_key);

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();

        // From:  https://www.instructables.com/id/ESP8266-Parsing-JSON/
        // https://arduinojson.org/v6/assistant/
        
        const size_t bufferSize = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(5) + 6*JSON_OBJECT_SIZE(9) + 1360;
        DynamicJsonDocument doc(bufferSize);
            
        deserializeJson(doc, http.getString());
        
        JsonObject Services_0 = doc["Services"][0];
        const char* Services_0_ServiceNo = Services_0["ServiceNo"]; // "805"
        
        JsonObject Services_0_NextBus = Services_0["NextBus"];
        const char* Services_0_NextBus_EstimatedArrival = Services_0_NextBus["EstimatedArrival"]; // "2020-07-12T21:29:03+08:00"
        
        JsonObject Services_0_NextBus2 = Services_0["NextBus2"];
        const char* Services_0_NextBus2_EstimatedArrival = Services_0_NextBus2["EstimatedArrival"]; // "2020-07-12T21:41:09+08:00"
        
        JsonObject Services_0_NextBus3 = Services_0["NextBus3"];
        const char* Services_0_NextBus3_EstimatedArrival = Services_0_NextBus3["EstimatedArrival"]; // "2020-07-12T21:47:57+08:00"
        
        JsonObject Services_1 = doc["Services"][1];
        const char* Services_1_ServiceNo = Services_1["ServiceNo"]; // "812"
        
        JsonObject Services_1_NextBus = Services_1["NextBus"];// "59009"
        const char* Services_1_NextBus_EstimatedArrival = Services_1_NextBus["EstimatedArrival"]; // "2020-07-12T21:27:34+08:00"
        
        JsonObject Services_1_NextBus2 = Services_1["NextBus2"];
        const char* Services_1_NextBus2_EstimatedArrival = Services_1_NextBus2["EstimatedArrival"]; // "2020-07-12T21:43:12+08:00"
        
        JsonObject Services_1_NextBus3 = Services_1["NextBus3"];
        const char* Services_1_NextBus3_EstimatedArrival = Services_1_NextBus3["EstimatedArrival"]; // "2020-07-12T21:53:41+08:00"

        Serial.println(Services_0_ServiceNo);
        char_buff_append(" 805"); 
        PrintTimeToBus(now, char_string_to_time(Services_0_NextBus_EstimatedArrival));
        PrintTimeToBus(now, char_string_to_time(Services_0_NextBus2_EstimatedArrival));
        PrintTimeToBus(now, char_string_to_time(Services_0_NextBus3_EstimatedArrival));
        Serial.println(Services_1_ServiceNo);
        char_buff_append(" 812"); 
        PrintTimeToBus(now, char_string_to_time(Services_1_NextBus_EstimatedArrival));
        PrintTimeToBus(now, char_string_to_time(Services_1_NextBus2_EstimatedArrival));
        PrintTimeToBus(now, char_string_to_time(Services_1_NextBus3_EstimatedArrival));
        //Serial.println(Services_1_NextBus_EstimatedArrival);
        //Serial.println(Services_1_NextBus2_EstimatedArrival);
        //Serial.println(Services_1_NextBus3_EstimatedArrival);
        
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }

  Serial.println (char_buff);

}


void PrintTimeToBus(time_t current_time, time_t bus_time) {
  double seconds;
  double minutes;
  int minutes_int;

  char tmp_char[4];
  
  //Serial.println(ctime(&current_time));
  //Serial.println(ctime(&bus_time));

  seconds = difftime(bus_time, current_time);
  minutes = seconds/60;
  minutes_int = (int) minutes;
  
  if (minutes <= 2) {
    Serial.print("Arr / ");
    Serial.println(ctime(&bus_time));
    char_buff_append(" Arr", 4);
  } else {
    Serial.print(minutes_int);
    Serial.print(" min / ");
    Serial.println(ctime(&bus_time));
    sprintf(tmp_char, "%4d", minutes_int);
    char_buff_append(tmp_char, 4);
  }
}


time_t char_string_to_time(const char* char_arr) {
  time_t rawtime;
  struct tm* timeinfo;

  int year, month, day, hour, minute, second;
  //Serial.println("parse date");
  sscanf(char_arr, "%4d-%2d-%2dT%2d:%2d:%2d+08:00", &year, &month, &day, &hour, &minute, &second);

  // Getting time
  //Serial.println("getting raw time");
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  // Filling in time
  //Serial.println("Filling in time");
  timeinfo->tm_year = year - 1900;
  timeinfo->tm_mon = month - 1;
  timeinfo->tm_mday = day;
  timeinfo->tm_hour = hour;
  timeinfo->tm_min = minute;
  timeinfo->tm_sec = day;

  /*
  Serial.println(year);
  Serial.println(month);
  Serial.println(day);
  Serial.println(hour);
  Serial.println(minute);
  Serial.println(second);
  */
  return mktime(timeinfo);
}

