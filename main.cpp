#include <SPI.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#define RELAY_PIN D3  
#define DHTTYPE DHT11
#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1  
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define DHT_SENSOR_PIN  D5 
#define DHT_SENSOR_TYPE DHT11

const char *ssid     = "";
const char *password = "";

const char* www_username = "";
const char* www_password = "";

const char* PARAM_INPUT_1 = "state";
int relay_state = LOW;


ESP8266WebServer server(80); // Web server on port 80

float Temperature;
float Humidity;


Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


DHT dht(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);


const long utcOffsetInSeconds = 12600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


const int temp = D7;
int temp_prev_button_state = HIGH; 
int temp_button_state;

const int clk = D4;
int clk_prev_button_state = HIGH; 
int clk_button_state;

const int sw = D1;
int sw_prev_button_state = HIGH; 
int sw_button_state;

const int sww = D6;
int sww_prev_button_state = HIGH; 
int sww_button_state;

  

void setup()   {
  WiFi.begin(ssid, password);
  Serial.begin(9600);
  timeClient.begin();
  dht.begin();
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, relay_state);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );}

  // home page
  server.on("/",  [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
  Temperature = dht.readTemperature(); 
  Humidity = dht.readHumidity();
  server.send(200, "text/html", getHTML(Temperature,Humidity)); 
  });

  // Route to control the Relay
  server.on("/zzzxxx9090/on", HTTP_GET, []() {
    relay_state = HIGH;
    digitalWrite(RELAY_PIN, relay_state);
    server.send(200, "text/html", getHTML(Temperature,Humidity));
  });
  server.on("/zzzxxx9090/off", HTTP_GET, []() {
    relay_state = LOW;
    digitalWrite(RELAY_PIN, relay_state);
    server.send(200, "text/html", getHTML(Temperature,Humidity));
  });

  // Start the server
  server.begin();


  pinMode(temp, INPUT_PULLUP);
  pinMode(sw, INPUT_PULLUP);
  pinMode(sww, INPUT_PULLUP);


  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
 
  display.display();
  delay(2000);

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.write("Alphabet Room");
  display.println();
  display.println();
  display.println("Wi-Fi:");
  display.println(WiFi.SSID());
  display.println(WiFi.localIP());
  display.println(WiFi.BSSIDstr());
  display.println();
  display.println(WiFi.hostname());
  display.display();
  delay(2000);
  display.clearDisplay();

}

String getHTML(float Temperature, float Humidity) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="<script>\n";
  ptr +="setInterval(loadDoc,800);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.getElementById(\"webpage\").innerHTML =this.responseText}\n";
  ptr +="};\n";
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
  ptr +="</script>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>Alphabet Room</h1>\n";
  ptr += "<p>Relay state: <span style='color: red;'>";
  if (relay_state == LOW)
    ptr += "OFF";
  else
    ptr += "ON";
  ptr += "</span></p>";
  ptr += "<a href='/zzzxxx9090/on'>Turn ON</a>";
  ptr += "<br><br>";
  ptr += "<a href='/zzzxxx9090/off'>Turn OFF</a>";
  ptr +="<p>Temperature: ";
  ptr +=(float)Temperature;
  ptr +="°C</p>";
  ptr +="<p>Humidity: ";
  ptr +=(int)Humidity;
  ptr +="%</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void loop() {
    server.handleClient();
    //temp button
    temp_button_state = digitalRead(temp);
    if(temp_prev_button_state == LOW && temp_button_state == HIGH){
      for (int i = 0; i < 10; i++) { 
       float humi  = dht.readHumidity();
       float temp = dht.readTemperature();
       Serial.println("temp");
       display.setTextSize(1);
       display.setTextColor(SH110X_WHITE);
       display.setCursor(0, 0);
       display.write("Temp And Humi:");
       display.setTextSize(2);
       display.println();
       display.println();
       display.println(temp);
       display.println(humi);
       display.display();
       display.clearDisplay();
       delay(1000);
       
      }
    }
    temp_prev_button_state = temp_button_state;

   //clock button
   clk_button_state = digitalRead(clk);
   if(clk_prev_button_state == LOW && clk_button_state == HIGH){
       timeClient.update();
       Serial.println("clock");
       display.setTextSize(1);
       display.setTextColor(SH110X_WHITE);
       display.setCursor(0, 0);
       display.write("NTP-CLock:");
       display.println("");
       display.println("");
       display.setTextSize(2);
       display.println(daysOfTheWeek[timeClient.getDay()]);
       display.println("");
       display.print("  ");
       display.print(timeClient.getHours());
       display.print(":");
       display.print(timeClient.getMinutes());
       display.print(":");
       display.println(timeClient.getSeconds());
       display.display();
       display.clearDisplay();
       }
   clk_prev_button_state = clk_button_state;
   
   //on button
   sw_button_state = digitalRead(sw);
   if(sw_prev_button_state == LOW && sw_button_state == HIGH){
       digitalWrite(D3, HIGH);
       display.setTextSize(1);
       display.setCursor(0, 0);
       display.write("Light/on");
       display.setTextColor(SH110X_WHITE);
       display.display();
       display.clearDisplay();
       }
   sw_prev_button_state = sw_button_state;

   //off button
   sww_button_state = digitalRead(sww);
   if(sww_prev_button_state == LOW && sww_button_state == HIGH){
       digitalWrite(D3, LOW);
       display.setTextSize(1);
       display.setCursor(0, 0);
       display.write("Light/off");
       display.setTextColor(SH110X_WHITE);
       display.display();
       display.clearDisplay();
       }
   sww_prev_button_state = sww_button_state;
}
