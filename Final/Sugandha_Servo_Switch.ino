int pot = A11;  // analog pin used to connect the potentiometer
int AvgFilter[30]={0};
int sum=0;
int i=0;
//volatile unsigned int En;
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
    for (i=0;i<30;i++)
    {
      AvgFilter[i]=analogRead(pot);
      sum=sum+AvgFilter[i];
    }
    int avg=round(sum/30);
    
    //This condition is when the GUI requests that Servo Motor position be controlled
    //by reading Potentiometer values
    PotentiometerVal=avg;     // reads the value of the potentiometer (value between 0 and 1023) 
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

