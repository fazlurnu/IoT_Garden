int myPin = 5;

void setup() {
  // initialize digital pin 13 as an output.
  Serial.begin(115200);
  delay(100);
  pinMode(myPin, OUTPUT);
  digitalWrite(myPin, LOW);    // turn the LED off by making the voltage LOW
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(myPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.print("On");
  delay(1000);              // wait for a second
  digitalWrite(myPin, LOW);    // turn the LED off by making the voltage LOW
  Serial.print("Off");
  delay(1000);              // wait for a second
}
