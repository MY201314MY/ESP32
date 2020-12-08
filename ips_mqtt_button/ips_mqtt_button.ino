#include <stdio.h>
#include <Adafruit_GFX.h>   
#include <Arduino_ST7789.h> 
#include <SPI.h>
#include <SimpleDHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "iPhone";
const char* password = "XXXX";
const char* mqtt_server = "120.26.185.144";
WiFiClient espClient;
PubSubClient client(espClient);

const int button_a = 32;      
const int button_b = 33;      

int pinDHT11 = 15;
SimpleDHT11 dht11(pinDHT11);

const int HC_SR = 14;     // the number of the pushbutton pin
int HC_SR_State = 0;         // variable for reading the pushbutton status
uint32_t tsLastReport = 0;
char msg[64];

#define TFT_DC    2
#define TFT_RST   4 
#define TFT_MOSI  23 
#define TFT_SCLK  18
int Color[8]={RED,GREEN,BLUE,WHITE,YELLOW,BLACK,CYAN,MAGENTA};
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    
  } else {
   
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-SMART-Toilte";
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("toilte", "hello world");
      // ... and resubscribe
      //client.subscribe("toilte");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  char* text = "     SMART Toilet";

  Serial.begin(115200);
  pinMode(button_a, INPUT_PULLUP);
  pinMode(button_b, INPUT_PULLUP);

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  pinMode(HC_SR, INPUT);
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

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  char buff[32];

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (millis() - tsLastReport > 5000) {
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

    sprintf(msg, "{\"temperature\":%02d,\"humidity\":%02d}", (int)temperature, (int)humidity);
    client.publish("toilte", msg);
    
    tsLastReport = millis();
  }

  HC_SR_State = digitalRead(HC_SR);

  /*if (HC_SR_State == HIGH) {
    Serial.println("High"); 
    tft.fillRect(60, 120, 120, 60, RED);
  } else {
    Serial.println("Low"); 
    tft.fillRect(60, 120, 120, 60, CYAN);
  }*/
  a = digitalRead(button_a);
  b = digitalRead(button_b);

  if(backup[0] !=a){
      backup[0] = a;
      if(a==1){
        tft.fillRect(30, 120, 75, 60, GREEN);
        client.publish("toilte", "L:on");
      }else{
        tft.fillRect(30, 120, 75, 60, RED);
        client.publish("toilte", "L:off");
       }
    }

  if(backup[1] !=b){
      backup[b] = b;
      if(b==1){
        tft.fillRect(135, 120, 75, 60, GREEN);
        client.publish("toilte", "R:on");
      }else{
        tft.fillRect(135, 120, 75, 60, RED);
        client.publish("toilte", "R:off");
       }
    }
  
  backup[0] = a;
  backup[1] = b;
  delay(20);
}
