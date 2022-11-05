#include <Servo.h>

// Declare Servo pin
int servoPin = 10;

// Create an object for servo
Servo Servo1;

#define HALLEFFECT A0
#define IRLONG A1
#define LineSensorLeft 4
#define LineSensorMid 5
#define LineSensorRight 6
#define LeftLED 1
#define RightLED 0
#define MagneticRed 8
#define NonMagneticGreen 9
#define Flashing 7
#define Switch 10


#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

long prev = 0; //timer for program so 1ms loop time is consistent.
bool AveragingSensors = true;


float Direction[] = {1,0}; //direction is a unit vector
float Position[] = {50,2500}; //position is miliseconds at forward speed in the direction

#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(1);
// You can also make another motor on port M2
Adafruit_DCMotor *RightMotor = AFMS.getMotor(2);

float Hall1Cal = 0;
bool BlockMagnetic = false;

int MotorRunType = 0; //0= stop, 1 = forward, 2 = left, 3 = right, 4 = forward left, 5 = forward right, 6 = crawl, 7 = backwards
int ProgramPhase = 0; //0=start box, 1=Line Found, 2 = tunnel, 3 = found line
bool inSearchArea = false;
bool inStartBox = true;
bool inTunnel = false;
bool onRamp = false;
bool HoldingBlock = false;


bool searchStartTrig = true; // trigger for starting the search routine
bool tunnelStartTrig = false; // trigger for starting the tunnel run routine

long prevloop = 0; // number for storing current time, used to make the main loop run every millisecond
int stage = 0; // sets default stage for ultrasonic sensor to 0

// booleans to show which line sensor is triggering
bool onTheLineLeft = false;
bool onTheLineMid = false;
bool onTheLineRight = false;

// what are these?
int Line1Val = 0;
int Line2Val = 0;


long linelost = 0;
bool linelostTrig = false;

bool SearchTrig = false;
bool setflip = true;
float Flip[] = {0,0};


#include "Utils.h"

#include "Sequencing.h"


void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  pinMode(LeftLED, OUTPUT);
  pinMode(RightLED, OUTPUT);
  pinMode(MagneticRed, OUTPUT);
  pinMode(NonMagneticGreen, OUTPUT);
  pinMode(Flashing, OUTPUT);
  pinMode(Switch, INPUT);

  pinMode(LineSensorLeft, INPUT);
  pinMode(LineSensorMid, INPUT);
  pinMode(LineSensorRight, INPUT);
  
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  
  Serial.println("Motor Shield found.");

  LeftMotor->run(RELEASE);
  RightMotor->run(RELEASE);

  delay(2000);
  Forward();

  Servo1.attach(10);
  
  Servo1.write(135);

  calHALL();

  Serial.println("Sensor's");
  
  prev = millis();
}

void loop() {
  // only run if switch is pressed
  if (!digitalRead(Switch)){
    // only run the loop once a millisecond 
    if(millis()-prevloop >= 1){
      prevloop = millis();
      
      ultrasonic();
      
      //update my position
      whereAmI();

      //read sensors
      int HallVal = analogRead(HALLEFFECT);
      int IRLongVal = analogRead(IRLONG);
      
      onTheLineLeft = digitalRead(LineSensorLeft);
      onTheLineMid = digitalRead(LineSensorMid);
      onTheLineRight = digitalRead(LineSensorRight);
      
      
      //if in the search area look for the block pick it up and go back to the line
      if(inSearchArea and !HoldingBlock){
        // check if robot has set up for running search routine yet
        if (searchStartTrig){
          // 
          float targetdir[] = {-Direction[1], Direction[0]};
          long prevturn = millis();
          while (!((targetdir[0]-0.05 < Direction[0] and  Direction[0] < targetdir[0]+0.05 ) and (targetdir[1]-0.05 < Direction[1] and Direction[1] < targetdir[1]+0.05 ))){
            if (millis() - prevturn >= 1){
              TurnLeft();
              whereAmI();
              prevturn = millis();
              Serial.println("Turning");
            }
          }
          searchStartTrig = false;
        }
        if (Search(IRLongVal, &whereAmI)){
          double long linePosition[] = {Position[0],Position[1]};
          Approach(&whereAmI);
          HoldingBlock = true;
          ReturnToLine(linePosition, &whereAmI);
          setflip = true;
          SearchTrig = false;
        }
      // if holding the block and in the start Box area go to the correct box and put it in there.
      }else if(HoldingBlock and inStartBox){
        if (BlockMagnetic){
          //go to red
          HoldingBlock = false;
          digitalWrite(MagneticRed, HIGH);
          digitalWrite(NonMagneticGreen, LOW);
        }else{
          //go to green
          HoldingBlock = false;
          digitalWrite(NonMagneticGreen, HIGH);
          digitalWrite(MagneticRed, LOW);
        }
      // follow the line
      }else{
        if (onTheLineLeft or onTheLineRight or onTheLineMid){ //trigger for line lost
          linelostTrig = false;
        }
        
        if (!onTheLineLeft and onTheLineRight and !onTheLineMid){
          TurnLeft();
        }else if (!onTheLineLeft and onTheLineRight and onTheLineMid){
          TurnLeftForward();
        }else if (onTheLineLeft and !onTheLineRight and onTheLineMid){
          TurnRightForward();
        }else if (onTheLineLeft and !onTheLineRight and !onTheLineMid){
          TurnRight();
        }else if (onTheLineMid){
          Forward();
        }else{        // cannot see line
          if (inTunnel){
            Serial.print(distance);
            Serial.print(",     ");
            if (distance >= 20){
              Forward();
            }else if (distance >= 7){
              TurnRightForward();
            }else if(distance <= 5){
              TurnLeftForward();
            }else{
              Forward();
            }
          }else if(onRamp){
            Serial.print(distance);
            Serial.print(",     ");
            if (distance >= 40){
              Forward();
            }else if (distance >= 15){
              TurnRightForward();
            }else if(distance <= 9){
              TurnLeftForward();
            }else{
              Forward();
            }
          }
          else{
            if (!linelostTrig){
              linelost = millis();
              linelostTrig = true;
            }
            //find Line
            if (millis() - linelost < 3000){
              if(inStartBox){
                TurnRight();
              }else{
                TurnLeft();
              }
            }else if(millis() - linelost < 9000){
              if(!inStartBox){
                TurnRight();
              }else{
                TurnLeft();
              }
            }else{
              Serial.println("Crawl");
              Crawl();
            }
          }
        }
      }

      
      
      //Calibration Code
      /*
      if (Direction[0] > 0){
        TurnLeft();
      }else{
        Stop();
      }
      */
      /*
      if (Direction[0] > 0){
        TurnLeftForward();
      }else{
        Stop();
      }*/
      
      /*
      if (Position[0] < 5000){
        Forward();
      }else{
        Stop();
      }*/
      Serial.print(Position[0]);
      Serial.print(", ");
      Serial.print(Position[1]);

      Serial.print(",    ");
      Serial.print(Direction[0]);
      Serial.print(", ");
      Serial.println(Direction[1]);

      
      
      

      digitalWrite(LeftLED, onTheLineLeft);
      digitalWrite(RightLED, onTheLineRight);
    }
    
  }
}
