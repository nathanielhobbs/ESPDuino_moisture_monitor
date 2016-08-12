/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "settings.h"

ESP8266WiFiMulti WiFiMulti;
int led_state = 0;

void setup() {
  Serial.begin(115200);  //connect to serial at 115200 bits/sec (baud=signal changes/sec)
  delay(10);

  // Connect to WiFi network
  //WiFiMulti.addAP("Goyoo", "f39ac7e2d0");  //work
  WiFiMulti.addAP(WIFI_SSID, WIFI_PSWD);   //home

   // set LEDs
  pinMode(0, OUTPUT); //green1
  pinMode(2, OUTPUT); //green2
  pinMode(4, OUTPUT); //green3
  pinMode(5, OUTPUT); //yellow
  pinMode(12, OUTPUT); //red


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
  int sensorValue = analogRead(A0);
  
  // Print log to serial
  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("sensor reading: ");
  Serial.println(sensorValue);

  // after adding water: 330  | 600..700   | 530..550 |  680  | 375..590 | 
  // before adding water: ??? | 1018..1024 | 960..970 | 958-9 |   1024   | 

  if(sensorValue > 800){
    digitalWrite(0, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(12, HIGH); 
    
    Serial.println("level 5");
  }
  else if(sensorValue > 600 && sensorValue < 800){
    digitalWrite(0, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(12, LOW);
    
    Serial.println("level 4");
  }
  else if(sensorValue > 400 && sensorValue < 600){
    digitalWrite(0, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(12, LOW);
    
    Serial.println("level 3");
  }
  else if(sensorValue > 230 && sensorValue < 400){
    digitalWrite(0, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(12, LOW);
    
    Serial.println("level 2");
  }
  else if(sensorValue > 175 && sensorValue < 230){
    digitalWrite(0, HIGH);
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(12, LOW);
    
    Serial.println("level 1");
  }
  else{
    digitalWrite(0, LOW);
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(12, LOW);
    
    Serial.println("level 0");
  }


  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  // try to establish tcp connection
  if (!client.connect(host, port)) {
      Serial.println("connection failed");
      Serial.println("wait 5 sec...");
      delay(5000);
      return;
  }

  // Send LED heartbeat request to the server
  client.println("POST /light HTTP/1.1");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: 7");
  client.println("");//need empty line before body
  if(led_state == 0){
    Serial.println("light on");
    
    // send POST to server turning on light
    client.println("state=1"); 
    
    // set led state
    led_state = 1;
  }else{
    Serial.println("light off");

    // send POST to server turning off light
    client.println("state=0");
    
     led_state = 0;
  }

  
  // POST moisture value to server
  client.println("POST /plants HTTP/1.1");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: 20");  //16 chars and up to 4 digits
  client.println(""); //need empty line before body
  client.print("name=1&moisture=");
  client.println(sensorValue);

  //read back one line from server
  String line = client.readStringUntil('\r');
  client.println(line);

  Serial.println("closing connection");
  client.stop();
  
  Serial.println("wait 2 sec...");
  delay(2000);
}

