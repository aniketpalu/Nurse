#include "config.h"
#define LED 02  // LED is connected to GPIO 02  
#define TouchPin  05 //  in ESP32 board There are 9 Touch Pin these ar
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/************************ Example Starts Here *******************************/

// digital pin 5
#define TouchPin 5
#define L 4
// button state
String current;
String current1;
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "XXXXXXXXX"
#define MQTT_PASS "XXXXXXXXXXXXXXXXXXX"
int UpperThreshold = 518;
    int LowerThreshold = 490;
    int reading = 0;
    float BPM = 0.0;
    bool IgnoreReading = false;
    bool FirstPulseDetected = false;
    unsigned long FirstPulseTime = 0;
    unsigned long SecondPulseTime = 0;
    unsigned long PulseInterval = 0;



// set up the 'digital' feed
AdafruitIO_Feed *Nurse = io.feed("Nurse");
AdafruitIO_Feed *PulseRate = io.feed("PulseRate");


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/Light");

void setup() {

  // set button pin as an input
  pinMode(TouchPin, INPUT);

  // start the serial connection
  Serial.begin(9600);

  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());



}

void loop() {
    io.run();
    int button=0;
int val = analogRead(TouchPin);// store tha touch value in val variable
  Serial.println(val);
  delay(1000);
  if(val>0)
  {
      current = "Patient Needs help"; 
  Serial.print("sending button -> ");
  Serial.println(current);
  Nurse->save(current);   
  }

  else
  {
      
    Serial.println("Patient is OK");
      
  }

  reading = analogRead(A0); 

      // Heart beat leading edge detected.
      if(reading > UpperThreshold && IgnoreReading == false){
        if(FirstPulseDetected == false){
          FirstPulseTime = millis();
          FirstPulseDetected = true;
        }
        else{
          SecondPulseTime = millis();
          PulseInterval = SecondPulseTime - FirstPulseTime;
          FirstPulseTime = SecondPulseTime;
        }
        IgnoreReading = true;
      }

      // Heart beat trailing edge detected.
      if(reading < LowerThreshold){
        IgnoreReading = false;
      }  

      BPM = (1.0/reading) * 60.0 * 1000;

      Serial.print(reading);
      Serial.print("\t");
      Serial.print("\t");
      Serial.print(BPM);
      Serial.println(" BPM");
      Serial.flush();
PulseRate->save(BPM);
  delay(1000);
      // Please don't use delay() - this is just for testing purposes.
      delay(1000);  
   
}
