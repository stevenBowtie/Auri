#include <Arduino.h>
#include "lib/dual-vnh5019-motor-shield/DualVNH5019MotorShield.h"
#include "lib/dual-vnh5019-motor-shield/DualVNH5019MotorShield.cpp"

#define AN_B A3
#define AN_X A4
#define AN_Y A5
//2, 4, 6, 7, 8, 9, 10, A0, A1 Reserved by MotorShield
DualVNH5019MotorShield md;

unsigned long lastPrint = 0;

void setup(){
  Serial.begin( 115200 );
  pinMode( AN_B, INPUT_PULLUP );
  md.init();
}

void loop(){
  if( millis() - lastPrint > 100 ){
    lastPrint = millis();
    Serial.print( analogRead( AN_X ) );
    Serial.print( "," );
    Serial.print( analogRead( AN_Y ) );
    Serial.print( "," );
    Serial.println( digitalRead( AN_B ) );
    
  }
  if( analogRead( AN_X ) < 250 ){
    //Out
    md.setM1Speed( -400 );
  }
  else if( analogRead( AN_X ) > 750 ){
    //In
    md.setM1Speed( 400 );
  }
  else{
    //Stop
    md.setM1Speed( 0 );
  }
  if( analogRead( AN_Y ) < 250 ){
    //Down
    md.setM2Speed( -400 );
  }
  else if( analogRead( AN_Y ) > 750 ){
    //Up
    md.setM2Speed( 400 );
  }
  else{
    md.setM2Speed( 0 );
  }
}

void stopIfFault(){
  if (md.getM1Fault()){
    Serial.println("M1 fault");
    while(1);
  }
  if (md.getM2Fault()){
    Serial.println("M2 fault");
    while(1);
  }
}
