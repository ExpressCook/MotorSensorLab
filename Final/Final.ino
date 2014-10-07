//integration

void setup()
{
  Serial.begin(9600);
  
  //initial the Stepper motor
  initSM();

  //initial the DC motor
  
  //inital the Servo  
}

void loop()
{
  int val=0;
  int smSpeed =0; 
  val = getIRValue();  
  
  //change the speed of steeper motor
  //calulate the speed
  smSpeed = 200+5*val;
  Serial.println(smSpeed);
  setSMSpeed(smSpeed);
}
