/************************************************************
ESPUberSurgeAlert.ino
Uber API Example
Andres Sabas @ The Inventor's House
Original Creation Date: March 7, 2016
https://github.com/sabas1080/UberSurgeAlertwithESP8266

This example demonstrates how to use the TCP client 
functionality of the SparkFun ESP8266 WiFi library to post 
to a IFTTT event on 
https://ifttt.com

Development environment specifics:
  IDE: Arduino 1.6.5
  ESP8266 HUZZAH Adafruit
  Node MCU 1.0

This code is beerware; if you see me (or any other The Inventor's House 
member) at the local, and you've found our code helpful, 
please buy us a round!

Distributed as-is; no warranty is given.
************************************************************/

#include <ESP8266WiFi.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

// Time to sleep (in seconds):
const int sleepTimeS = 300;

WiFiClient client;

void setup() {
Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

  int status;

 Serial.print(F("connecting to "));
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WPA_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address is: "));
  Serial.println(WiFi.localIP());

  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(14,OUTPUT);

  Serial.println("Setup complete.\n");
}

void loop() {

    TembooChoreo GetPriceEstimatesChoreo(client);

    // Invoke the Temboo client
    GetPriceEstimatesChoreo.begin();

    // Set Temboo account credentials
    GetPriceEstimatesChoreo.setAccountName(TEMBOO_ACCOUNT);
    GetPriceEstimatesChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    GetPriceEstimatesChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String StartLatitudeValue ="21.880989";
    GetPriceEstimatesChoreo.addInput("StartLatitude", StartLatitudeValue);
    String EndLatitudeValue = "21.9004248";
    GetPriceEstimatesChoreo.addInput("EndLatitude", EndLatitudeValue);
    String ServerTokenValue = "YourServerToken"; // Insert You Server Token Uber API
    GetPriceEstimatesChoreo.addInput("ServerToken", ServerTokenValue);
    String StartLongitudeValue = "-102.2996968";
    GetPriceEstimatesChoreo.addInput("StartLongitude", StartLongitudeValue);
    String EndLongitudeValue = "-102.316174";
    GetPriceEstimatesChoreo.addInput("EndLongitude", EndLongitudeValue);

    // Identify the Choreo to run
    GetPriceEstimatesChoreo.setChoreo("/Library/Uber/Estimates/GetPriceEstimates");

    // add an output filter to extract the current surge_multiplier
    GetPriceEstimatesChoreo.addOutputFilter("surge_multiplier", "/prices/surge_multiplier", "Response");

    
    // add an output filter to extract the current name
    GetPriceEstimatesChoreo.addOutputFilter("display_name", "/prices/display_name", "Response");

    // add an output filter to extract the current name
    //GetPriceEstimatesChoreo.addOutputFilter("high_estimate", "/prices/high_estimate", "Response");
    

    GetPriceEstimatesChoreo.run();
    
    int a=0, b=0;
 
  while(GetPriceEstimatesChoreo.available()) {
       // read the name of the next output item
        String name = GetPriceEstimatesChoreo.readStringUntil('\x1F');
        name.trim(); // use “trim” to get rid of newlines

        // read the value of the next output item
        String data = GetPriceEstimatesChoreo.readStringUntil('\x1E');
        data.trim(); // use “trim” to get rid of newlines

         if (name == "display_name") {
             if(data =="uberX" ){
              a++;
             Serial.println("Tipo de Uber: " + data);
             }
         }
          if((name == "surge_multiplier") && (b<a)){
              b++;
              //Serial.println("Multiplicador: " + data);
              int dato =data.toInt();
              Serial.println(dato);
              if((dato>=1) && (dato<=2)){
                Serial.println("Multiplicador: 1");
                digitalWrite(13,HIGH);
                digitalWrite(12,LOW);
                digitalWrite(14,LOW);
                delay(1000);
              }
              if((dato>2) && (dato<=3)){
                Serial.println("Multiplicador: 2");
                digitalWrite(13,HIGH);
                digitalWrite(12,HIGH);
                digitalWrite(14,LOW);
                delay(1000);
              }
              if((dato>3) && (dato<=4)){
                Serial.println("Multiplicador 3");
                digitalWrite(13,HIGH);
                digitalWrite(12,HIGH);
                digitalWrite(14,HIGH);
                delay(1000);
              }
             }
         }
      a=0;
      b=0;
      GetPriceEstimatesChoreo.close();
      Serial.print(F("Sleeping..."));
      // deepSleep time is defined in microseconds. Multiply
      // seconds by 1e6 
      ESP.deepSleep(sleepTimeS * 1000000);
    
}
