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
  int averaging = 1000;
  int Hall1[averaging];

  for(int i = 0; i<=averaging; i++){
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
  Servo1.write(100);
}

void OpenClaws(){
  Servo1.write(135);
}

int IRDataStore[] = {0,0};
int position = 0;

bool boxStartFound = false;
bool boxEndFound = false;
bool boxLocated = false;
long boxstart = 0;
long boxend = 0;

bool StartProgram = true;

bool isBlockMagnetic(){
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
  int IRSensor = analogRead(IRLONG);
  Serial.print("Approaching  ");
  Serial.println(IRSensor);
  if(IRSensor >= 550){
    return true;
  }else{
    return false;
  }
}


void Approach(void *(whereAmI)()){

  long prevwhile = millis();
  while(!OnBlock()){
    if (millis() - prevwhile >= 1){
      prevwhile = millis();
      whereAmI();      
      Backwards(); 
    }   
  }
  // little extra distance the sensor cannot see.
  long startmillis = millis();
  while(millis()-startmillis < 1500){
    if (millis() - prevwhile >= 1){
      prevwhile = millis();
      whereAmI();      
      Backwards(); 
    }   
  }
  Stop();
  whereAmI();
  BlockMagnetic = isBlockMagnetic();
  //turn on LED's for Magnet
  delay(2000);
  Grab();
}




bool Search(int IRVal, void *(whereAmI)()){
  // turn on the spot and look for an object. then face the object and go towards it
  if(!StartProgram){
    TurnLeft();
    IRDataStore[1] =  IRVal;

    //scanning IR data
    if((IRDataStore[1] - IRDataStore[0]) > 100 and IRDataStore[1] > 200){
        Serial.println("Edge Detected (start of box)");
        boxstart = millis();
        boxStartFound = true;
    }
    if(((IRDataStore[1] - IRDataStore[0]) < 100) and boxStartFound == true and IRDataStore[1] > 200){
      Serial.println("Edge Detected (end of box)");
      boxend = millis();
      boxEndFound = true;
    }
    
    //Long range detection and scanning
    if (boxStartFound == true and boxEndFound == true) {
      float boxwidth = boxend - boxstart;
      boxStartFound = false;
      boxEndFound = false;
      Stop();
      long a = millis();
      long prevturn = millis();
      while ((millis() - a) < (boxwidth/2)){
        if (millis() - prevturn >= 1){
          TurnRight();
          whereAmI();
          prevturn = millis();
        }
      }
      Stop();
      IRDataStore[0] = 0;
      IRDataStore[1] = 0;
      boxLocated = true;
      return true;
    }
    
    IRDataStore[0] = IRDataStore[1];
    return false;
  }else{
    IRDataStore[1] =  IRVal;
    IRDataStore[0] = IRDataStore[1];
    StartProgram = false;
    return false;
  }
}


void ReturnToLine(double long linePosition[], void *(whereAmI)()){
  //return to the line  
  long dir1 = Position[0] - linePosition[0];
  long dir2 = Position[1] - linePosition[1];

  dir1 / sqrt(dir1*dir1 + dir2*dir2);
  dir2 / sqrt(dir1*dir1 + dir2*dir2);
  long returnprev = millis();
  //point in the right direction
  while(!(dir1+0.05 >= Direction[0] or dir1-0.05 <= Direction[0]) and !(dir2+0.05 >= Direction[1] or dir2-0.05 <= Direction[1])){
    // if statements runs causes loop to run every millisecond
    if (millis() - returnprev >= 1){
      returnprev = millis();
      TurnRight();
      whereAmI();
    }    
  }

  Forward();

}
