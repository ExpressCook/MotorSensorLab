#include <Servo.h>
#include "Encoder.h"
#include "Time.h" 
#include "string.h"
#include <QueueList.h>
#include <iostream>

//global
boolean isFirstTimeSMPos = true;

//integration
void setup()
{
  Serial.begin(9600);
  
  //initial the Stepper motor
  initSM();

  //initial the DC motor
  initDC();
  
  //inital the Servo 
  initServo();
  attachInterrupt(0,debounceSwitch,CHANGE);
}

int state = 0;
//a gui servo pos
//b servo
//c gui stepper pos
//d gui stepper vel
//e stepper
//f gui dc pos
//g gui dc vel
//h dc
int controlValue = 0;


void loop()
{
  if(Serial.available())
  {
    //parse the serial and change the state
    state = Serial.read(); //state is decided by the first byte
    
    switch(state)
    {
      case 'a':
      case 'c': isFirstTimeSMPos = true;
      case 'd':
      case 'f': 
      case 'g': controlValue = Serial.parseInt();
                break;
    }
     
     //special init for DC motor
     //need to be improved
    if(state == 'f')
       init_DCPos(controlValue);
    if(state == 'g')
       init_DCSpeed(controlValue);  
  }
  
  switch(state)
  {
    case 'a':Servo_Control(1,controlValue);
             break;
    case 'b':Servo_Control(1,-1);
             break;
    case 'c':setSMPos(controlValue);
             //send back the value
             break;
    case 'd':setSMSpeed(controlValue);
             break;
    case 'e':controlSMByIR();
             break;
    case 'f':DCPos();
             break;
    case 'g':DCSpeed();
             break;
    case 'h':DCManual();
             break; 
  }
   
}


