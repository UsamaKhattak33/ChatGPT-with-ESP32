// Libraries We are using
#include "MAX30100_PulseOximeter.h"
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <SPI.h>

// declaring variable
float BPM, SpO2; 
float temp;
/*Puting SSID & Password*/
const char* ssid = "ERROR404";  // Enter SSID here
const char* password = "qwerty1234";  //Enter Password here
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
PulseOximeter pox;
uint32_t tsLastReport = 0;

ESP8266WebServer server(80);

#define REPORTING_PERIOD_MS     1000
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() 
{
  Serial.begin(115200);
  mlx.begin();
  pinMode(16, OUTPUT);
  delay(100);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    display.display(); 
    delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(1);
  display.setCursor(0, 0);
  Serial.print("Initializing pulse oximeter..");
  Serial.println("Connecting to ");
  Serial.println(ssid);
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.enableCORS(true);
  server.begin();
  Serial.println("server started");
  if (!pox.begin())
  {
    Serial.println("FAILED");
    for (;;);
  } else 
  {
    Serial.println("SUCCESS");
  }
}
void loop() 
{
  server.handleClient();
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) 
  {
    BPM = round(pox.getHeartRate());
    SpO2 = round(pox.getSpO2());
    temp = round(mlx.readObjectTempC());
    Serial.print("BPM: ");
    Serial.println(BPM);
    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");
    Serial.print("Temperature ");
    Serial.print(temp);
    Serial.println("degree");
    Serial.println("*********************************");
    Serial.println();
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("HEART BPM: ");
    display.println(BPM);;
    display.setCursor(0, 10);
    display.print("PR: ");
    display.print(SpO2);
    display.setCursor(0,20);
    display.print("TEMP: ");
    display.print(temp);    
    display.display();
    tsLastReport = millis();
  }   
}

void handle_OnConnect() 
{
  String response = "{";
  response += "\"BPM\": \""+String(BPM)+"\"";
  response += ",\"SpO2\": \""+String(SpO2)+"\"";
  response += ",\"TEMPERATURE\": \""+String(temp)+"\"";
  response+="}";
  server.send(200, "text/json", response);
}

void handle_NotFound() 
{
  server.send(404, "text/plain", "Not found");
}
