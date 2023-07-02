//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Defines
//
//////////////////////////////////////////////////////////////////////////////////////////////

//Status
#define IDLE 0
#define ARMED 1
#define TRIGGERED 2
#define DISARMED 3

//Timer
#define TIMER 30
#define TIMER_ENDED 0
#define TIMER_RUNNING 1

//Buttons
#define BUTTON_GREEN 1
#define BUTTON_RED 2

//Tone
#define FREQUENCY 1000
#define DURATION 200

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Global Variables
//
//////////////////////////////////////////////////////////////////////////////////////////////

//Pin variables
int ledGreen = D0;
int ledYellow = D1;
int speaker = D2;
int buttonYellow = D3;
int buttonGreen = D4;
int buttonRed = D5;
int doorSwitch = D6;
int ledRed = D7;

//Flank detection
int isStatDoorSwitch = 0;
int isStatBttnYellow = 0;
int isStatBttnGreen = 0;
int isStatBttnRed = 0;
int wasStatBttnYellow = 0;
int wasStatBttnGreen = 0;
int wasStatBttnRed = 0;
int flankBttnYellow = 0;
int flankBttnGreen = 0;
int flankBttnRed = 0;

//Status, timer, pin code variables
int status = 0;
int timestamp = 0;
int timerState = 0;
const int pinCode[3] = {BUTTON_GREEN,BUTTON_GREEN,BUTTON_RED};
int BufferPinCodeInput[3] = {0,0,0};

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Setup loop for pin modes
//
//////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
    
    pinMode(doorSwitch, INPUT_PULLDOWN);
    pinMode(buttonYellow, INPUT_PULLDOWN);
    pinMode(speaker, OUTPUT);
    pinMode(buttonGreen, INPUT_PULLDOWN);
    pinMode(buttonRed, INPUT_PULLDOWN);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledYellow, OUTPUT);
    pinMode(ledRed, OUTPUT);
    
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function prototypes
//
//////////////////////////////////////////////////////////////////////////////////////////////

void buttonHandler();
int timer(int timestamp);
void pinCodeInput();
void resetPinCodeInput();

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Main loop
//  Checks the status und performs actions accordingly
//
//////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

    buttonHandler();                                //Check buttons
        
    if(status == IDLE && flankBttnYellow == 1){     //Set system from idle(default) to armed when yellow button was pressed
            
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, LOW);
        tone(speaker, FREQUENCY, DURATION);
        status = ARMED;
   
    } 
        
    if(status == ARMED && isStatDoorSwitch == 0){   //Set system from armed to triggered when door is opened
            
        timestamp = Time.now();                     //Set start time for timer
        digitalWrite(ledYellow, HIGH);
        tone(speaker, FREQUENCY);
        status = TRIGGERED;
            
    }
        
    if(status == TRIGGERED){                        //If alarm is triggered
            
        timerState = timer(timestamp);              //Start timer and check if time ran out
            
        if(timerState == TIMER_RUNNING){            //If timer is still running do this
            
            pinCodeInput();

        }
            
        if(timerState == TIMER_ENDED){              //If timer ran out do this
            
            digitalWrite(ledYellow, LOW);
            digitalWrite(ledRed, HIGH);
            noTone(speaker);
            resetPinCodeInput();
            status = IDLE;

        }   
    }
        
    if(status == DISARMED){                         //If system is disarmed do this
            
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledYellow, LOW);
        noTone(speaker);
        delay(220);
        tone(speaker, FREQUENCY, DURATION);
        delay(220);
        tone(speaker, FREQUENCY, DURATION);
        status = IDLE;
            
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//	The button handler is watching the button states and detects changes of those states.
//	It is needed to prevent multiple button detections.
//
//////////////////////////////////////////////////////////////////////////////////////////////

void buttonHandler(){
    
    isStatBttnYellow = digitalRead(buttonYellow);               //read the current state of the buttons
    isStatBttnGreen = digitalRead(buttonGreen);
    isStatBttnRed = digitalRead(buttonRed);
    isStatDoorSwitch = digitalRead(doorSwitch);
    
    if (isStatBttnYellow == 1 && wasStatBttnYellow == 0){       //If button was not pressed in cycle before (was state)

		flankBttnYellow = 1;                                    //set flank (active for one cycle)

	} else { 
	    
	    flankBttnYellow = 0;                                    //If button was pressed in cycle before (was state) reset flank 
	    
	}
	
	if (isStatBttnGreen == 1 && wasStatBttnGreen == 0){

		flankBttnGreen = 1;

	} else { 
	    
	    flankBttnGreen = 0;
	    
	}
	
	if (isStatBttnRed == 1 && wasStatBttnRed == 0){

		flankBttnRed = 1;

	} else { 
	    
	    flankBttnRed = 0;
	    
	}
	
	wasStatBttnYellow = isStatBttnYellow;                       //save button states from this cycle for comparison in next cycle
	wasStatBttnGreen = isStatBttnGreen;
	wasStatBttnRed = isStatBttnRed;
    
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Timer function. The duration of timer is adjustable in #define TIMER.
//	
//////////////////////////////////////////////////////////////////////////////////////////////

int timer(int timestamp){
    
    if(Time.now() - timestamp >= TIMER){
        
        return TIMER_ENDED;                     //return when timer ended
        
    } else {
        
        return TIMER_RUNNING;                   //return while timer is running
        
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  The pin code input function handles the pin code input and checks if the code is correct.
//	
//////////////////////////////////////////////////////////////////////////////////////////////

void pinCodeInput(){
    
    if(flankBttnGreen == 1){                                //move array by one and add current button pressed
        
        BufferPinCodeInput[0] = BufferPinCodeInput[1];
        BufferPinCodeInput[1] = BufferPinCodeInput[2];
        BufferPinCodeInput[2] = BUTTON_GREEN;
        
    }
    
    if(flankBttnRed == 1){                                  //move array by one and add current button pressed
       
        BufferPinCodeInput[0] = BufferPinCodeInput[1];
        BufferPinCodeInput[1] = BufferPinCodeInput[2];
        BufferPinCodeInput[2] = BUTTON_RED;
        
    }
    
    if(memcmp(BufferPinCodeInput, pinCode, sizeof(BufferPinCodeInput)) == 0){       //check if pin code is correct
        
        resetPinCodeInput();
        status = DISARMED;
        
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  The reset pin code input function resets the pin code input array.
//
//////////////////////////////////////////////////////////////////////////////////////////////

void resetPinCodeInput(){
    
    memset(&BufferPinCodeInput, 0 , sizeof(BufferPinCodeInput));
    
}