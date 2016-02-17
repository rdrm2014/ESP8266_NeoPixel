// Import required libraries
#include <ESP8266WiFi.h>
#include <aREST.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// NeoPixel Configuration
#define NPixeis 1
#define PIN 2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NPixeis, PIN, NEO_GRB + NEO_KHZ800);

// The port to listen for incoming TCP connections
#define LISTEN_PORT           80

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "ESPap";
const char* password = "123456789";

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API
int temperature;
int humidity;

int colorR = 0;
int colorG = 0;
int colorB = 0;

int delayTime = 0;

// Declare functions to be exposed to the API
int ledControl(String command);

int colorRControl(String command);
int colorGControl(String command);
int colorBControl(String command);

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  // Init variables and expose them to REST API
  temperature = 24;
  humidity = 40;
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);

  // Function to be exposed
  rest.function("led",ledControl);

  rest.function("colorR",colorRControl);
  rest.function("colorG",colorGControl);
  rest.function("colorB",colorBControl);

  // Give name and ID to device
  rest.set_id("1");
  rest.set_name("esp8266");

  // Setup WiFi network
  WiFi.softAP(ssid, password);
  Serial.println("");
  Serial.println("WiFi created");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void loop() {

  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  rest.handle(client);

}

// Custom function accessible by the API
int ledControl(String command) {

  // Get state from command
  int state = command.toInt();
  digitalWrite(13,state);
  return 1;
}

// Custom function accessible by the API
int colorRControl(String command) {
  // Get state from command
  colorR = command.toInt();
  colorWipe(strip.Color(colorR, colorG, colorB), delayTime);
  return 1;
}

// Custom function accessible by the API
int colorGControl(String command) {
  // Get state from command
  colorG = command.toInt();
  colorWipe(strip.Color(colorR, colorG, colorB), delayTime);
  return 1;
}

// Custom function accessible by the API
int colorBControl(String command) {
  // Get state from command
  colorB = command.toInt();
  colorWipe(strip.Color(colorR, colorG, colorB), delayTime);
  return 1;
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
