//combine the IR Sensor and stepper motor
//the stepper motor is controlled using arduino library
const int analogIR = 0;
const int smC1 = 28;
const int smC2 = 30;
const int smC3 = 32;
const int smEn = 26;
const int smStep = 22;
const int smDir = 24;

enum SMDriveType
{
  full_step, half_step, quarter_step, eigth_step, sixteenth_step
};

void setSMDriveType(int drive)
{
  switch(drive)
  {
    case full_step:
      digitalWrite(smC1,LOW);
      digitalWrite(smC2,LOW);
      digitalWrite(smC3,LOW);
      break; 
    case half_step:
      digitalWrite(smC1,HIGH);
      digitalWrite(smC2,LOW);
      digitalWrite(smC3,LOW);
      break; 
    case quarter_step:
      digitalWrite(smC1,LOW);
      digitalWrite(smC2,HIGH);
      digitalWrite(smC3,LOW);
      break; 
    case eigth_step:
      digitalWrite(smC1,HIGH);
      digitalWrite(smC2,HIGH);
      digitalWrite(smC3,LOW);
      break; 
    case sixteenth_step:
      digitalWrite(smC1,HIGH);
      digitalWrite(smC2,HIGH);
      digitalWrite(smC3,HIGH);
      break; 
  }
}

void initSM()
{
  //Set the pin mode attached to stepper motor
  pinMode(smC1, OUTPUT);
  pinMode(smC2, OUTPUT);
  pinMode(smC3, OUTPUT);
  pinMode(smEn, OUTPUT);
  pinMode(smStep, OUTPUT);
  pinMode(smDir, OUTPUT);
  
  //set the default drive type
  //default drive type is full step
  SMDriveType type = full_step;
  setSMDriveType(type);
  
  //set the default direction
  digitalWrite(smDir, HIGH);
  
  //set the power state
  digitalWrite(smEn, LOW);
}

//set the position of the motor
int SMPos =0;
void setSMPos(int degree)
{
  if(isFirstTimeSMPos)
  {
    //control the direction
    if(degree<=0)
        digitalWrite(smDir, LOW);
    else
        digitalWrite(smDir, HIGH);

    //the optimum drving frequency
    int period = 1500;
  
    //calculate the steps
    int steps= abs(degree/1.8);  //based on full-step
  
    for(int i=0;i<steps;i++)
    {
      digitalWrite(smStep,LOW);
      delayMicroseconds(period);
      digitalWrite(smStep,HIGH);
      delayMicroseconds(period);
    }
  
    digitalWrite(smStep,LOW);
    digitalWrite(smDir, HIGH);
    SMPos = (SMPos + degree)%360;
  }
  isFirstTimeSMPos = false;
}

//Run the motor in the speed for a very short of time
int SMSpeed = 0;
void setSMSpeed(int smSpeed)
{
  int period = 40000/smSpeed;
  SMSpeed = smSpeed;
  for(int i=0;i<10;i++)
  {
    digitalWrite(smStep,LOW);
    delayMicroseconds(period);
    digitalWrite(smStep,HIGH);
    delayMicroseconds(period);
  }
  
}

int getSMSpeed()
{
  return SMSpeed;
}

int getSMPos()
{
  return SMPos;
}

//get the value from the IR sensor
int getIRValue()
{
  return analogRead(analogIR);
}

//using IR to control speed
void controlSMByIR()
{
  int val = getIRValue();    
  //change the speed of steeper motor
  //calulate the speed
  int smSpeed = 5+val/15;
  
  setSMSpeed(smSpeed);
}


