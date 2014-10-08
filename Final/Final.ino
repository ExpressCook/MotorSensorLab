//integration
void setup()
{
  Serial.begin(9600);
  
  //initial the Stepper motor
  initSM();

  //initial the DC motor
  
  //inital the Servo  
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
boolean isFirstTimeSMPos = true;

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
  }
  
  switch(state)
  {
    case 'a':break;
    case 'b':break;
    case 'c':setSMPos(controlValue);
           //send back the value
           break;
    case 'd':setSMSpeed(controlValue);
           break;
    case 'e':controlSMByIR();
           break;
    case 'f':break;
    case 'g':break;
    case 'h':break;
  }
   
}


