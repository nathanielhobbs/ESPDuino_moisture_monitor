/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>   //for connection to wifi
#include <ESP8266HTTPClient.h>  //for sending HTTP requests
#include "settings.h"

// prep to store MAC address 
uint8_t MAC_array[6];
char MAC_char[18];

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
char* plant_name = "Happiness Tree (幸福树)";
int led_state = 0;
int pump_iterations = 0;
const int SENSOR_MAX = 1024;
int DEFAULT_MOISTURE_LEVELS[] = {300, 400, 500, 700, 900};
int moisture_levels[] = {300, 400, 500, 700, 900};
int num_moisture_levels = 5;
int connection_attempts = 0;
int iterations_since_post = 0;

// copy elements from arr2 into arr1
void copyIntArray(int arr1[], int arr2[], int length) {
  for (int i = 0; i < length; i++) {
    arr1[i] = arr2[i];
  }
}

void setup() {
  Serial.begin(115200);  //connect to serial at 115200 bits/sec (baud=signal changes/sec)

  // set MAC address
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array); ++i){
    sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
  }

  // set digital pins for leds and pump
  pinMode(GREEN1, OUTPUT); //green1
  pinMode(GREEN2, OUTPUT); //green2
  pinMode(GREEN3, OUTPUT); //green3
  pinMode(YELLOW, OUTPUT); //yellow
  pinMode(RED, OUTPUT); //red
  pinMode(BLUE, OUTPUT);//blue
  pinMode(RELAY, OUTPUT);

  //set moisture levels to default values
  copyIntArray(moisture_levels, DEFAULT_MOISTURE_LEVELS, num_moisture_levels);


  // Connect to WiFi network
  WiFiMulti.addAP(WIFI_SSID, WIFI_PSWD);  
   
  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}

boolean between(int value, int min, int max) {
  return value >= min && value <= max;
}

// if moisture sensor passes threshhold, turn on pump for 5 sec
// allow a maximum of 3 times running pump above threshhold, otherwise
// don't allow pump to run (in case e.g. sensor breaks and value always above threshhold)
int handlePump(int sensor_value, int pump_iterations) {
  if (between(sensor_value, moisture_levels[4], SENSOR_MAX) && pump_iterations < 3) {
    digitalWrite(RELAY, HIGH); //Turns ON Relay
    Serial.println("Pump ON");
    Serial.println(pump_iterations);
    delay(10000); //wait 10 seconds
    digitalWrite(RELAY, LOW); //Turns OFF Relay
    Serial.println("Pump OFF");
    return pump_iterations+1;
  }
  //reset pump iterations if under between green and yellow
  else if (sensor_value < (moisture_levels[2]+moisture_levels[3])/2) { 
    return pump_iterations = 0;
  }
  else
    return pump_iterations;
}

void handleSensorLights(int sensor_value) {
  // after adding water: 330  | 600..700   | 530..550 |  680  | 375..590 |
  // before adding water: ??? | 1018..1024 | 960..970 | 958-9 |   1024   |

  if (between(sensor_value, moisture_levels[4], SENSOR_MAX)) {
    digitalWrite(GREEN1, HIGH);
    digitalWrite(GREEN2, HIGH);
    digitalWrite(GREEN3, HIGH);
    digitalWrite(YELLOW, HIGH);
    digitalWrite(RED, HIGH);

    Serial.println("level 5");
  }
  else if (between(sensor_value, moisture_levels[3], moisture_levels[4])) {
    digitalWrite(GREEN1, HIGH);
    digitalWrite(GREEN2, HIGH);
    digitalWrite(GREEN3, HIGH);
    digitalWrite(YELLOW, HIGH);
    digitalWrite(RED, LOW);

    Serial.println("level 4");
  }
  else if (between(sensor_value, moisture_levels[2], moisture_levels[3])) {
    digitalWrite(GREEN1, HIGH);
    digitalWrite(GREEN2, HIGH);
    digitalWrite(GREEN3, HIGH);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);

    Serial.println("level 3");
  }
  else if (between(sensor_value, moisture_levels[1], moisture_levels[2])) {
    digitalWrite(GREEN1, HIGH);
    digitalWrite(GREEN2, HIGH );
    digitalWrite(GREEN3, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);

    Serial.println("level 2");
  }
  else if (between(sensor_value, moisture_levels[0], moisture_levels[1])) {
    digitalWrite(GREEN1, HIGH);
    digitalWrite(GREEN2, LOW);
    digitalWrite(GREEN3, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);

    Serial.println("level 1");
  }
  else {
    digitalWrite(GREEN1, LOW);
    digitalWrite(GREEN2, LOW);
    digitalWrite(GREEN3, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);

    Serial.println("level 0");
  }
}

