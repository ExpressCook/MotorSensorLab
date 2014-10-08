#include "Encoder.h"
#include "Time.h" 
#include "string.h"
int flag_CONTROL_SPEED=0, flag_CONTROL_POS=0, flag_CONTROL_MANUAL=0;

Encoder motorEncoder(18, 19);
Encoder rotaryEncoder(20, 21);

int DCpwmPin = 6;
int DCmotorDriver1 = 5;
int DCmotorDriver2 = 4;
int motorEncoderA = 18;
int motorEncoderB = 19;

int time_interval, current_time, last_call_time;
long DCdesiredPosition, DClastPosition = -999, DCcurrentPosition;
long DCrotaryEncoderPosition;

double DCdesiredSpeed, DCinputSpeed, DCcurrentSpeed;
int adjDCinputSpeed = 0;
int adjDCinputSpeed2 = 0;
double Kp = 0.1;
double Ki = 0.03;
double Kd = 0.08;
float pidTerm = 0;                                                            // PID correction
int error_s = 0, intError_s = 0, error_p = 0, intError_p = 0;                                  
static int last_error_s = 0, last_error_p = 0; 

int debounce()
{
  current_time = millis(); // retrieve current time
  time_interval = current_time - last_call_time;
  if (time_interval>200) // if 5ms passed after the last interrupt
  {                                    // take current interrupt as valid
    last_call_time = current_time;
    return 1;
  }
  else 
  {
    last_call_time = current_time;
    return 0;
  }
}

int PID_speed(int DCinputSpeed, int targetValue, int currentValue)   
{             
  error_s = abs(targetValue) - abs(currentValue); 
  pidTerm = (Kp * error_s) + (Ki * last_error_s) + (Kd * (error_s - last_error_s));                            
  last_error_s = error_s;
  return constrain(DCinputSpeed + int(pidTerm), 0, 255);
}

int PID_speed4pos(int DCinputSpeed, int targetValue, int currentValue)   
{             
  error_s = abs(targetValue) - abs(currentValue); 
  intError_s = intError_s + error_s;
  pidTerm = (0.12 * error_s) + (0.5 * intError_s) + (0.03 * (error_s - last_error_s));                            
  last_error_s = error_s;
  return constrain(DCinputSpeed + int(pidTerm), 30, 255);
}

int PID_pos(int DCinputSpeed, int targetValue, int currentValue)   
{           
  error_p = (targetValue - currentValue);    
  intError_p = intError_p + error_p;
  pidTerm = 3*((0.17 * error_p) +(0.0 * intError_p) + (0.01 * (error_p - last_error_p)));                            
  last_error_p = error_p;
  
  return constrain(int(pidTerm), -255, 255); 
}

void setDutyCycle(int dutyCycle)
{
  analogWrite(DCpwmPin, dutyCycle);
}

void setDirection(int dir)
{
  if (dir>0)
  {
    digitalWrite(DCmotorDriver1, HIGH);
    digitalWrite(DCmotorDriver2, LOW);
  }
  else
  {
    digitalWrite(DCmotorDriver1, LOW);
    digitalWrite(DCmotorDriver2, HIGH);
  }
}

void setNewSpeed_closeloop(char* string)
{

}

void printCurrentStatus()
{
  long pos = getCurrentOrientationMotor();
  double s = getDCcurrentSpeed();
  //Serial.print("Current Speed:");
  //Serial.print(s);
  //Serial.print("  ");
  //Serial.print("Current Orientation:");
  //Serial.print((pos/2)%360);
  //Serial.print('\n');
}

long getCurrentOrientationMotor()
{
  long motorPosition;
  motorPosition = motorEncoder.read(); // get current orientation
  return motorPosition;
}

long getCurrentOrientationRE()
{
  long rotaryPosition;
  rotaryPosition = rotaryEncoder.read(); // get current orientation
  return rotaryPosition;
}

double getDCcurrentSpeed()
{
  unsigned long time1, time2;
  long pos1, pos2;
  int flag=1;
  time1 = micros();
  pos1 = getCurrentOrientationMotor();
  while (flag)
  {
    pos2 = getCurrentOrientationMotor();
    delay(0.2);
    if(pos2>pos1+5)
    {
      time2 = micros();
      flag=0;
      return 5000000/(double)(2*(time2-time1));
    }
    else if(pos2<pos1-5)
    {
      time2 = micros();
      flag=0;
      return -5000000/(double)(2*(time2-time1));
    }
    if(micros()-time1>100000) return 0;
  } 
  
}

void rotateXDegree(int degree)
{
  long motorPositionStart, motorPositionEnd = 0;
  motorPositionStart = motorEncoder.read();
  int flag = 1;
  time_t timeStart, timeEnd;
  timeStart = second();
  while(flag==1)
  {
    motorPositionEnd = motorEncoder.read();
    if ((motorPositionEnd - motorPositionStart)>(double)(degree-5)*2)
    {
      analogWrite(DCpwmPin, 0);
      flag = 0;
    }
    timeEnd = second();
    if (timeEnd-timeStart > 5) return;

    //Serial.print(timeEnd);
    //Serial.print("   ");
    printCurrentStatus();
  }
}

