#define ADC_PIN   9
#define ADL_PIN   10
#define START_PIN 11
//connect encoder output 5 to 0 to digital pins 2 to 7 

#define ENABLE_MOTOR()  pinMode(START_PIN, OUTPUT)
#define DISABLE_MOTOR() pinMode(START_PIN, INPUT)
#define ENABLE_ADC()  { pinMode(ADC_PIN, OUTPUT); analogWrite(ADC_PIN, 128); }
#define DISABLE_ADC() { pinMode(ADC_PIN, INPUT_PULLUP); analogWrite(ADC_PIN, 0); }

byte lastEncoder = 0;
byte target = 5;

bool encoderUpdate() {
  byte encoder = PIND >> 2; //read in encoder value
  if(encoder == 0b111111)
    return;
  //stop motor here
  printByte(encoder);
  if(encoder == target /*&& lastEncoder != encoder*/){
    /*pinMode(START_PIN, INPUT);
    pinMode(ADC_PIN, INPUT);
    pinMode(ADC_PIN, OUTPUT);
    delay(1000);
    pinMode(START_PIN, OUTPUT);
    pinMode(ADC_PIN, INPUT);*/
    return true;
  }
  return false;
  //lastEncoder = encoder;
}

void printByte(byte b) {
  /*for(byte i = 0; i < 8; i++) {
    Serial.print(((b >> i) & 1) + i*4); Serial.print(" ");
  }
  Serial.println();*/
  Serial.print(b);
  Serial.print(" ");
  Serial.print(b, 2);
  Serial.print(" t: ");
  Serial.print(target);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  
  for(byte i = 2; i <= 7; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  //attachInterrupt(digitalPinToInterrupt(2), encoderUpdate, CHANGE);
  pinMode(ADL_PIN, OUTPUT);
  pinMode(START_PIN, INPUT);
  //ENABLE_ADC(); //start clock for ADC pin
  
  pinMode(13, OUTPUT);
  digitalWrite(ADL_PIN, LOW);

}

unsigned long lastPrint = 0, lastMot = 0, lastTick = 0;
bool lastMotorState = false;
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long now = micros();

  if(now - lastTick >= 4000) {
    lastTick = now;
    tick();
  }

  /*if(now - lastPrint >= 1000000) {
    lastPrint = now;
    target--;
    target &= 0b111111;
    if(target == 0b111111)
      target--;
    else if(target == 0)
      target = 0b111110;
  }*/

  if(Serial.available()) {
    target = Serial.parseInt(); 
  }
  
  /*if(now - lastPrint >= 100) {
    encoderUpdate();
    lastPrint = now;
  }
    
  if(now - lastMot >= 500000) {
    lastMot = now;
    lastMotorState ? DISABLE_MOTOR() : ENABLE_MOTOR();
    digitalWrite(13, lastMotorState);
    lastMotorState = !lastMotorState;
  }*/
}

bool toggle = false;
/*
void tick() {
  pinMode(ADL_PIN, INPUT);
  pinMode(ADC_PIN, OUTPUT);
  delayMicroseconds(1000);
  pinMode(ADC_PIN, INPUT);
  delayMicroseconds(1000);
  pinMode(ADL_PIN, OUTPUT);
  delayMicroseconds(1000);
  pinMode(ADC_PIN, OUTPUT);
  delayMicroseconds(1000);
  encoderUpdate();
  pinMode(START_PIN, INPUT);
  delayMicroseconds(100);
  pinMode(ADC_PIN, INPUT);
  delayMicroseconds(1000);
  pinMode(ADL_PIN, INPUT);
}*/

void tick() {
  pinMode(START_PIN, INPUT);
  pinMode(ADC_PIN, INPUT);
  pinMode(ADC_PIN, OUTPUT);
  bool stopHere = encoderUpdate();
  pinMode(START_PIN, stopHere ? INPUT : OUTPUT);
  pinMode(ADC_PIN, INPUT);

  /*
  pinMode(ADL_PIN, INPUT);
  pinMode(START_PIN, INPUT);
  delayMicroseconds(100);
  pinMode(ADC_PIN, OUTPUT);
  delayMicroseconds(100);
  pinMode(ADC_PIN, OUTPUT);
  encoderUpdate();
  pinMode(ADL_PIN, OUTPUT);
  pinMode(ADC_PIN, OUTPUT);
  pinMode(START_PIN, OUTPUT);*/
}
/*
void flip(byte index) {
  DDRD &= 0b00000011; //set pins 2-7 to input
  DDRD |= (index << 2); //set pins 2-7 corresponding to input
  Serial.println(DDRD, 2);
  pinMode(ADL_PIN, OUTPUT);
  pinMode(START_PIN, OUTPUT);
}*/