int heartBeat(int led_state) {
  if (led_state == 0) {
    Serial.println("heartbeat light on");
    digitalWrite(BLUE, HIGH);
    return led_state = 1;
  } else {
    Serial.println("heartbeat light off");
    digitalWrite(BLUE, LOW);
    return led_state = 0;
  }
}

int countDigits(int number){
  if(number > 0){
    return floor (log10 (abs (number))) + 1;
  }
  return 0;
}

void postSensorValue(char *host, uint16_t port, char *url, int sensor_value) {

  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  http.begin(host, port, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String post_contents =  String("mac=" + String(MAC_char) + "&moisture=" + String(sensor_value));  
  Serial.print("[HTTP] POST...\n");
  int httpCode = http.POST(post_contents);
//  http.writeToStream(&Serial);
  http.end();

  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // connected successfully, so reset connection_attempts
      connection_attempts = 0; //keeping track so can take actions when out of touch with server for too long

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
      }
  } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

//  http.end();

}

void postToQoncrete(String host, uint16_t port, String url, int sensor_value) {
  HTTPClient http;
  
  http.begin(host, port, url, port==443?true:false, String("C6:C2:A4:61:E9:4E:02:DF:DA:D3:18:7B:B8:34:0F:71:9C:41:76:2B"));
  http.addHeader("Content-Type", "application/json");
  Serial.print("[HTTP] POST to Qoncrete...\n");
  String json = String("{\"mac\":\"" + String(MAC_char) + "\", \"moistureReading\":" + sensor_value + "}");
  int httpCode = http.POST(json);
  http.writeToStream(&Serial);  
  http.end();
 }

void loop() {
  int sensor_value = analogRead(A0);

  // Print sensor value
  Serial.print("sensor reading: ");
  Serial.println(sensor_value);

  // Adjust LED lights according to sensor value
  handleSensorLights(sensor_value);

  // Turn on pump if necessary
  Serial.println(pump_iterations);
  pump_iterations = handlePump(sensor_value, pump_iterations);
  Serial.println(pump_iterations);

  //--------------- Send update to server ------------------
  // format "http://host:port/url"
  uint16_t port = HOST_PORT;
  char *host = HOST_IP; // ip address or host name
  char *url = HOST_URL;     // path
  
  Serial.printf("connecting to http%s://%s:%d%s => ", port==443?"s":"", host, port, url);


  // wait for WiFi connection
  if((WiFiMulti.run() == WL_CONNECTED)) {
    // change led state to show successful wifi connection
    led_state = heartBeat(led_state);

    // POST moisture value to server every 5 iterations
    if(iterations_since_post++ > 5){
      postSensorValue(host, port, url, sensor_value); 

      uint16_t q_port = QONCRETE_HOST_PORT;
      String q_host = QONCRETE_HOST_ADDRESS; // ip address or host name
      String q_url = QONCRETE_HOST_URL;     // path
      postToQoncrete(q_host, q_port, q_url, sensor_value);
      iterations_since_post = 0;
    }
  }
}

