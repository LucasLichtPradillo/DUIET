#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>
#include <AccelStepper.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int ledStripPin = 9;

#define NUM_LEDS    5
CRGB leds[NUM_LEDS];

const int stepperIN1 = 2;
const int stepperIN2 = 3; 
const int stepperIN3 = 4; 
const int stepperIN4 = 5; 

AccelStepper stepper(AccelStepper::FULL4WIRE, stepperIN1, stepperIN3, stepperIN2, stepperIN4);

float currentTemperature = 20.0;
int lastBrightnessStrip = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  FastLED.addLeds<WS2812, ledStripPin, GRB>(leds, NUM_LEDS);
  FastLED.show();

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);

  stepper.setCurrentPosition(0);
  stepper.moveTo(0);
  stepper.runToPosition();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Desired Temp. ");
  lcd.setCursor(6,1);
  lcd.print(currentTemperature, 1);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil(',');
    if (command == "SET_LED_ONorOFF") {
      int id = Serial.parseInt();
//adding this step to prevent id 0 from being picked up when something random is accidentally picked up as a CV marker
      if (id >= 1 && id <= 20) {
        float tx = Serial.parseFloat();
        float ty = Serial.parseFloat();
        float tz = Serial.parseFloat();
        float rx = Serial.parseFloat();
        float ry = Serial.parseFloat();
        float rz = Serial.parseFloat();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Desired Temp.");

                if (tx < 0) {
          currentTemperature = map(rz, 3, -3, 18, 24);

          int targetPosition = map(rz, 3.2, -3.2, 750, -750);
          stepper.moveTo(targetPosition);
          stepper.runToPosition();
        }
        
        lcd.setCursor(6, 1);
        lcd.print(currentTemperature, 1);

        int brightnessStrip = 0;
        if (tx > 0) {
          brightnessStrip = map(rz, -3.2, 3.2, 255, 0);
          brightnessStrip = constrain(brightnessStrip, 0, 255);
          lastBrightnessStrip = brightnessStrip;
        }

        FastLED.setBrightness(lastBrightnessStrip);
        fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255));
        FastLED.show();
      }
    }
  }
  delay(10);
}