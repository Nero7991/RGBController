
#include <Arduino.h>
#include <Switch.h>
#include <EEPROM.h>

TimerClass T1;
SwitchClass SwitchUp, SwitchDown, SwitchMode;

volatile const uint8_t LedPin = 13;
volatile const uint8_t RedChannel = 11;
volatile const uint8_t GreenChannel = 10;
volatile const uint8_t BlueChannel = 9;
volatile const uint8_t DownButton = 8;
volatile const uint8_t ModeButton = 7;
volatile const uint8_t UpButton = 6;
volatile const uint8_t BlinkToggle = A5;
volatile bool LedState = false, BlinkEnabled = false, SleepEnabled = true;

volatile uint8_t Mode = 0;
volatile uint8_t Values[3];

void readSavedValues(){
  EEPROM.get(0, Mode);
  for(int i = 0; i <= 2; i += 1){
    EEPROM.get(i+1, Values[i]);
  }
}

void saveValues(){
  EEPROM.put(0, Mode);
  for(int i = 0; i <= 2; i += 1){
    EEPROM.put(i+1, Values[i]);
  }
}

void updateChannels(){
  analogWrite(RedChannel, )
}

void toggleLed(uint8_t Id){
    if(LedState)
    digitalWrite(LedPin, LOW);
    else
    digitalWrite(LedPin, HIGH);
    LedState ^= 1;
}

void timerDone(uint8_t id){
    if(BlinkEnabled)
    toggleLed(0);
}

void toggleBlinking(uint8_t switchId){
    BlinkEnabled ^= 1;
}

void sleep(){
  T1.powerDown(SLEEP_MODE_IDLE);          //Stop CPU and FLASH clocks (Using other sleep modes causes delay in wake up)
}

void setup() {
  pinMode(LedPin, OUTPUT);
  pinMode(DownButton, INPUT_PULLUP);
  pinMode(ModeButton, INPUT_PULLUP);
  pinMode(UpButton, INPUT_PULLUP);
  pinMode(RedChannel, OUTPUT);
  pinMode(GreenChannel, OUTPUT);
  pinMode(BlueChannel, OUTPUT);
  
  Serial.begin(9600); Serial.println("Setting up things...");
  T1.begin();                             //It's necessary to call begin on any TimerClass object first, then SwitchClass object
  T1.initializeTimer();
  T1.setCallBackTime(500, TIMER_MODE_REPEAT, timerDone);
  SwitchUp.begin(); 
  SwitchUp.initializeSwitch(UpButton);
  SwitchDown.initializeSwitch(DownButton);
  SwitchMode.initializeSwitch(ModeButton);
  SwitchUp.shortPress(toggleLed);
  SwitchUp.longPress(toggleBlinking);
  SwitchDown.shortPress(toggleLed);
  SwitchDown.longPress(toggleBlinking);
  SwitchMode.shortPress(toggleLed);
  SwitchMode.longPress(toggleBlinking);
  analogWrite(RedChannel, 128);
  analogWrite(GreenChannel, 128);
  analogWrite(BlueChannel, 128);
  Serial.println("Done.");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(SleepEnabled){
    sleep();
  }
}