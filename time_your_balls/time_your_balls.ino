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


int pin = 5; // INPUT from switch

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long ball_in_trough =0; //counter for when ball is in trough
unsigned long last_ball_in_trough=0; // last count for ball_in_trough
int debounceDelay = 500; //time between checking on the state of things
unsigned long clockTime =0; //counter for how long has the ball been in play
int clock_running = 0; // is the clock running?

//LedControl(int dataPin, int clkPin, int csPin, int numDevices);
// dataPIn: 4
// clkPin: 2
// csPin (load): 3
// numDevices: 1
LedControl lc=LedControl(4,2,3,1);

void setup() {
  pinMode(pin, INPUT);
//  Serial.begin(9600); // for debugging
  lc.shutdown(0,false);
  lc.setIntensity(0, 15);
  printTime(0);
  countDown();
  lastDebounceTime= millis();
}

// My display was having problems initializing when connected to an external power source.  
// Worked fine when connected to a USB port, but I wasn't going to keep a computer in the pinball machine at all times
// My solution was to do a second intiialization when the main loop started to run.
int initialized = 0;  //Have we done the second initialization of the display?
void loop() {
  if (initialized = 0) { // first time through, reinitialize the display
     delay(1000);
     LedControl lc=LedControl(4,2,3,1);
    lc.shutdown(0,false);
    lc.setIntensity(0, 15);
    initialized = 1; // set flag so initialization only happens once
  }
  //if clock is running, show time
  if (clock_running) {printTime(millis()-clockTime);}

  //state will be low when ball is in trough
  //state will be high when ball is not in trough
  int state = digitalRead(pin);
  
//  Serial.println(state); // debugging
  if (state == 0) {
    ball_in_trough++;
  }
  if (ball_in_trough > 1000000){ball_in_trough = 100000;} // to keep the number from overrunning buffer (just in case)

  //let's check the state every debounce Period
  if (millis() - lastDebounceTime > debounceDelay) {
    //debuggin stuff to figure out correct limits for ball_in_trough and logic for starting stopping clock
//    Serial.print(state);
//    Serial.print(" ");
//    Serial.println(ball_in_trough);

    //reset debounce
    lastDebounceTime = 0;

    //check to see if ball has been released from trough.  If so, reset ball_in_trough
    if (ball_in_trough > 10 && (ball_in_trough -last_ball_in_trough)<10) { // ball has been released
        // the logic is that the ball_in_trough count will increase slowly (if at all) between checks 
        // because the power supplied to one side of the switch from the arduino is running through the switch
        // when there is no ball in the trough and closing the switch (switch is open when ball is released).  
        // The power then runs to the arduino input pin for checking on the switch's state.  
        // This has a pull-down to the ground.  THe power running across  the switch must be high enough to 
        // overrun the pull-down.  

        ball_in_trough =0; // reset counter to 0
    }

    //check to see what to do with starting/stopping clock
    if (clock_running && ball_in_trough > 200) { // clock is running; ball in trough: stop clock
      // The number of 200 is emperically choosen.  This is based on the number of LOWs sensed when the switch is closed.
      
      clock_running = 0; // turn off the clock

      // Need to make sure that ball saver isn't saving the ball.  Usually ~5 seconds for ball saver, 
      // then another 5 seconds for the ball to be plunged.
      if (millis()-clockTime > 10000) {countDown();}
    } 
    else if (clock_running == 0 && ball_in_trough < 10) { //ball out of trough, clock not started:  start clock!
      clock_running = 1;
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
    ball_in_trough = 0;
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


