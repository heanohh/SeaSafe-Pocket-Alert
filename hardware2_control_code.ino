#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 8      
#define LED_COUNT 96   
#define BRIGHTNESS 200  

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
SoftwareSerial BTSerial(2, 3); 

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
}

void loop() {
  if (BTSerial.available()) {
    String command = BTSerial.readStringUntil('\n');
    if (command == "TURN_ON") {
      turnOnLight();
    }
  }
}

void turnOnLight() {
  int color_r = 255;
  int color_g = 255;
  int color_b = 255;

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, color_r, color_g, color_b, 0);
  }
  strip.show();
}
