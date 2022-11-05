
// ultrasonic needs explaing
void ultrasonic() {
  switch(stage){
    case 0:
      digitalWrite(trigPin, LOW);
      prev = micros();
      stage = 1;
      break;
    case 1:
      if (micros() - prev >= 2) {
        digitalWrite(trigPin, HIGH);
      }
      prev = micros();
      stage = 2;
      break;
    case 2:
      if (micros() - prev >= 10) {
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH);
        distance = duration * 0.034 / 2;
      }
      stage = 0;
      break;
  }
}

/*
void calBlack(){
  int averaging = 1000;
  int Line1[averaging];
  int Line2[averaging];
  int goawayvar[averaging];

  for(int i = 0; i<=averaging; i++){
    Line1[i] = analogRead(LineSensorOne);
    Line2[i] = analogRead(LineSensorTwo);
    delay(1);
  }
  double sumLine1 = 0;
  double sumLine2 = 0;
  int Lin1min = 1024;
  int Lin1max = 0;
  int Lin2min = 1024;
  int Lin2max = 0;
    
  for(int i = 0; i <= averaging; i++){
    sumLine1 += Line1[i];
    sumLine2 += Line2[i];
    if (Line1[i] < Lin1min and Line1[i] > 0) {
          Lin1min = Line1[i];
    }
    if (Line1[i] > Lin1max  and Line1[i] < 900) {
        Lin1max = Line1[i];
    }
    if (Line2[i] < Lin2min and Line2[i] > 0) {
          Lin2min = Line2[i];
    }
    if (Line2[i] > Lin2max  and Line2[i] < 900) {
        Lin2max = Line2[i];
    }
  }
  BlackLine1 = sumLine1 / averaging;
  BlackLine2 = sumLine2 / averaging;
  Serial.println("");
  Serial.println("Callibration of Black:");  
  Serial.println(Lin1min);
  Serial.println(Lin2min);
  Variation1 = Lin1max - Lin1min;
  Variation2 = Lin2max - Lin2min;
  
  Serial.println(Variation1);
  Serial.println(Variation2); 

}*/

void calHALL(){
  // function for calibrating trigger value for Hall Effect (HE) sensor

  int averaging = 1000;
  int Hall1[averaging];

  for(int i = 0; i<=averaging; i++){
    // populate array with readings from HE sensor every millisecond
    Hall1[i] = analogRead(HALLEFFECT);
    delay(1);
  }

  double sumLine1 = 0;
  int Lin1min = 1024;
  int Lin1max = 0;
    
  for(int i = 0; i <= averaging; i++){
    sumLine1 += Hall1[i];
    if (Hall1[i] < Lin1min and Hall1[i] > 0) {
          Lin1min = Hall1[i];
    }
    if (Hall1[i] > Lin1max  and Hall1[i] < 900) {
        Lin1max = Hall1[i];
    }
  }
  Hall1Cal = sumLine1 / averaging;
  Serial.println("");
   

}
/*
bool OnALine(int sensor, int side, float change, bool prevonLine){
  switch(side){
    case 1: 
      if (((sensor >= BlackLine1 + 100) and !prevonLine) or change > 10){
        return true;
      }else if (prevonLine and (sensor <= BlackLine1-Variation1/10)){
        return false;
      }else{
        return prevonLine;
      }
      break;
    case 2: 
      if (((sensor >= BlackLine2+100) and !prevonLine) or change > 10){
        return true;
      }else if (prevonLine and (sensor <= BlackLine2-Variation2/10)){
        return false;
      }else{
        return prevonLine;
      }
      break;
  }

}*/


void TurnLeft(){
  //Turn on the spot slowly
  MotorRunType = 2;
  LeftMotor->setSpeed(200);
  LeftMotor->run(BACKWARD);
  
  RightMotor->setSpeed(200);
  RightMotor->run(FORWARD);
 
}

void TurnRight(){
  //Turn on the spot slowly
  MotorRunType = 3;
  LeftMotor->setSpeed(200);
  LeftMotor->run(FORWARD);
  
  RightMotor->setSpeed(200);
  RightMotor->run(BACKWARD);
}
void TurnLeftForward(){
// While moving, turn left
  MotorRunType = 4;
  LeftMotor->setSpeed(50);
  LeftMotor->run(FORWARD);
  
  RightMotor->setSpeed(220);
  RightMotor->run(FORWARD);

}
void TurnRightForward(){
  //While moving, turn right
  MotorRunType = 5;
  LeftMotor->setSpeed(220);
  LeftMotor->run(FORWARD);
  
  RightMotor->setSpeed(50);
  RightMotor->run(FORWARD);
}

void Backwards(){

  // just go backwards
  MotorRunType = 7;
  LeftMotor->setSpeed(220);
  LeftMotor->run(BACKWARD);
  
  RightMotor->setSpeed(215);
  RightMotor->run(BACKWARD);
}

void Forward(){
  // just go forward
  MotorRunType = 1;
  LeftMotor->setSpeed(220);
  LeftMotor->run(FORWARD);
  
  RightMotor->setSpeed(215);
  RightMotor->run(FORWARD);
}
void Stop(){
  // just go stop
  MotorRunType = 0;
  LeftMotor->setSpeed(0);
  LeftMotor->run(FORWARD);
  
  RightMotor->setSpeed(0);
  RightMotor->run(FORWARD);
}
void Crawl(){
  // just go forward
  MotorRunType = 6;
  LeftMotor->setSpeed(100);
  LeftMotor->run(FORWARD);
  
  RightMotor->setSpeed(100);
  RightMotor->run(FORWARD);
}


void CloseClaws(){
  // sets servo to appropriate angle for grabbing
  Servo1.write(100);
}

