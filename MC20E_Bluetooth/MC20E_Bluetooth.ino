HardwareSerial mySerial(1);
char response[256];

short Set(char *Command, char *Response) {
  Serial.print("\r\n---------send AT Command:---------\r\n");
  Serial.println(Command);

  mySerial.write(Command);
  mySerial.write("\r\n");
  
  unsigned char i = 0;

  long lastmoment=millis();
  while(mySerial.available()<=0) {
    long now = millis();
    delay(100);
    Serial.print(". ");
    if(now-lastmoment>2000) {
      Serial.println("There're something wrong with MC20E, trying restart the module. . .");
      return -1;
    }
  }
  Serial.println();
  char message[256];
  while (mySerial.available()) {
    message[i++] = char(mySerial.read());
  }
  message[i]='\0';
  if (strstr(message, Response) != NULL)
  {
    Serial.print("\r\n==========receive AT Command:==========\r\n");
    Serial.print(message);
    return 0;
  }else {
    Serial.print("\r\n=========== unknown errors ============\r\n");
    Serial.print(message);
  }
  
  return 1;
}

short Query(char *Command) {  

  mySerial.write(Command);
  mySerial.write("\r\n");
    

  unsigned char i = 0;

  long lastmoment=millis();
  while(mySerial.available()<=0) {
    long now = millis();
    delay(100);
    Serial.print(". ");
    if(now-lastmoment>2000) {
      Serial.println("There're something wrong with MC20E, trying restart the module. . .");
      
      return -1;
    }
  }
  Serial.println();
  while (mySerial.available()) {
    response[i++] = char(mySerial.read());
  }
  response[i]='\0';
  Serial.print(response);
  return 0;
}

void MC20E_Bluetooth_init(){
  while (Set("AT", "\r\nOK") != 0){
    Serial.print(".");
    delay(2000);
  }
  Serial.println();
  Serial.println("MC20E get ready. . .");
  if(Query("AT+QBTPWR?") == 0){
    if (strstr(response, "+QBTPWR: 1\r\n\r\nOK") != NULL) {
      Serial.println("MC20E's Bluetooth has been opened already. . .");
    } else {
      if (Set("AT+QBTPWR=1\r\n", "OK\r\n") == 0){
        Serial.println("Bluetooth was turned on.");
      }
    }
  }
}
void setup() {
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 22, 23);
  MC20E_Bluetooth_init();
}
void loop() {
 
}


















/*HardwareSerial mySerial(1);
 
void setup() {
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 5, 19);
}

void loop() {
  if (mySerial.available()) {
    int inByte = mySerial.read();
    Serial.write(inByte);
  }

  if (Serial.available()) {
    int inByte = Serial.read();
    mySerial.write(inByte);
  }
}*/
