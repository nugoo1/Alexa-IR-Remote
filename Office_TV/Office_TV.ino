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

/******************************************
 * This version of the code is a fork by Nuwan Goonewardena.
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



// Change this before you flash
const char* ssid = "YOUR WIFI USERNAME";
const char* password = "YOUR WIFI PASSWORD";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *lightOne = NULL;
Switch *lightTwo = NULL;



// Set Relay Pins
int relayOne = 4;




IRsend irsend(relayOne);  // Set the GPIO to be used to sending the message.
 
void setup()
{
 

  //Serial.begin(115200);

  // Initialise wifi connection
  wifiConnected = connectWifi();
  Serial.print("WiFi Connected");

  if (wifiConnected) {
    upnpBroadcastResponder.beginUdpMulticast();

 
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    lightOne = new Switch("TV", 3611, lightOneOn, lightOneOff);
    lightTwo = new Switch("HDMI", 3612, lightTwoOn, lightTwoOff);
    

    //Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*lightOne);
    upnpBroadcastResponder.addDevice(*lightTwo);
 

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
  }
}
// TV ON
void lightOneOn() {
  Serial.print("Switch 1 turn on ...");
  irsend.sendNEC(0x4FB4AB5, 32);

}
// TV OFF
void lightOneOff() {
  Serial.print("Switch 1 turn off ...");
  irsend.sendNEC(0x4FB4AB5, 32);
  
 
}
// Source Up
void lightTwoOn() {
  Serial.print("Switch 2 turn on ...");
   irsend.sendNEC(0x4FB728D, 32);
  delay(500);
  irsend.sendNEC(0x4FBE21D, 32);
  delay(500);
  irsend.sendNEC(0x4FB52AD, 32);
}

// Source Down
void lightTwoOff() {
  // Serial.print("Switch 2 turn off ...");
  irsend.sendNEC(0x4FB728D, 32);
  delay(500);
  irsend.sendNEC(0x4FBB24D, 32);
  delay(500);
  irsend.sendNEC(0x4FB52AD, 32);
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
    //  Serial.println("");
    //  Serial.print("Connected to ");
    //  Serial.println(ssid);
    // Serial.print("IP address: ");
    //  Serial.println(WiFi.localIP());
  }
  else {
    // Serial.println("");
    //Serial.println("Connection failed.");
  }

  return state;
}
