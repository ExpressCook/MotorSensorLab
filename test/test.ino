int switch1 = 0;

void setup()
{
  pinMode(13,OUTPUT);
  attachInterrupt(switch1, pressButton1, RISING);
}

void pressButton1()
{
  int value = digitalRead(13);
  
  if(value == HIGH)
    value = LOW;
   else
    value = HIGH;
    
  digitalWrite(13,value);
}

void loop()
{
}
