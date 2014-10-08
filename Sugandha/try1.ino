#include <Servo.h>
  
Servo servo;  //create servo object

int pot = A1;  // analog pin used to connect the potentiometer
int val;

void setup() 
{ 
  //control servo via pin 9 
  servo.attach(9, 650, 2350);
  //start serial comm. for debugging
  Serial.begin(9600);
} 

void loop() 
{ 
   val = analogRead(pot);       // reads the value of the potentiometer (value between 0 and 1023) 
   Serial.println(val);
   val = map(val, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 169) 
   Serial.println(val);
   servo.write(val);                  // sets the servo position according to the scaled value  
   delay(15);
}

