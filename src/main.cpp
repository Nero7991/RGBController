
#include <Arduino.h>
#include <Switch.h>
#include <EEPROM.h>

#define StepValue 5

TimerClass T1;
SwitchClass SwitchUp, SwitchDown, SwitchMode;

const uint8_t LedPin = 13;
const uint8_t RedChannel = 10;
const uint8_t GreenChannel = 5;
const uint8_t BlueChannel = 9;
const uint8_t DownButton = 8;
const uint8_t ModeButton = 7;
const uint8_t UpButton = 6;
const uint8_t BlinkToggle = A5;
volatile bool LedState = false, BlinkEnabled = false, SleepEnabled = true, SaveFlag = false, IncrementFlag = false, DecrementFlag = false, ModeChangeFlag = false;

uint8_t Mode = 0, Temp = 0;
uint8_t Values[3];


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
  analogWrite(RedChannel, Values[0]);
  analogWrite(GreenChannel, Values[1]);
  analogWrite(BlueChannel, Values[2]);
}

void changeMode(){
  if(Mode >= 3)
    Mode = 0;
  else
    Mode += 1;
  SaveFlag = true;
  saveValues();
  ModeChangeFlag = false;
}

void incrementPressed(uint8_t Id){
  IncrementFlag = true;
}

void decrementPressed(uint8_t Id){
  DecrementFlag = true;
}

void modeChangePressed(uint8_t Id){
  ModeChangeFlag = true;
}

void incrementChannel(){
  if(Mode < 3){
    if(Values[Mode] < (255 - (StepValue + 2))){
      Values[Mode] += StepValue;
      SaveFlag = true;
      saveValues();
    }
    else
    {
      Values[Mode] = 255;
      SaveFlag = true;
      saveValues();
    }
  }
  if(Mode == 3){
    for(int i = 0; i <= 2; i += 1){
       Mode = i;
       incrementChannel();
    }
    Mode = 3;
  }
  IncrementFlag = false;
}

void decrementChannel(){
  if(Mode < 3){
    if(Values[Mode] > (StepValue + 2)){
      Values[Mode] -= StepValue;
      SaveFlag = true;
      saveValues();
    }
    else
    {
      Values[Mode] = 0;
      SaveFlag = true;
      saveValues();
    }
  }
  if(Mode == 3){
    for(int i = 0; i <= 2; i += 1){
      Mode = i; 
      decrementChannel();
    }
    Mode = 3;
  }
  DecrementFlag = false;
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
  // pinMode(RedChannel, OUTPUT);
  // pinMode(GreenChannel, OUTPUT);
  // pinMode(BlueChannel, OUTPUT);
  
  Serial.begin(9600); Serial.println("Setting up things...");
  T1.begin();                             //It's necessary to call begin on any TimerClass object first, then SwitchClass object
  T1.initializeTimer();
  T1.setCallBackTime(500, TIMER_MODE_REPEAT, timerDone);
  SwitchUp.begin(); 
  SwitchUp.initializeSwitch(UpButton);
  SwitchDown.initializeSwitch(DownButton);
  SwitchMode.initializeSwitch(ModeButton);
  // SwitchUp.longPress(toggleBlinking);
  // SwitchDown.longPress(toggleBlinking);
  // SwitchMode.longPress(toggleBlinking);
  SwitchUp.shortPress(incrementPressed);
  SwitchDown.shortPress(decrementPressed);
  SwitchMode.shortPress(modeChangePressed);
  SleepEnabled = true;
  readSavedValues();
  updateChannels();
  Serial.println("Done.");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(SleepEnabled){
    sleep();
  }
  if(IncrementFlag)
    incrementChannel();
  if(DecrementFlag)
    decrementChannel();
  if(ModeChangeFlag)
    changeMode();
  if(SaveFlag){
    SaveFlag = false;
    saveValues();
    updateChannels();
    Serial.print("Mode = ");
    Serial.print(Mode);
    Serial.print(" Red = ");
    Serial.print(Values[0]);
    Serial.print(" Green = ");
    Serial.print(Values[1]);
    Serial.print(" Blue = ");
    Serial.println(Values[2]);
  }
}