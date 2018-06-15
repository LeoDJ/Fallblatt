#include <Arduino.h>

/**
 * Code to control multiple flap displays
 * Connect all displays together with a ribbon cable and the ADC pin of each display to an Arduino pin
 * See the project README.md for more information
 * 
 * Receives commands over serial, two methods:
 * 1)   <dispId>,<card>[Enter]
 *      e.g.  0,31
 *      Set display 0 to card 31
 * 
 * 2)   <card0>,<card1>,<card2>,<card3>[Enter] 
 *      e.g. 30,31,32,33
 *      Set all displays to a card respectively (here 0 to 30, 1 to 31, 2 to 32 and 3 to 33)
 * 
 * 
 * ============================================
 * 
 * TODOs: 
 *  - stop displays if could not reach target in given time
 *  - send ack over serial when targets are reached
 * 
 */


#define START_PIN   8
#define ADL_PIN     9
const uint8_t ADC_PINS[] = {A0, A1, A2, A3};
//connect encoder output 5 to 0 to digital pins 2 to 7 

const uint8_t NUM_DISPLAYS = sizeof(ADC_PINS)/sizeof(ADC_PINS[0]);

uint8_t displayTarget[NUM_DISPLAYS];

void setup() {
    Serial.begin(115200);
    Serial.println("Fallblatt multiFlap\n");

    
    //enable encoder input pin pullups
    for(uint8_t i = 2; i <= 7; i++) {
        pinMode(i, INPUT_PULLUP);
    }

    for(uint8_t i = 0; i < NUM_DISPLAYS; i++) {
        pinMode(ADC_PINS[i], INPUT);
        digitalWrite(ADC_PINS[i], LOW);

        displayTarget[i] = 31;
    }

    //select display line permanently (controlled over column lines)
    digitalWrite(ADL_PIN, LOW);
    pinMode(ADL_PIN, OUTPUT);
}

unsigned long lastTick = 0;

void printEncoderDebug(uint8_t b, uint8_t target, uint8_t displayId) {
    //if(displayId != 0) return;
    Serial.print(displayId);
    Serial.print(" - ");
    Serial.print(b);
    Serial.print(" ");
    Serial.print(b, 2);
    Serial.print(" t: ");
    Serial.print(target);
    Serial.println();
}

bool checkTargetReached(uint8_t target, uint8_t displayId) { //returns if final position is reached
  byte encoder = PIND >> 2; //read in encoder value from port D
  if(encoder == 0b111111) //filter out false reading
    return false;
  //printEncoderDebug(encoder, target, displayId); //debug message
  if(encoder == target) { //check if target has been reached
    return true;
  }
  return false;
}

void tick() {
    for(uint8_t i = 0; i < NUM_DISPLAYS; i++) {
        pinMode(ADC_PINS[i], OUTPUT); //select display
        delayMicroseconds(100); //give the electronics time to settle

        //check if display has reached its target position
        bool targetReached = checkTargetReached(displayTarget[i], i);

        //set if motor should be stopped
        if(targetReached) 
            pinMode(START_PIN, INPUT);
        else
            pinMode(START_PIN, OUTPUT);
        
        pinMode(ADC_PINS[i], INPUT); //end communication with display (sets motor state)
    }
}

String cmd = "";
uint8_t cmdNums[NUM_DISPLAYS];
uint8_t cmdIdx = 0;

void loop() {
    unsigned long now = micros();
    if(now - lastTick >= 4000) {
        lastTick = now;
        tick();
    }

    //parse serial input
    if(Serial.available()) {
        char c = Serial.read();

        if(c == ',' || c == '\n') {
            //parse received number
            cmdNums[cmdIdx] = cmd.toInt();
            cmdIdx++;
            cmd = "";
        
            //execute command
            if(c == '\n') {
                //two parameter message
                if(cmdIdx == 2) {
                    displayTarget[cmdNums[0]] = cmdNums[1];
                }

                else if(cmdIdx == NUM_DISPLAYS) {
                    for(uint8_t i = 0; i < NUM_DISPLAYS; i++) {
                        displayTarget[i] = cmdNums[i];
                    }
                }
                cmdIdx = 0;
            }
        }
        else {
            cmd += c;
        }
    }

}