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
int colorModeId = 0;

int delayTime = 0;


// Declare functions to be exposed to the API
int ledControl(String command);

int colorRControl(String command);
int colorGControl(String command);
int colorBControl(String command);
int colorRGBControl(String command);

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
  rest.function("colorRGB",colorRGBControl);

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

  switch(colorModeId){
    case 1:
      rainbow(delayTime);
      break;
    case 2:
      rainbowCycle(delayTime);
      break;
    case 3:
      theaterChaseRainbow(delayTime);
      break;
  }
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

// Custom function accessible by the API
int colorRGBControl(String command) {
  // Get state from command
  colorR = command.toInt();
  colorG = command.toInt();
  colorB = command.toInt();
  colorWipe(strip.Color(colorR, colorG, colorB), delayTime);
  return 1;
}

// Custom function accessible by the API
int colorMode(String command) {
  // Get state from command
  colorModeId = command.toInt();  
  return 1;
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
