#include <Arduino.h>

#define ADC_PIN   9
#define ADL_PIN   10
#define START_PIN 11
//connect encoder output 5 to 0 to digital pins 2 to 7 


byte target = 5;

bool encoderUpdate() { //returns if final position is reached
  byte encoder = PIND >> 2; //read in encoder value
  if(encoder == 0b111111) //filter out false reading
    return false;
  printByte(encoder); //debug message
  if(encoder == target) { //check if target has been reached
    return true;
  }
  return false;
}

void printByte(byte b) {
  Serial.print(b);
  Serial.print(" ");
  Serial.print(b, 2);
  Serial.print(" t: ");
  Serial.print(target);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Fallblatt flapTest1\n");
  
  for(byte i = 2; i <= 7; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(ADL_PIN, OUTPUT);
  pinMode(START_PIN, INPUT);
  
  pinMode(13, OUTPUT);
  digitalWrite(ADL_PIN, LOW);

}

unsigned long lastTick = 0;

void loop() {
  unsigned long now = micros();

  if(now - lastTick >= 4000) {
    lastTick = now;
    tick();
  }

  if(Serial.available()) { //receive id of card to show over serial
    target = Serial.parseInt(); 
  }
}

void tick() {
  pinMode(START_PIN, INPUT);
  pinMode(ADC_PIN, INPUT);
  pinMode(ADC_PIN, OUTPUT);
  bool stopHere = encoderUpdate();
  pinMode(START_PIN, stopHere ? INPUT : OUTPUT);
  pinMode(ADC_PIN, INPUT);
}

