//Eric Lyons 2014
//Have fun!

#include "LedControl.h"

/* 
 * Create a new controler 
 * Params :
 * int dataPin    The pin on the Arduino where data gets shifted out
 * int clockPin   The pin for the clock
 * int csPin      The pin for selecting the device when data is to be sent
 * int numDevices The maximum number of devices that can be controled
*/

//LedControl(int dataPin, int clkPin, int csPin, int numDevices);
LedControl lc=LedControl(4,2,3,1);

int pin = 5; // INPUT from switch

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long ball_in_trough =0;
unsigned long last_ball_in_trough=0;
int debounceDelay = 500; 
unsigned long clockTime =0;
int clock_running = 0; // is the clock running?

void setup() {
  pinMode(pin, INPUT);
  Serial.begin(9600);
  for(int index=0;index<lc.getDeviceCount();index++) {
    lc.shutdown(index,false);
    lc.setIntensity(index, 15);
    printTime(0);
  }
}


void loop() {
  //if clock is running, show time
  if (clock_running) {printTime(millis()-clockTime);}
  
  //state will be low when ball is in trough
  //state will be high when ball is not in trough
  int state = digitalRead(pin);
  
//  Serial.println(state);
//  printNumber(123);
  if (state == 0) {
    ball_in_trough++;
  }
  if (ball_in_trough > 1000000){ball_in_trough = 100000;} // to keep the number from overrunning buffer (just in case)

  //let's check the state every debounce Period
  if (millis() - lastDebounceTime > debounceDelay) {
    //debuggin stuff to figure out correct limits for ball_in_trough and logic for starting stopping clock
    Serial.print(state);
    Serial.print(" ");
    Serial.println(ball_in_trough);

    //reset debounce
    lastDebounceTime = 0;

    //check to see if ball has been released from trough.  If so, reset ball_in_trough
    if (ball_in_trough > 10 && ball_in_trough == last_ball_in_trough) { // ball has been released
        ball_in_trough =0;
    }

    //check to see what to do with starting/stopping clock
    if (clock_running && ball_in_trough > 500) { // clock is running; ball in trough: stop clock
      clock_running = 0;
      //need to make sure that ball saver isn't saving the ball.  Usually ~5 seconds for ball saver, then another 5 seconds for the ball to be plunged.
      if (millis()-clockTime > 10000) {countDown();}
    } 
    else if (clock_running == 0 && ball_in_trough < 10) { //ball out of trough, clock not started:  start clock!
      clock_running=1;
      startClock();
    }

    //ball_in_trough will creep up as other switches are actived.  Reset back to 0    
    if (clock_running && ball_in_trough < 10) {
      ball_in_trough = 0;
    }

    //record last state of ball_in_trough
    last_ball_in_trough = ball_in_trough;

    lastDebounceTime = millis();
  }
}

//10 second countdown for people to check out their ball time
void countDown() {
  for (int i=0; i<10; i++)
    {
       lc.setDigit(0,7,9-i,false);
       delay (100);
       lc.setChar(0,7,' ',false);
       delay (100);
       lc.setDigit(0,7,9-i,false);
       delay (100);
       lc.setChar(0,7,' ',false);
       delay (100);
       lc.setDigit(0,7,9-i,false);
       delay (100);
       lc.setChar(0,7,' ',false);
       delay (100);
       lc.setDigit(0,7,9-i,false);
       delay (100);
       lc.setChar(0,7,' ',false);
       delay (100);
       lc.setDigit(0,7,9-i,false);
       delay (100);
       lc.setChar(0,7,' ',false);
       delay (100);
    }  
}

void resetClock(){
  clockTime=0;
}

void startClock(){
  clockTime = millis();
}
void printTime(unsigned long v) {
    unsigned long ones;
    unsigned long tens;
    unsigned long hundreds;

    //milli ignore
    ones=v%10;
    //10 milli ignore
    v=v/10;
    tens=v%10;
    //hundredths
    v=v/10;
    hundreds=v%10;			
    //tenths
    v=v/10;

    unsigned long minutes = v/60;
    unsigned long seconds = v%60;
    //seconds
    unsigned long thousands=seconds%10;			
    //10s seconds
//    v=v/10;
    unsigned long tenthousands=seconds/10;			
    //minutes
//    v=v/10;
    unsigned long hundredthousands=minutes%10;			
    minutes=minutes/10;
    unsigned long millions=minutes%10;			

    //Now print the number digit by digit
    //10s of minutes
    lc.setDigit(0,5,(int)millions,false);
    //minutes
    lc.setDigit(0,4,(int)hundredthousands,true);

    //mark for minutes/seconds separation
//    lc.setChar(0,4,'_',false);

    //10s seconds
    lc.setDigit(0,3,(int)tenthousands,false);
    //seconds
    lc.setDigit(0,2,(int)thousands,true);
    //tenths of second
    lc.setDigit(0,1,(int)hundreds,false);
    //hundredths of second
    lc.setDigit(0,0,(int)tens,false);
}

void printNumber(int v) {
    int ones;
    int tens;
    int hundreds;
    boolean negative;	

    if(v<0) {
        negative=true;
        v=v*-1;
    }
    ones=v%10;
    v=v/10;
    tens=v%10;
    v=v/10;
    hundreds=v;			
    if(negative) {
       //print a minus on the first MAX72XX, 4'th digit from the right, no decimal-point	
       lc.setChar(0,3,'-',false);
    }
    else {
       //print a blank in the sign column
       lc.setChar(0,3,' ',false);
    }
    //Now print the number digit by digit
    lc.setDigit(0,2,(byte)hundreds,false);
    lc.setDigit(0,1,(byte)tens,false);
    lc.setDigit(0,0,(byte)ones,false);
}

