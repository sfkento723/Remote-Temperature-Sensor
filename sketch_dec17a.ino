#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>

int tmpAddress = 0x4F; //Slave Addresses set
int ResolutionBits = 10;   //Resolution set
String output;

/* Put your SSID & Password */

const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

void setup() 
{
  Wire.begin();
  Serial.begin(115200);

  SetResolution();
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}
void loop() 
{
  getTemperature();
  server.handleClient();
  
}

void handle_OnConnect() 
{
  server.send(200, "text/html", output); 
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

float getTemperature()
{
  Wire.requestFrom(tmpAddress,2);
  
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  
  int TemperatureSum = ((MSB << 8) | LSB) >> 4;
  
  float celsius = TemperatureSum*0.0625;
  float fahrenheit = (celsius * 1.8) + 32;
  
  output = "Celsius: ";
  output += celsius;
  output += "\nFahrenheit: ";
  output += fahrenheit;
  
  Serial.print("Celsius: ");
  Serial.println(celsius);
  Serial.print("\nFahrenheit: ");
  Serial.println(fahrenheit);
}

void SetResolution()
{
  if (ResolutionBits < 9 || ResolutionBits > 5) exit;
  Wire.beginTransmission(tmpAddress);
  Wire.write(B00000001); //addresses the configuration register
  Wire.write((ResolutionBits-9) << 5); //writes the resolution bits
  Wire.endTransmission();
  Wire.beginTransmission(tmpAddress); //resets to reading the temperature
  Wire.write((byte)0x00);
  Wire.endTransmission();
}