void OpenClaws(){]
  // sets servo to widest possible claw width (for ramp clearance, being able to fit in start box, etc.)
  Servo1.write(135);
}

int IRDataStore[] = {0,0}; // first entry stores IR reading at previous loop execution, second entry stores current IR reading
int position = 0; // what is this used for?

bool boxStartFound = false; // checks whether first box edge has been detected
bool boxEndFound = false; // checks whether last box edge has been detected
bool boxLocated = false; // checks whether robot is facing centre of box
long boxstart = 0; // stores time value at which first box edge is found
long boxend = 0; // stores time value at which last box edge is found

bool StartProgram = true; // used to ensure conditions for starting search function are correct (see below)

bool isBlockMagnetic(){
  // returns true if HE reading is above trigger, false if below
  int HallReading = analogRead(HALLEFFECT);
  int magnetictrigger = Hall1Cal + 10;
  if (HallReading >= magnetictrigger){
    return true;
  }
  else {
    return false;
  }
}

void Grab(){
  OpenClaws();
  CloseClaws();
}

bool OnBlock(){
  // if current IR reading is above trigger, return true, otherwise return false
  int IRSensor = analogRead(IRLONG);
  Serial.print("Approaching");
  Serial.println(IRSensor);
  if(IRSensor >= 550){
    return true;
  }else{
    return false;
  }
}


void Approach(void *(whereAmI)()){
  // moves towards block until it is within grabbing distance
  long prevwhile = millis();
  // while loop runs until OnBlock triggered
  while(!OnBlock()){
    if (millis() - prevwhile >= 1){ // loop runs every millisecond
      prevwhile = millis();
      // move towards block and update position and orientation continuously
      whereAmI();      
      Backwards(); 
    }   
  }

  // Robot move towards block for set duration to cover the extra distance the IR sensor cannot see
  long startmillis = millis();
  while(millis()-startmillis < 1500){
    if (millis() - prevwhile >= 1){
      prevwhile = millis();
      // move towards block and update position and orientation continuously
      whereAmI();      
      Backwards(); 
    }   
  }
  
  Stop();
  whereAmI(); // update position and orientation after stopping
  BlockMagnetic = isBlockMagnetic(); // check if block is magnetic
  delay(2000);
  Grab();
}



// search function uses current IR reading as input and uses the positioning function
bool Search(int IRVal, void *(whereAmI)()){
  // spin on the spot
  if(!StartProgram){ // StartProgram ensures the previous IR datastore has been populated before executing function
    TurnLeft(); // spin left on the spot
    IRDataStore[1] =  IRVal; // update current IR data store to current reading, which is the input

    //scanning IR data
    if((IRDataStore[1] - IRDataStore[0]) > 100 and IRDataStore[1] > 200){
      // if required spike in readings is detected, record time of spike and first box edge is found
        Serial.println("Edge Detected (start of box)");
        boxstart = millis();
        boxStartFound = true;
    }
    if(((IRDataStore[1] - IRDataStore[0]) < 100) and boxStartFound == true and IRDataStore[1] > 200){ // shouldn't it be IRDataStore[0] < 200?
      // if required drop in readings is detected, record time of trough ad last box edge is found
      Serial.println("Edge Detected (end of box)");
      boxend = millis();
      boxEndFound = true;
    }
    
    
    if (boxStartFound == true and boxEndFound == true) {
      // if both box edges found, rotate until robot points to centre of box
      
      float boxwidth = boxend - boxstart; // time needed for robot to spin from facing one edge of the box to facing the other

      // reset booleans finding box edges to false default
      boxStartFound = false;
      boxEndFound = false;

      Stop();
      
      // record time at which spinning to centre of box begins
      long a = millis();
      long prevturn = millis();

      while ((millis() - a) < (boxwidth/2)){
        // spin robot for same time as the (difference in time taken to find last and first box edges)/2

        if (millis() - prevturn >= 1){ // while loop is executed every millisecond
          // continuously turn on the spot right and update position and orientation while doing so
          TurnRight();
          whereAmI(); 
          prevturn = millis();
        }
      }
      
      Stop();

      // reset current and previous IR data stores
      IRDataStore[0] = 0;
      IRDataStore[1] = 0;

      boxLocated = true; // robot is now pointing towards centre of block

      return true; // return true to stop search execution in next loop
    }
    
    IRDataStore[0] = IRDataStore[1];// set previous IR data store to current for next loop execution
    return false; // return false so that search continues being executed in main loop

  }else{
    // ensures previous IR data store is populated with current value to allow function to start
    IRDataStore[1] =  IRVal;
    IRDataStore[0] = IRDataStore[1];
    StartProgram = false; // searching can now start

    return false; // return false so that search continues being executed in main loop
  }
}


void ReturnToLine(double long linePosition[], void *(whereAmI)()){
  //return to the line  

  // find unit vector pointing towards the last point the robot was on the line
  long dir1 = Position[0] - linePosition[0];
  long dir2 = Position[1] - linePosition[1];
  dir1 / sqrt(dir1*dir1 + dir2*dir2);
  dir2 / sqrt(dir1*dir1 + dir2*dir2);

  long returnprev = millis();
  //point in the right direction
  while(!(dir1+0.05 >= Direction[0] or dir1-0.05 <= Direction[0]) and !(dir2+0.05 >= Direction[1] or dir2-0.05 <= Direction[1])){
    // robot spins until its vector is roughly parallel to above calculated unit vector
    if (millis() - returnprev >= 1){ // if statements causes loop to run every millisecond
      returnprev = millis();
      // turn right on the spot and update position and orientation continuously
      TurnRight();
      whereAmI();
    }    
  }

  Forward();

}
