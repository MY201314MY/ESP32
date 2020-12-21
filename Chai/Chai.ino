#include <stdio.h>
#include <Adafruit_GFX.h>   
#include <Arduino_ST7789.h> 
#include <SPI.h>
#include <SimpleDHT.h>

const int button_a = 32;      
const int button_b = 33;  
    

int pinDHT11 = 15;
SimpleDHT11 dht11(pinDHT11);

#define TFT_DC    2
#define TFT_RST   4 
#define TFT_MOSI  23 
#define TFT_SCLK  18
int Color[8]={RED,GREEN,BLUE,WHITE,YELLOW,BLACK,CYAN,MAGENTA};
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST);

void setup() {
  char* text = "     SMART Toilet";

  Serial.begin(115200);
  pinMode(button_a, INPUT_PULLUP);
  pinMode(button_b, INPUT_PULLUP);
  
  tft.init(240, 240);
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setTextWrap(true);
  tft.print(text);
  tft.setTextColor(RED);
  tft.setCursor(0, 224);
  tft.setTextColor(GREEN);
  tft.print("MADE BASED IN ESP32 ");
  tft.setTextColor(CYAN);
  tft.setCursor(0, 48);
  tft.print("NETWORK Initial OK!");
  delay(2000);
  tft.fillRect(0, 32, 240, 32, BLACK);
}

int a,b,backup[2];
unsigned long tsLastReport;
void loop() {
  byte temperature = 0;
  byte humidity = 0;
  char buff[32];
  
  if (millis() - tsLastReport > 2000) {
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
      return;
    }
    tft.setTextSize(3);
    sprintf(buff, " Temp:%02d*C", (int)temperature);
    tft.setCursor(0, 32);
    tft.setTextColor(RED);
    tft.fillRect(0, 32, 240, 32, BLACK);
    tft.println(buff);    

    sprintf(buff, " Humi:%02d H", (int)humidity);
    tft.setCursor(0, 64);
    tft.setTextColor(YELLOW);
    tft.fillRect(0, 64, 240, 32, BLACK);
    tft.println(buff);
    
    tsLastReport = millis();
  }

  a = digitalRead(button_a);
  b = digitalRead(button_b);

  if(backup[0] !=a){
      backup[0] = a;
      if(a==1){
        tft.fillRect(30, 120, 75, 60, GREEN);
      }else{
        tft.fillRect(30, 120, 75, 60, RED);
       }
    }

  if(backup[1] !=b){
      backup[b] = b;
      if(b==1){
        tft.fillRect(135, 120, 75, 60, GREEN);
      }else{
        tft.fillRect(135, 120, 75, 60, RED);
       }
    }
  
  backup[0] = a;
  backup[1] = b;
  delay(20);
}
