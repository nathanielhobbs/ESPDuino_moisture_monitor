/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "settings.h"

//map digital pin numbers to words
#define GREEN1 0
#define GREEN2 2
#define GREEN3 4
#define YELLOW 5
#define RED 12
#define BLUE 13
#define RELAY 16

ESP8266WiFiMulti WiFiMulti;
WiFiClient client; // Use WiFiClient class to create TCP connections
int led_state = 0;
const int SENSOR_MAX = 1023;

void setup() {
  Serial.begin(115200);  //connect to serial at 115200 bits/sec (baud=signal changes/sec)
  delay(10);

  // Connect to WiFi network
  //WiFiMulti.addAP("Goyoo", "f39ac7e2d0");  //work
  WiFiMulti.addAP(WIFI_SSID, WIFI_PSWD);   //home

   // set LEDs
  pinMode(GREEN1, OUTPUT); //green1
  pinMode(GREEN2, OUTPUT); //green2
  pinMode(GREEN3, OUTPUT); //green3
  pinMode(YELLOW, OUTPUT); //yellow
  pinMode(RED, OUTPUT); //red
  pinMode(BLUE, OUTPUT);//blue

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void loop() {
  const uint16_t port = HOST_PORT;
  const char *host = HOST_IP; // ip or dns
  int sensor_value = analogRead(A0);
  
  // Print log to serial
  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("sensor reading: ");
  Serial.println(sensor_value);

  handleLights(sensor_value);
  handlePump(sensor_value);
  
  // try to establish tcp connection
  if (!client.connect(host, port)) {
      Serial.println("connection failed");
      Serial.println("wait 5 sec...");
      delay(5000);
      return;
  }

  // Send LED heartbeat request to the server
  heartBeat(led_state);

  
  // POST moisture value to server
  postSensorValue(sensor_value);

  Serial.println("closing connection");
  client.stop();
  
  Serial.println("wait 2 sec...");
  delay(2000);
}

void handlePump(int sensor_value){
  if(between(sensor_value, 800, SENSOR_MAX)){
    digitalWrite(RELAY, LOW); //Turns ON Relay
    Serial.println("Pump ON");
    delay(5000); //wait 5 seconds
    digitalWrite(RELAY, HIGH); //Turns OFF Relay
    Serial.println("Pump OFF");
  }
}

void handleLights(int sensor_value){
  // after adding water: 330  | 600..700   | 530..550 |  680  | 375..590 | 
  // before adding water: ??? | 1018..1024 | 960..970 | 958-9 |   1024   | 

  if(between(sensor_value, 800, SENSOR_MAX)){
    digitalWrite(RED, HIGH); 
    
    Serial.println("level 5");
  }
  else if(between(sensor_value, 600, 800)){
    digitalWrite(YELLOW, HIGH);
    digitalWrite(RED, LOW);
    
    Serial.println("level 4");
  }
  else if(between(sensor_value, 400, 600)){
    digitalWrite(GREEN3, HIGH);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);
    
    Serial.println("level 3");
  }
  else if(between(sensor_value, 230, 400)){
    digitalWrite(GREEN2, HIGH);
    digitalWrite(GREEN3, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);
    
    Serial.println("level 2");
  }
  else if(between(sensor_value, 175, 230)){
    digitalWrite(GREEN1, HIGH);
    digitalWrite(GREEN2, LOW);
    digitalWrite(GREEN3, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);
    
    Serial.println("level 1");
  }
  else{
    digitalWrite(GREEN1, LOW);
    digitalWrite(GREEN2, LOW);
    digitalWrite(GREEN3, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);
    
    Serial.println("level 0");
  }
}

void heartBeat(int led_state){
  client.println("POST /light HTTP/1.1");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: 7");
  client.println("");//need empty line before body
  if(led_state == 0){
    Serial.println("light on");
    digitalWrite(BLUE, HIGH);
    
    // send POST to server turning on light
    client.println("state=1"); 
    
    // set led state
    led_state = 1;
  }else{
    Serial.println("light off");
    digitalWrite(BLUE,LOW);

    // send POST to server turning off light
    client.println("state=0");
    
     led_state = 0;
  }
}

void postSensorValue(int sensor_value){
  client.println("POST /plants HTTP/1.1");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: 20");  //16 chars and up to 4 digits
  client.println(""); //need empty line before body
  client.print("name=1&moisture=");
  client.println(sensor_value);

  //read back one line from server
  String line = client.readStringUntil('\r');
  client.println(line);
}

boolean between(int value, int min, int max){
  return value >= min && value <=max;
}

