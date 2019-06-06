#include <stdio.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Adafruit_GFX.h>    // Core graphics library by Adafruit
#include <Arduino_ST7789.h> // Hardware-specific library for ST7789 (with or without CS pin)
#include <SPI.h>

#define TFT_DC    16
#define TFT_RST   4 
#define TFT_MOSI  23   // for hardware SPI data pin (all of available pins)
#define TFT_SCLK  18   // for hardware SPI sclk pin (all of available pins)

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST);
char* text = "  WIFI&BLUE QUALCOMM\r\n\r\nSSID:M Router:HiLink\r\nIP:192.168.3.16\r\n";
bool flag=1;
static boolean Receive = false;
char message[24]="";
int Color[8]={RED,GREEN,BLUE,WHITE,YELLOW,BLACK,CYAN,MAGENTA};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Receive=true;
      int i =0;
      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (i = 0; i < value.length(); i++){
          Serial.print(value[i]);
          message[i] = value[i];
        }
        message[i]='\0';
        Serial.println("*********");
      }
    }
};

void setup(void) {
  Serial.begin(115200);
  
  Serial.print("This is designed to communicate to a raspbrrypi.");
  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to mongoose");
  Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");
  
  BLEDevice::init("mongoose");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("This is designed to show some information for pi.");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  
  tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels
  Serial.println("Initialized");
  uint16_t time = millis();
  tft.fillScreen(BLACK);
  time = millis() - time;
  Serial.println(time, DEC);
 
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setTextWrap(true);
  tft.print(text);

  tft.setCursor(0, 16);
  tft.print("  Hello,world.");
  tft.setCursor(0, 64);
  tft.print("message from iPhone");
  tft.setCursor(0, 224);
  tft.print("MADE BASED IN ESP32 ");
  tft.drawRect(0, 80, 240, 48, RED);  
  for(int i=1;i<5;i++){
      tft.drawFastHLine(0, i*16, tft.width(), WHITE);
      delay(10);   
  }
    
  delay(4000);
}

void loop() {
  int i;
  static short index=0; 

  if(Receive == true){
    Receive = false;
    tft.fillRect(1, 81, 238, 46, BLUE);
    tft.setCursor(0, 81);
    tft.print(message);
  }
  if(flag==1){
    flag=0;
    for(i=148;i<224;i++){
      tft.drawFastHLine(0, i, tft.width(), Color[index]);
      delay(10);   
    }
  }
  else{
    flag=1;
    for(i=223;i>=148;i--){
      tft.drawFastHLine(0, i, tft.width(), Color[index]);
      delay(10);   
    }
  }
  index++;
  if(index>7)
    index=0;
}

