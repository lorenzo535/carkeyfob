// board DYIMore Beetle = Genuino Micro 

#include <Streaming.h>

//timer2 will interrupt at 8kHz

short int zeros, pos;
#define BUFFER_SIZE 256
boolean vector[BUFFER_SIZE];


boolean store , look_for_rising_edge, got_frame;


void setup() {


 pinMode(9, INPUT);
 pinMode(LED_BUILTIN, OUTPUT);
   store = false;
  look_for_rising_edge = false;
  got_frame = false;
  zeros = 0;
  pos = 0;


  cli();//stop interrupts


  //set timer2 interrupt at 8kHz
  TCCR1A = 0;// set entire TCCR2A register to 0
  TCCR1B = 0;// same for TCCR2B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 10khz increments
  OCR1A = 24;// = (16*10^6) / (10000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR1A |= (1 << WGM11);
  // Set CS21 bit for 64 prescaler
  TCCR1B |= (1 << CS01)|(1<<CS00);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


  sei();//allow interrupts


  Serial.begin(9600);


}//end setup






ISR(TIMER1_COMPA_vect) //timer1 interrupt 8kHz
{
    
  if (pos > BUFFER_SIZE -1) 
  { 
    got_frame = true;
   // cli();   
    return;
  }
    
  bool pinvalue = digitalRead(9);

  /*
  vector[pos] = pinvalue;
  pos++;
  return;
  */ 
  
  if (look_for_rising_edge)
  {
    if (pinvalue)
    {
      look_for_rising_edge = false;
      store = true;
      pos = 0;
    
    }
  }

  if (store)
  {
    vector[pos] = pinvalue;
    pos++;
    return;    
  }
     
  if (!look_for_rising_edge)
  {
    if (!pinvalue)
      zeros++;
    else zeros = 0;

    if (zeros > 50)
    {
      look_for_rising_edge = true;
      
    }
  }

}


void loop() {
  //do other things here
  int i;
  if (got_frame)
  {
    //cli();//stop interrupts
    //Serial << "got something " <<"\n";
    for (i = 0; i < BUFFER_SIZE; i++)
      Serial << vector[i]<<"\n";
   //Serial <<"\n";
   //sei();
   
  store = false;
  look_for_rising_edge = false;
  got_frame = false;
  zeros = 0;
  pos = 0;

  }
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(200);                       // wait for a second
}
