int pot = A1;  // analog pin used to connect the potentiometer
int PotentiometerVal;
int ServoPosition;
int button=3;
long debouncing_time=50;
int AvgFilter[100]={0};
int sum=0;
int i=0;
volatile unsigned long last_millis=0;
volatile unsigned int En;
Servo servo;  //create servo object

void initServo()
{
    servo.attach(9, 650, 2350);
}

void Servo_Control(int En,int Position)
{
  if(En==1)
  {
    if(Position==-1)
  {
    sum=0;
    for (i=0;i<100;i++)
    {
      AvgFilter[i]=analogRead(pot);
      sum=sum+AvgFilter[i];
    }
    int avg=round(sum/100);
    //This condition is when the GUI requests that Servo Motor position be controlled
    //by reading Potentiometer values
    //PotentiometerVal = analogRead(pot);       // reads the value of the potentiometer (value between 0 and 1023) 
    ServoPosition = map(avg, 0, 1023, 0, 179);
    servo.write(ServoPosition);                  // sets the servo position according to the scaled value  
    delay(15);
  }
  else
  {
    //This condition is when the GUI sends the Servo motor position 
    servo.write(Position);
    delay(15);
  }
  }
  
}

void debounceSwitch(){
  //En pin determines whether or not the Servo will be enabled
  if((long)(millis()-last_millis)>=debouncing_time*10){
    En=1-En;
    last_millis=millis();
  }
}
