#include <Arduino.h>
#include <Servo.h>
#include "lib/dual-vnh5019-motor-shield/DualVNH5019MotorShield.h"
#include "lib/dual-vnh5019-motor-shield/DualVNH5019MotorShield.cpp"

//2, 4, 6, 7, 8, 9, 10, A0, A1 Reserved by MotorShield
#define AN_B A3
#define AN_X A4
#define AN_Y A5

DualVNH5019MotorShield md;

class Wing{
  public:
    int min;
    int max;
    int direction;
    int range;
    int output;
    Servo servo;

    void init( Servo * servo_ptr ){
      servo = *servo_ptr;
      range = max - min;
      servo.writeMicroseconds( min );
    }

    void update( float target ){
      output = ( range * target ) + min;
      servo.writeMicroseconds( output );
      Serial.println( output );
    }
};

Wing wing_r;
Wing wing_l;
Servo flap_l;
Servo flap_r;
//Servo range is 400-2600uS and moves 90 degrees in 1.5S or 1.466uS/1mS
int min_r = 1300;  //400 min
int max_r = 2000; //2600 max
int min_l = 1200;
int max_l = 2000;
float us_ms = 0.001466;
float timestep = 0;
float target = 0;
unsigned long elapsed = 0;
unsigned long lastRun = 0;
bool count_sign = 1;

unsigned long lastPrint = 0;

void setup(){
  Serial.begin( 115200 );
  pinMode( AN_B, INPUT_PULLUP );
  md.init();
  flap_l.attach( 3 );
  flap_r.attach( 5 );
  wing_r.min = 1050;
  wing_r.max = 1900;
  wing_l.min = 1950;
  wing_l.max = 1200;
  wing_r.init( &flap_l );
  wing_l.init( &flap_r );
}

void loop(){
  if( millis() - lastPrint > 100 ){
    lastPrint = millis();
    Serial.println( target );
  }

  if( !digitalRead( AN_B ) ){
    elapsed = millis()-lastRun;
    timestep = elapsed * us_ms; 
    target = count_sign ? target+=timestep : target-=timestep ;
    if( target > 1 ){ count_sign = 0; }
    if( target < 0 ){ count_sign = 1; }
    
    wing_r.update( target );
    wing_l.update( target );
    lastRun=millis();
  }
  else{ 
    lastRun=millis();
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
