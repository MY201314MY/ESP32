#include <stdio.h>
#include <Adafruit_GFX.h>    
#include <Arduino_ST7789.h>
#include <SPI.h>

#define TFT_DC    16
#define TFT_RST   4 
#define TFT_MOSI  23  
#define TFT_SCLK  18   
int Color[8]={RED,GREEN,BLUE,WHITE,YELLOW,BLACK,CYAN,MAGENTA};
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST);
HardwareSerial mySerial(1);

struct MC20E{
  bool GPS_flag;
  String longitude;
  String latitude;
  String moment;
  unsigned int count;
};

struct MC20E iMessage;

unsigned short sendCommand(char *Command, char *Response, unsigned char Retry)
{
  for (unsigned char n = 0; n < Retry; n++)
  {
    Serial.print("\r\n---------send AT Command:---------\r\n");
    Serial.println(Command);

    mySerial.write(Command);
    delay(500);
    unsigned char i = 0;
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
    }
    else{
      Serial.print("\r\n=========== unknown errors ============\r\n");
      Serial.print(message);
    }
    delay(1000);
  }
  return 1;
}
unsigned char MC20E_GPS_Query(){
  int i = 0;
  char message[1024] = "";
  static unsigned int grade = 0;
  mySerial.write("AT+QGNSSRD=\"NMEA/RMC\"\r\n");
  delay(500);
  while (mySerial.available()) {
    message[i++] = char(mySerial.read());
  }
  String m = String(message);
  if((m.indexOf(",N,") != -1) && (m.indexOf(",E,") != -1)){
    iMessage.GPS_flag=true;
    int a = m.indexOf(",A,");
    int b = m.indexOf(",N,");
    int c = m.indexOf(",E,");
    int d = m.indexOf("$GNRMC");

    iMessage.moment = m.substring(d+7, a-4);
    Serial.print("time:");
    Serial.print(iMessage.moment);
    Serial.print("  ----  ");
    
    iMessage.latitude = m.substring(a+3, b);
    Serial.print("latitude:");
    Serial.print(iMessage.latitude);
    Serial.print("  ----  ");

    iMessage.longitude = m.substring(b+3, c);
    Serial.print("longitude:");
    Serial.println(iMessage.longitude);
    Serial.println("****************");

    char buff[32];
    iMessage.count++;
    sprintf(buff, "Count:%06d", iMessage.count);
    tft.fillRect(8, 88, 144, 16, BLACK);
    tft.setCursor(8, 88);
    tft.print(buff);

    tft.fillRect(8, 108, 144, 16, BLACK);
    tft.setCursor(8, 108);
    tft.print("time:"+iMessage.moment);
    
    tft.fillRect(8, 128, 144, 16, BLACK);
    tft.setCursor(8, 128);
    tft.print("E:" + iMessage.longitude);

    tft.fillRect(8, 148, 144, 16, BLACK);
    tft.setCursor(8, 148);
    tft.print("N:0" + iMessage.latitude);
    
  }else{
    Serial.println("Query error!");
  }
}
unsigned short MQTT_CMD(char *Command, char *Response, bool Publish)
{
  Serial.print("\r\n---------send AT Command:---------\r\n");
  Serial.println(Command);

  mySerial.write(Command);
  if(Publish == true){
    delay(200);
    mySerial.write(0x1A);
  }
  bool flag = false;
  unsigned char i = 0;
  char message[256]={0};

  while(1){
    while (mySerial.available()) {
      char character = char(mySerial.read());
      message[i++] = character;
      if(strstr(message, Response) != NULL){
        Serial.println("ACK");
        flag = true;
        Serial.print("\r\n==========receive AT Command:==========\r\n");
        Serial.print(message);
        return 0;
      }
      if(flag){
        message[i]='\0';
        break;
      }
    }
    if(flag){
      break;
    }
  }
  while(Serial2.available()){
    Serial2.read();
  }
}
void MC20E_init(){
  iMessage.GPS_flag=false;
  iMessage.count=0;
  
  if (sendCommand("AT\r\n", "OK\r\n", 10) == 0){
    Serial.println("MC20E get ready. . .");
  }
  if (sendCommand("AT+QGNSSC?\r\n", "+QGNSSC: 1\r\n\r\nOK", 1) == 0){
    Serial.println("MC20E's GPS has been opened already. . .");
    tft.setCursor(0, 32);
    tft.setTextColor(CYAN);
    tft.print("GPS Initial OK!");
  }
  else if (sendCommand("AT+QGNSSC=1\r\n", "OK\r\n", 1) == 0){
    tft.setCursor(0, 32);
    tft.print("GPS Initial OK!");
  }
  else{
    
  }
  if (sendCommand("AT+CGREG?\r\n", "+CGREG: 0,1\r\n\r\nOK", 30) == 0){
    Serial.println("MC20E has connect to Network successfully. . .");
     tft.setCursor(0, 48);
     tft.print("NETWORK Initial OK!");
  }
  if (MQTT_CMD("AT+QMTCFG=\"VERSION\",0,1\r\n", "OK", false) == 0){
    Serial.println("Set MC20E's protocol. . .");
  }
  /*if (MQTT_CMD("AT+QMTOPEN=0,\"183.230.40.39\",6002\r\n", "+QMTOPEN: 0,0",false) == 0){
    Serial.println("Try to connect OneNET");
  }
  if (MQTT_CMD("AT+QMTCONN=0,\"47975479\",\"179383\",\"r6VsaTGQVMeYmOujP4xkHFVCXvo=\"\r\n", "+QMTCONN: 0,0,0", false) == 0){
    Serial.println("MC20E connect to ONENET Successfully.");
  }*/
}
void TFT_ST7789_init(){
  char* text = "  MC20E's GPRS&GPS";
  tft.init(240, 240);
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setTextWrap(true);
  tft.print(text);
  tft.setTextColor(RED);
  tft.drawRect(0, 80, 240, 112, RED);
  tft.setCursor(0, 224);
  tft.setTextColor(GREEN);
  tft.print("MADE BASED IN ESP32 ");
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 5, 19);
  TFT_ST7789_init();
  MC20E_init();
  delay(2000);
  tft.fillRect(0, 32, 240, 32, BLACK);
}

void loop() {
  MC20E_GPS_Query();
  if(iMessage.GPS_flag == true){
    if (MQTT_CMD("AT+QMTPUB=0,0,0,0,\"mteaching\"\r\n", ">", false) == 0){
        Serial.println("Open the topic successfully.");
        String location = iMessage.longitude+"-"+iMessage.latitude+"\r\n";
        char cArr[location.length() + 1];
        location.toCharArray(cArr,location.length() + 1);
        if (MQTT_CMD(cArr, "+QMTPUB: 0,0,0", true) == 0){
          Serial.println("Send the data successfully.");
        }
      }
  }
  delay(5000);
}


/*
if (mySerial.available()) {
    int inByte = mySerial.read();
    Serial.write(inByte);
  }

  if (Serial.available()) {
    int inByte = Serial.read();
    mySerial.write(inByte);
  }
*/
