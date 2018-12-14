/******************************************
   The unmodified version of this code is originally
   by kakopappa and can be found at http://bit.ly/2kKQiRg.

   This version of the code has been modified by Charles Gantt
   and requires five additional files which can be found at http://bit.ly/2lRDwAJ

   Find out more about this project on Charles' website
   http://www.themakersworkbench.com

   Follow Charles and TheMakersWorkbench on the following sites:
   YouTube: bit.ly/TMWB-on-YouTube
   TMWB on Facebook: bit.ly/TMWB-on-Facebook
   CJGanttMakes on Facebook: bit.ly/CJGanttMakes
   TMWB on Twitter: bit.ly/TMWB-on-Twitter
   Charles Gantt on Twitter: bit.ly/CJGanttOnTwitter
   Instructables: bit.ly/CJGanttOnInstructables
   TMWB Website: bit.ly/TheMakersWorkbench
   Charles Gantt on Element14: bit.ly/CJGantt-On-E14
   Charles Gantt on GitHub: bit.ly/CJGantt-on-Github
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include <Wire.h>

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>



// prototypes
boolean connectWifi();

//on/off callbacks
void lightOneOn();
void lightOneOff();
void lightTwoOn();
void lightTwoOff();
void outletOneOn();
void outletOneOff();


// Change this before you flash
const char* ssid = "YOUR WIFI USERNAME HERE";
const char* password = "YOUR WIFI PASSWORD HERE";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *lightOne = NULL;
Switch *lightTwo = NULL;
Switch *outletOne = NULL;


// Set Relay Pins
int relayOne = 4;
#define IR_LED 4  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(relayOne);  // Set the GPIO to be used to sending the message.

uint16_t AcOn[59] = {3130, 9916,  488, 1596,  518, 520,  516, 520,  516, 524,  520,
1554,  516, 520,  518, 528,  516, 526,  518, 524,  518, 518,  516, 528,  516, 518,  518,
516,  518, 518,  516, 518,  516, 528,  490, 1596,  518, 528,  516, 528,  516, 1570,  516,
520,  516, 1562,  518, 526,  518, 518,  516, 1572,  516, 1562,  518, 528,  516, 1584,  494};

uint16_t AcOff[59] = {3150, 9900, 500, 1600, 500, 550, 500, 550, 500, 550, 500, 1600, 500, 550, 500, 550, 500, 550, 500, 1600, 500, 1600, 500, 550, 500, 550, 500, 550, 500, 550, 500, 550, 500, 550, 500, 550, 500, 550, 500, 550, 500, 550, 500, 550, 500, 1600, 500, 550, 500, 1600, 500, 550, 500, 550, 500, 550, 500, 1600, 500};

uint16_t highCool[59] = {3162, 9856,  546, 1564,  518, 524,  516, 534,  520, 524,  492, 1594,  520, 518,  518, 526,  518, 524,  520, 524,  518, 536,  518, 524,  514, 1584,  518, 524,  520, 526,  512, 536,  520, 518,  518, 524,  520, 516,  518, 518,  518, 518,  518, 1562,  518, 1560,  520, 516,  518, 1570,  518, 1560,  518, 1562,  518, 1554,  518, 518,  518};

uint16_t temp24[59] = {3164, 9906,  506, 1578,  518, 526,  518, 518,  518, 526,  494, 1586,  492, 550,  520, 516,  496, 548,  518, 526,  494, 552,  518, 524,  518, 518,  514, 1574,  492, 556,  494, 550,  516, 520,  518, 1562,  494, 540,  520, 518,  518, 1554,  494, 540,  494, 1596,  492, 548,  518, 528,  516, 524,  518, 1572,  516, 526,  494, 1588,  492}; 

uint16_t temp27[59] = {3134, 9902,  490, 1596,  520, 516,  518, 516,  520, 524,  520, 1552,  520, 524,  518, 524,  520, 518,  520, 522,  520, 518,  518, 516,  520, 516,  518, 1554,  518, 518,  518, 516,  520, 516,  520, 1576,  520, 1568,  518, 532,  520, 524,  494, 558,  520, 1570,  518, 522,  520, 516,  520, 1554,  518, 526,  542, 494,  518, 516,  492};

void setup()
{
  Serial.begin(115200);
  
  // Initialise wifi connection
  wifiConnected = connectWifi();
  Serial.print("WiFi Connected");

  if (wifiConnected) {
    upnpBroadcastResponder.beginUdpMulticast();

    // Define switches here. Max 10
    // Format: Alexa invocation name, local port no, on callback, off callback
    lightOne = new Switch("Office AC", 3601, lightOneOn, lightOneOff);
    lightTwo = new Switch("High Cool", 3602, lightTwoOn, lightTwoOff);
    outletOne = new Switch("Room Temperature", 3603, outletOneOn, outletOneOff);

    //Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*lightOne);
    upnpBroadcastResponder.addDevice(*lightTwo);
    upnpBroadcastResponder.addDevice(*outletOne);

    //Set relay pins to outputs
    pinMode(relayOne, OUTPUT);
  }
}

void loop()
{
  if (wifiConnected) {
    upnpBroadcastResponder.serverLoop();
    lightOne->serverLoop();
    lightTwo->serverLoop();
    outletOne->serverLoop();
  }
}
// AC ON
void lightOneOn() {
 irsend.sendRaw(AcOn, 59, 38);

}
// AC OFF
void lightOneOff() {
  // Serial.print("Socket 2 turn off ...");
 irsend.sendRaw(AcOff, 59, 38); 
}

// HIGH COOL
void lightTwoOn() {
 Serial.print("Socket 1 turn on ...");
 irsend.sendRaw(highCool, 59,38);
}

// Turn off High Cool (24 degrees)
void lightTwoOff() {
  // Serial.print("Switch 2 turn off ...");
//Serial.print("Socket 1 turn off ...");
  irsend.sendRaw(temp24, 59, 38);
}

//27 Degrees
void outletOneOn() {
  irsend.sendRaw(temp27, 59, 38);
}
//24 Degrees
void outletOneOff() {
  irsend.sendRaw(temp24, 59, 38);
}


// connect to wifi – returns true if successful or false if not
  boolean connectWifi() {
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  // Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }

  if (state) {
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
     Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
  }
  else {
     Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}
