#include <Servo.h>
#include "Encoder.h"
#include "Time.h" 
#include "string.h"
#include <iostream>

//global Values
boolean isFirstTimeSMPos = true;
//Servo value
int PotentiometerVal;
int ServoPosition;
volatile unsigned int En=1;
volatile unsigned long last_millis=0;
long debouncing_time=50;



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
    case 'a':Servo_Control(En,controlValue);
             sendFeedBack(ServoPosition);
             break;
    case 'b':Servo_Control(En,-1);
             sendFeedBack(PotentiometerVal, ServoPosition);
             break;
    case 'c':setSMPos(controlValue);
             sendFeedBack(getSMPos());
             break;
    case 'd':setSMSpeed(controlValue);
             sendFeedBack(getSMSpeed());
             break;
    case 'e':controlSMByIR();
             sendFeedBack(getIRValue(),getSMSpeed());
             break;
    case 'f':DCPos();
             sendFeedBack(getDCPos());
             break;
    case 'g':DCSpeed();
             sendFeedBack(getDCSpeed());
             break;
    case 'h':DCManual();
             sendFeedBack(getDCEncoder(),getDCSpeed());
             break; 
  }
   
}

void sendFeedBack(int senVal, int posVal)
{
  //Serial.print('s');
  Serial.println(senVal);
  Serial.println(posVal);
}

void sendFeedBack(int posVal)
{
  //Serial.print('p');
  Serial.println(posVal);
}

void debounceSwitch(){
  //En pin determines whether or not the Servo will be enabled
  if((long)(millis()-last_millis)>=debouncing_time*10){
    En=1-En;
    //Serial.println("butttttton");
    last_millis=millis();
  }
  
}

