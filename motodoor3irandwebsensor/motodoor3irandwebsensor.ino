#include "0TA.h"
#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRtext.h>
#include <IRtimer.h>
#include <IRutils.h>

//Left Wheel
const int A1A = D5;// for in1
const int A1B = D2;//for in2
//Right Wheel
const int B1A = D8;// for in3
const int B1B = D6;//in4 B1B
const int irPin = D1; //DOOR SENSOR
int irValue = 0;
int RECV_PIN = D4;
IRrecv irrecv(RECV_PIN);
decode_results results;

unsigned long entry;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  setupOTA("motodoorWebSensor");

  // your code
  pinMode(irPin, INPUT);
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
  WiFiClient client = server.available();   // Listen for incoming clients
  Serial.begin (115200);

  //IS THERE A WEB REQUEST
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
             irValue = digitalRead(irPin);
            if (irValue != 0){ 
              Serial.println("OPENING DOOR");
              //    OPEN;
              digitalWrite(A1A, LOW);
              analogWrite(A1B, 850);
              digitalWrite(B1B, LOW);
              analogWrite(B1A, 850);
              delay(5000);
              digitalWrite(A1A, LOW);
              digitalWrite(A1B, LOW);
              digitalWrite(B1A, LOW);
              digitalWrite(B1B, LOW);}

            } else if (header.indexOf("GET /4/on") >= 0) {
              irValue = digitalRead(irPin);
            if (irValue == 0){
              Serial.println("CLOSING DOOR");
              //CLOSE;
              digitalWrite(A1B, LOW);
              analogWrite(A1A, 850);
              digitalWrite(B1A, LOW);
              analogWrite(B1B, 850);
              delay(3800);
              digitalWrite(A1A, LOW);
              digitalWrite(A1B, LOW);
              digitalWrite(B1A, LOW);
              digitalWrite(B1B, LOW);}

              //REFRESH-OFF

            } else if (header.indexOf("GET /4/off") >= 0) {
              digitalWrite(A1A, LOW);
              digitalWrite(A1B, LOW);
              digitalWrite(B1A, LOW);
              digitalWrite(B1B, LOW);

            }
                              //DOOR SENSOR STATE
            irValue = digitalRead(irPin);
            if (irValue == 0) {
              client.println("<p>Door State OPEN</p>");
            }
            else {
              client.println("<p>Door State CLOSED</p>");
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Motor Door Web Server</h1>");


            client.println("<p><a href=\"/5/on\"><button class=\"button\">Open</button></a></p>");

            client.println("<p><a href=\"/4/on\"><button class=\"button\">Close</button></a></p>");

            client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine

        }

      }

    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  //IS THERE A IR SIGNAL
  if (irrecv.decode(&results)) {

    switch (results.value) {
      // CODES FOR MEMOREX DVD PLAYER FF REW (BOTH) STOP
      case 0xFF11EE :
      irValue = digitalRead(irPin);
            if (irValue != 0){
        digitalWrite(A1A, LOW);
        analogWrite(A1B, 850);
        digitalWrite(B1B, LOW);
        analogWrite(B1A, 850);
        delay(5000);
        digitalWrite(A1A, LOW);
        digitalWrite(A1B, LOW);
        digitalWrite(B1A, LOW);
        digitalWrite(B1B, LOW);}
        break;

      case 0xFF13EC :
      irValue = digitalRead(irPin);
            if (irValue == 0){
        digitalWrite(A1B, LOW);
        analogWrite(A1A, 850);
        digitalWrite(B1A, LOW);
        analogWrite(B1B, 850);
        delay(3800);
        digitalWrite(A1A, LOW);
        digitalWrite(A1B, LOW);
        digitalWrite(B1A, LOW);
        digitalWrite(B1B, LOW);}
        break;
      case 0xFF09F6 :
      irValue = digitalRead(irPin);
            if (irValue != 0){
        digitalWrite(A1A, LOW);
        analogWrite(A1B, 850);
        digitalWrite(B1B, LOW);
        analogWrite(B1A, 850);
        delay(5000);
        digitalWrite(A1A, LOW);
        digitalWrite(A1B, LOW);
        digitalWrite(B1A, LOW);
        digitalWrite(B1B, LOW);}
        break;
      case 0xFF4BB4 :
      irValue = digitalRead(irPin);
            if (irValue == 0){
        digitalWrite(A1B, LOW);
        analogWrite(A1A, 850);
        digitalWrite(B1A, LOW);
        analogWrite(B1B, 850);
        delay(3800);
        digitalWrite(A1A, LOW);
        digitalWrite(A1B, LOW);
        digitalWrite(B1A, LOW);
        digitalWrite(B1B, LOW);}
        break;
      case 0xFF936C :
        digitalWrite(A1A, LOW);
        digitalWrite(A1B, LOW);
        digitalWrite(B1A, LOW);
        digitalWrite(B1B, LOW);
        break;
      case 0xFFC936 :
        digitalWrite(A1A, LOW);
        digitalWrite(A1B, LOW);
        digitalWrite(B1A, LOW);
        digitalWrite(B1B, LOW);
        break;
    }
 
 
    // GET THE NEXT IR VALUE
    irrecv.resume();
  }
  { ArduinoOTA.handle();


  }
  }
