#include <Wire.h>
#include <MPU9250.h>
#include <Servo.h>
#include <NewPing.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define TRIGGER_PIN  12  
#define ECHO_PIN     8   
#define MAX_DISTANCE 400 
#define LED_PIN    6   
#define CONTROL_PIN  5   
#define LED_COUNT    39  
#define BRIGHTNESS   200  

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

MPU9250 mpu;
Servo myServo;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
SoftwareSerial BTSerial(2, 3); 

float previousAngle = 0;
unsigned long previousTime = 0;
int pendulumCount = 0;
bool servoMoved = false;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);

  mpu.setup(0x68);
  myServo.attach(9);

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();

  pinMode(CONTROL_PIN, OUTPUT);
  digitalWrite(CONTROL_PIN, LOW); 

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (pendulumCount < 2) {
    mpu.update();
    float currentAngle = mpu.getAngleX();
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - previousTime) / 1000.0; 
    float angularVelocity = (currentAngle - previousAngle) / deltaTime;

    if (currentAngle > 45.0 && abs(angularVelocity) > 30.0) { 
      pendulumCount++;
      delay(500);
    }

    previousAngle = currentAngle;
    previousTime = currentTime;
  }

  if (pendulumCount >= 2 && !servoMoved) {
    myServo.write(180);
    servoMoved = true;  
    delay(1000); 

    
    bool stable = true;
    long startTime = millis();
    float initialDistance = sonar.ping_cm();

    while (millis() - startTime < 60000) { 
      unsigned int uS = sonar.ping();
      float newDistance = uS / US_ROUNDTRIP_CM;

      if (abs(newDistance - initialDistance) > 2) { 
        stable = false;
        initialDistance = newDistance;
        startTime = millis(); 
      }

      delay(1000); 
    }

    if (stable && initialDistance >= 15) { 
      turnOnLights();
      BTSerial.println("TURN_ON");
    }
  }
}

void turnOnLights() {
  digitalWrite(CONTROL_PIN, HIGH); 
  int color_r = 255;
  int color_g = 255;
  int color_b = 255;

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, color_r, color_g, color_b, 0);
  }
  strip.show();
}
