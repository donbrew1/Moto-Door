#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRtext.h>
#include <IRtimer.h>
#include <IRutils.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//Left Wheel
const int A1A = D5;// for in1
const int A1B = D2;//for in2
//Right Wheel
const int B1A = D8;// for in3
const int B1B = D6;//in4 B1B
int doorState =0;// 0 for open when uploaded

int RECV_PIN = D4;// ir sensor
IRrecv irrecv(RECV_PIN);
decode_results results;

#define MQTT_SERVER "192.168.1.212"
const char* ssid = "Dhome";
const char* password = "TiffanyZ7";

char* lightTopic = "door";

int trigPin=D3;
int echoPin=D7;
int pingTravelTime;
float pingTravelDistance;
float distanceToTarget;

void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void reconnect() {
  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if (WiFi.status() == WL_CONNECTED) {
    // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        client.subscribe(lightTopic);
      }
      else {
        abort();
      }
    }
  }
}
//generate unique name from MAC addr
String macToStr(const uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5) {
      result += ':';
    }
  }
  return result;
}
void measure(){
    for (unsigned int i = 0; i < 10; i++){
 digitalWrite(trigPin,LOW);
delayMicroseconds(10);
digitalWrite(trigPin,HIGH);
delayMicroseconds(10);
digitalWrite(trigPin,LOW);
pingTravelTime= pingTravelTime+pulseIn(echoPin,HIGH);
delay(25);}
pingTravelTime=pingTravelTime/10; 
}
void openDoor() {
 measure();
 if (pingTravelTime > 2000)
 {
  digitalWrite(A1A, LOW); 
  digitalWrite(A1B, HIGH);
 digitalWrite(B1A, HIGH);  
  digitalWrite(B1B, LOW);  
  delay(3800);
  digitalWrite(A1A, LOW);
  digitalWrite(A1B, LOW);
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, LOW);
  Serial.println("open");
 }
}
void closeDoor() {
measure();
 if (pingTravelTime < 5000)
  {
  digitalWrite(A1A, HIGH);
  digitalWrite(A1B, LOW);  
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, HIGH);
  delay(3800);
  digitalWrite(A1A, LOW);
  digitalWrite(A1B, LOW);
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, LOW);
  Serial.println("closed");
  }
}
void stopDoor() {
  digitalWrite(A1A, LOW);
  digitalWrite(A1B, LOW);
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, LOW);
}

void setup() {
  pinMode(trigPin,OUTPUT);
pinMode(echoPin,INPUT);
  //start wifi subsystem
  WiFi.begin(ssid, password);
  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();
  //wait a bit before starting the main loop
  delay(2000);
  ArduinoOTA.setHostname("BedRoom Door MQTT2");
  ArduinoOTA.begin();
  // your code

  irrecv.enableIRIn(); //Initialization infrared receiver

  pinMode(B1A, OUTPUT); // define pin as output
  pinMode(B1B, OUTPUT);
  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);

  digitalWrite(A1A, LOW);// set wheels stop
  digitalWrite(A1B, LOW);
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, LOW);
  delay(100);
}

void loop() {

  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {
    reconnect();
  }  
  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10);
 
  //IS THERE A IR SIGNAL
  if (irrecv.decode(&results)) {
    switch (results.value) {
      // CODES FOR APEX FF REW (BOTH) STOP
      case 0x9B918207 : //OPEN
        openDoor();
        break;
      case 0xB9CA1F19: //CLOSE
        closeDoor();
        break;
      case 0x102C :
        stopDoor();
        break;
    }
    // GET THE NEXT IR VALUE
    irrecv.resume();
  }
  { ArduinoOTA.handle();
   measure();
  if (pingTravelTime >5000)
 client.publish("door1","false");//door is closed
 if (pingTravelTime < 9000)
client.publish("door1", "true");//door is open
  
  }
 
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  //convert topic to string to make it easier to work with
  String topicStr = topic;
  
  //open door if the payload is '1' and publish to the MQTT server a confirmation message

  //close door if the payload is '0' and publish to the MQTT server a confirmation message
  //  irValue = digitalRead(irPin);
  if (payload[0] == '0'){
    closeDoor();
    }
  if (payload[0] == '1'){
    openDoor();
     }
}
