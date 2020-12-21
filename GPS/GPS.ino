#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>

String longitude,latitude;
unsigned short sendCommand(char *Command, char *Response, unsigned char Retry)
{
  for (unsigned char n = 0; n < Retry; n++)
  {
    Serial.print("\r\n---------send AT Command:---------\r\n");
    Serial.println(Command);

    Serial2.write(Command);
    delay(500);
    unsigned char i = 0;
    char message[256];
    while (Serial2.available()) {
      message[i++] = char(Serial2.read());
    }
    message[i]='\0';
    if (strstr(message, Response) != NULL)
    {
      Serial.print("\r\n==========receive AT Command:==========\r\n");
      Serial.print(message);
      return 0;
    }
    else{
      Serial.print("\r\n=========== unknown errors ============\r\n");
      Serial.print(message);
    }
    delay(1000);
  }
  return 1;
}
unsigned char GPS(){
  int i = 0;
  char message[1024] = "";
  static unsigned int grade = 0;
  Serial2.write("AT+QGNSSRD=\"NMEA/RMC\"\r\n");
  delay(500);
  while (Serial2.available()) {
    message[i++] = char(Serial2.read());
  }
  Serial.println(message);
  String m = String(message);
  if((m.indexOf(",N,") != -1) && (m.indexOf(",E,") != -1)){
    Serial.println("We get GPS position successfully.");
    int p = m.indexOf("+QGNSSRD: $GNRMC");
    int o = m.indexOf("\r\n\r\nOK");
      
    unsigned char buff[16];
    grade++;
    sprintf((char*)buff,"%06d", grade);
    oled.setTextXY(2,7);              
    oled.putString((char*)buff);
          
    String y = m.substring(p, o);
    int po = y.indexOf("$GNRMC");
    String result = y.substring(po, y.length());

    int m = result.indexOf('A');
    result = result.substring((m+2), result.length());
    m = result.indexOf(',');
    latitude = result.substring(0, m);
    Serial.println("****************");
    Serial.print("latitude:");
    Serial.print(latitude);
    Serial.print("  ----  ");

    m = result.indexOf('N');
    result = result.substring((m+2), result.length());
    m = result.indexOf(',');
    longitude = result.substring(0, m);
    Serial.print("longitude:");
    Serial.println(longitude);
    Serial.println("****************");

    oled.setTextXY(3,3);              
    oled.putString(longitude);
    oled.setTextXY(4,4);              
    oled.putString(latitude);
    return 0;     
  }
    else{
      Serial.println("Can't get GPS signal...");
      return 1;
    }
}
void setup() {
  Wire.begin();  
  oled.init();  
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.println("NB-IOT & GPS Example.\r\nLet's go!");
                     
  oled.clearDisplay();             
  oled.setTextXY(0,0);              
  oled.putString("    MC20E&GPS");
  oled.setTextXY(2,0);              
  oled.putString("Upload:");
              
  oled.setTextXY(3,0);              
  oled.putString("Lo:00.00");
  oled.setTextXY(4,0);              
  oled.putString("La:00.00");             
  if (sendCommand("AT\r\n", "OK\r\n", 10) == 0){
    Serial.println("MC20E get ready. . .");
  }
  if (sendCommand("AT+CGREG?\r\n", "+CGREG: 0,1\r\n\r\nOK", 30) == 0){
    Serial.println("MC20E has connect to Network successfully. . .");
  }
  if (sendCommand("AT+QGNSSC?\r\n", "+QGNSSC: 1\r\n\r\nOK", 1) == 0){
    Serial.println("MC20E's GPS has been opened already. . .");
  }
  else if (sendCommand("AT+QGNSSC=1\r\n", "OK\r\n", 1) == 0){
    Serial.println("MC20E's GPS was opened successfully. . .");
  }
  else{
    
  }
  delay(2000);
}

void loop() {
  GPS();
  delay(5000);
}

