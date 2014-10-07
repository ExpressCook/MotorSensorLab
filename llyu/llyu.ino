#include "Encoder.h"
#include "Time.h" 
#include "string.h"
//#include "PID_v1.h"
int flag_CONTROL_SPEED=0, flag_CONTROL_POS=0;

Encoder motorEncoder(2, 3);

int pwmPin = 6;
int motorDriver1 = 5;
int motorDriver2 = 4;
int motorEncoderA = 3;
int motorEncoderB = 2;

int time_interval, current_time, last_call_time;
long desiredPosition, lastPosition = -999, currentPosition;

double desiredSpeed, inputSpeed, currentSpeed;
double Kp = 0.1;
double Ki = 0.03;
double Kd = 0.8;
float pidTerm = 0;                                                            // PID correction
int error_s = 0, intError_s = 0, error_p = 0, intError_p = 0;                                  
static int last_error_s = 0, last_error_p = 0; 

//PID speedPID(&inputSpeed, &currentSpeed, &desiredSpeed, 500, 100, 500, DIRECT); // input, output, desired_output

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

int PID_speed(int inputSpeed, int targetValue, int currentValue)   
{             
  error_s = abs(targetValue) - abs(currentValue); 
  pidTerm = (Kp * error_s) + (Ki * last_error_s) + (Kd * (error_s - last_error_s));                            
  last_error_s = error_s;
  return constrain(inputSpeed + int(pidTerm), 0, 255);
}

int PID_pos(int inputSpeed, int targetValue, int currentValue)   
{           
  error_p = (targetValue - currentValue);  
  Serial.print("error=");
  Serial.print(error_p);
  Serial.print("  ");
  intError_p = intError_p + error_p;
  pidTerm = 0.1*((30 * error_p) +(0.01 * intError_p) + (180 * (error_p - last_error_p)));                            
  last_error_p = error_p;
  
  return constrain(inputSpeed + int(pidTerm), -255, 255); 
}

void setDutyCycle(int dutyCycle)
{
  analogWrite(pwmPin, dutyCycle);
}

void setDirection(int dir)
{
  if (dir>0)
  {
    digitalWrite(motorDriver1, HIGH);
    digitalWrite(motorDriver2, LOW);
  }
  else
  {
    digitalWrite(motorDriver1, LOW);
    digitalWrite(motorDriver2, HIGH);
  }
}

//void setNewSpeedAndReport_openloop()
//{
//  char string[21];
//  int length = 20;
//  int newSpeed;
//  Serial.readBytesUntil('\n', string, length);
//  sscanf(string, "%lf", &newSpeed);
//
//  setDutyCycle(newSpeed);
//
//  double dutyCycle = (double)newSpeed/256;
//
//  Serial.print("Current Duty Cycle:");
//  Serial.print(dutyCycle);
//  Serial.print('\n');
//}

void setNewSpeed_closeloop(char* string)
{
  int temp;
  sscanf(string, "%d", &temp);
  desiredSpeed = (double)temp;
  Serial.print("DESIREDSPEED CHANGED!!!!!!");
  Serial.print(desiredSpeed);
  Serial.print('\n');
  Serial.print(string);
  Serial.print('\n');
  //state = CONTROL_SPEED;
}
void printCurrentStatus()
{
  long pos = getCurrentOrientation();
  double s = getCurrentSpeed();
  Serial.print("Current Speed:");
  Serial.print(s);
  Serial.print("  ");
  Serial.print("Current Orientation:");
  Serial.print(pos);
  Serial.print('\n');
}

long getCurrentOrientation()
{
  long motorPosition;
  motorPosition = motorEncoder.read(); // get current orientation
  return motorPosition;
}

double getCurrentSpeed()
{
  unsigned long time1, time2;
  long pos1, pos2;
  int flag=1;
  time1 = micros();
  pos1 = getCurrentOrientation();
  while (flag)
  {
    pos2 = getCurrentOrientation();
    delay(0.2);
    //Serial.print("WHy");
    if(pos2>pos1+5)
    {
      time2 = micros();
      //      Serial.print(time1);
      //      Serial.print("   ");
      //      Serial.print(time2);
      flag=0;
    }
    if(micros()-time1>100000) return 0;
  } 
  return 5000000/(double)(2*(time2-time1));
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
      analogWrite(pwmPin, 0);
      flag = 0;
    }
    timeEnd = second();
    if (timeEnd-timeStart > 5) return;

    Serial.print(timeEnd);
    Serial.print("   ");
    printCurrentStatus();
  }
}

void setup()
{
  // setup
  pinMode(pwmPin, OUTPUT);
  pinMode(motorDriver1, OUTPUT);
  pinMode(motorDriver2, OUTPUT);

  int motorInitDutyCycle = 100;
  analogWrite(pwmPin, motorInitDutyCycle);

  pinMode(motorEncoderA, INPUT);
  pinMode(motorEncoderB, INPUT);

  Serial.begin(9600);

  // initialization
  digitalWrite(motorDriver1, HIGH);
  digitalWrite(motorDriver2, LOW);

  //turn the PID on
  desiredSpeed = 200;
  inputSpeed = 120;
}

void loop()
{
  if (flag_CONTROL_SPEED==1)
  {
    delay(10);
    Serial.print("current speed:");
    currentSpeed = getCurrentSpeed();
    Serial.print(currentSpeed);

    inputSpeed = PID_speed(inputSpeed, desiredSpeed, currentSpeed);
    Serial.print('\n');

    setDutyCycle(inputSpeed);
    setDirection(1);
  }

  if (flag_CONTROL_POS==1)
  {
    delay(10);
    currentPosition = getCurrentOrientation();
    Serial.print("current degree:");
    Serial.print(currentPosition);
    //Serial.print((currentPosition/2)%360);
    Serial.print("   ");
    
    inputSpeed = PID_pos(inputSpeed, desiredPosition, currentPosition);
    
    int adjInputSpeed;
    if(abs(inputSpeed)<50) adjInputSpeed = 50;
    else adjInputSpeed = abs(inputSpeed);
    
    int adjInputSpeed2 = 0;
    adjInputSpeed2 = PID_speed(adjInputSpeed2, adjInputSpeed, currentSpeed);
 
    Serial.print("Input Speed:");
    Serial.print(adjInputSpeed2);
    
    setDutyCycle(abs(adjInputSpeed2));
    if(adjInputSpeed2>0) setDirection(1);
    else setDirection(0);
    Serial.print('\n');
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
    Serial.print(commandClass);
    Serial.print('\n');
    Serial.print(commandContent);
    if(!strncmp(commandClass, "DCSpeed", 7)) 
    {
      Serial.print("1111111\n");
      flag_CONTROL_SPEED=1;
      flag_CONTROL_POS=0;
      setNewSpeed_closeloop(commandContent);
      intError_s = 0;
      error_s=0;
      last_error_s=0;
    }
    if(!strncmp(commandClass, "DCPos", 5)) 
    {
      Serial.print("2222222222\n");
      flag_CONTROL_POS=1;
      flag_CONTROL_SPEED=0;
      sscanf(commandContent, "%d", &desiredPosition);
      currentPosition = getCurrentOrientation();
      desiredPosition = (currentPosition/720)*720+desiredPosition;
      Serial.print("Desired Position:");
      Serial.print(desiredPosition);   
      Serial.print("   ");
      inputSpeed = 0;
      intError_p = 0;
      error_p=0;
      last_error_p=0;
    }
    //setNewSpeedAndReport_openloop();

    printCurrentStatus();
    //rotateXDegree(720);
  }
}






