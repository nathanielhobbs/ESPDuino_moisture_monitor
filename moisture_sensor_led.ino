

void setup() {
  // initialize serial connection at 9600 bits per second
  Serial.begin(9600); 

  // set LEDs
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  pinMode(13, OUTPUT);

  Serial.println("setup finished, going into loop");
}

void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  // read input from analog pin 0
  int sensorValue = analogRead(A0);

  // after adding water: 330
  // before adding water: ???

  if(sensorValue > 450){
    digitalWrite(7, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    Serial.println("red!");
  }
  else if(sensorValue > 400 && sensorValue < 450){
    digitalWrite(7, LOW);
    digitalWrite(6, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    Serial.println("yellow");
  }
  else if(sensorValue > 400 && sensorValue < 450){
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    Serial.println("green3");
  }
  else if(sensorValue > 350 && sensorValue < 400){
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    Serial.println("green2");
  }
  else if(sensorValue > 300 && sensorValue < 350){
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
    Serial.println("green1");
  }
  else{
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    digitalWrite(3, LOW);
  }
  Serial.println("sensor reading: " + sensorValue);
  
  delay(100);
  
}
