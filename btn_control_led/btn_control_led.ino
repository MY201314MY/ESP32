const int LED = 2;
const int buttonPin = 14;     // the number of the pushbutton pin
int buttonState = 0;          // variable for reading the pushbutton status
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    Serial.println("High");
    digitalWrite(LED, HIGH); 
  } else {
    Serial.println("Low");
     digitalWrite(LED, LOW); 
  }
  delay(1000);
}