void initDC()
{
  // setup
  pinMode(DCpwmPin, OUTPUT);
  pinMode(DCmotorDriver1, OUTPUT);
  pinMode(DCmotorDriver2, OUTPUT);

  int motorInitDutyCycle = 0;
  analogWrite(DCpwmPin, motorInitDutyCycle);

  pinMode(motorEncoderA, INPUT);
  pinMode(motorEncoderB, INPUT);

  // initialization
  digitalWrite(DCmotorDriver1, HIGH);
  digitalWrite(DCmotorDriver2, LOW);

  //turn the PID on
  DCdesiredSpeed = 0;
  DCinputSpeed = 120;
}

void loopDC()
{
  // DC motor speed control mode
  if (flag_CONTROL_SPEED==1)
  {
    DCSpeed();
  }

  // DC motor position control mode
  if (flag_CONTROL_POS==1)
  {
    DCPos();
  }
  
  // DC motor rotary encoder control mode
  if (flag_CONTROL_MANUAL==1)
  {
    DCManual();
  }

  if(Serial.available())
  {
    char commandClass[20], commandContent[20];
    for(int i=0;i<20;i++) 
    {
      commandClass[i] = NULL;
      commandContent[i] = NULL;
    }
    int length = 20;
    int temp;
    Serial.readBytesUntil('.', commandClass, length);
    Serial.readBytesUntil('\n', commandContent, length);
    if(!strncmp(commandClass, "DCSpeed", 7))  
    {
      flag_CONTROL_SPEED=1;
      flag_CONTROL_POS=0;
      flag_CONTROL_MANUAL=0;
      
      int temp;
      sscanf(commandContent, "%d", &temp);
      DCdesiredSpeed = (double)temp;
      
      init_DCSpeed(DCdesiredSpeed);
    }
    if(!strncmp(commandClass, "DCPos", 5)) 
    {
      flag_CONTROL_POS=1;
      flag_CONTROL_SPEED=0;
      flag_CONTROL_MANUAL=0;
      int DCPosition;
      sscanf(commandContent, "%d", &DCPosition);
      
      init_DCPos(DCPosition);
    }
    if(!strncmp(commandClass, "DCManual", 8)) 
    {
      flag_CONTROL_POS=0;
      flag_CONTROL_SPEED=0;
      flag_CONTROL_MANUAL=1;
      
      init_DCManual();
    }

    printCurrentStatus();
  }
  printCurrentStatus();
}

void DCSpeed()
{
  delay(10);
  //Serial.print("current speed:");
  DCcurrentSpeed = getDCcurrentSpeed();
  //Serial.print(DCcurrentSpeed);
  //Serial.print("   ");

  DCinputSpeed = PID_speed(DCinputSpeed, DCdesiredSpeed, DCcurrentSpeed);
 
  setDutyCycle(DCinputSpeed);
  if(DCdesiredSpeed>0) setDirection(1);
  else setDirection(0);
}

void DCPos()
{
  delay(10);
  DCcurrentPosition = getCurrentOrientationMotor();
  DCinputSpeed = PID_pos(DCinputSpeed, DCdesiredPosition, DCcurrentPosition);
  DCcurrentSpeed = getDCcurrentSpeed();
  adjDCinputSpeed = PID_speed4pos(abs(adjDCinputSpeed), abs(DCinputSpeed), DCcurrentSpeed);
  setDutyCycle(abs(adjDCinputSpeed));
  if(DCinputSpeed>0) setDirection(1);
  else setDirection(0);
}

void DCManual()
{
  delay(10);
  DCrotaryEncoderPosition = getCurrentOrientationRE();
//  Serial.print(DCrotaryEncoderPosition);
//  Serial.print("  \n");
  double DCtemp = DCrotaryEncoderPosition*2*3.1415/100;
//  Serial.print(floor(abs(sin(DCtemp))*255));
//  Serial.print("  \n");
  setDutyCycle(floor(abs(sin(DCtemp))*255));
  if((DCrotaryEncoderPosition%100)>50) setDirection(1);
  else setDirection(0);
}

void init_DCSpeed(double DCspeed)
{
  DCdesiredSpeed = (double)DCspeed;
  intError_s = 0;
  error_s=0;
  last_error_s=0;
}

void init_DCPos(int DCPosition)
{
  DCcurrentPosition = getCurrentOrientationMotor();
  DCdesiredPosition = (DCcurrentPosition/720)*720+DCPosition*2;
  DCinputSpeed = 0;
  intError_p = 0;
  error_p=0;
  last_error_p=0;
  intError_s = 0;
  error_s=0;
  last_error_s=0;
}

void init_DCManual()
{
}

int getDCEncoder()
{
  return int(getCurrentOrientationRE() % 100);
}

int getDCPos()
{
  return int((getCurrentOrientationMotor() % 720)/2);
}

int getDCSpeed()
{
  return int(getDCcurrentSpeed());
}



