void Sequence(){
}

// calibration values, obtained from rotating robot 90 degreees and creating 
float RotAngleCalibrated = -0.02;//-0.00248;
float ForwardRotMultiplyer = 0.41;

void whereAmI(){
  // if robot is moving, flash yellow LED. Update position and orientation based on previous motor action
  switch(MotorRunType){
    case(0):
    // stopped
      digitalWrite(Flashing, LOW);
      break;
    case(1):
      //forward
      Position[0] += Direction[0]; 
      Position[1] += Direction[1];
      digitalWrite(Flashing, HIGH);

      break;
    case(2):
      //left
      Direction[0] = Direction[0]*cos(RotAngleCalibrated) - Direction[1]*sin(RotAngleCalibrated);
      Direction[1] = Direction[0]*sin(RotAngleCalibrated) + Direction[1]*cos(RotAngleCalibrated);
      digitalWrite(Flashing, HIGH);

      break;
    case(3):
      //right
      Direction[0] = Direction[0]*cos(-RotAngleCalibrated) - Direction[1]*sin(-RotAngleCalibrated);
      Direction[1] = Direction[0]*sin(-RotAngleCalibrated) + Direction[1]*cos(-RotAngleCalibrated);
      digitalWrite(Flashing, HIGH);

      break;
    case(4):
      //forward left

      Position[0] += Direction[0];
      Position[1] += Direction[1];

      Direction[0] = Direction[0]*cos(ForwardRotMultiplyer*RotAngleCalibrated) - Direction[1]*sin(ForwardRotMultiplyer*RotAngleCalibrated);
      Direction[1] = Direction[0]*sin(ForwardRotMultiplyer*RotAngleCalibrated) + Direction[1]*cos(ForwardRotMultiplyer*RotAngleCalibrated);

      digitalWrite(Flashing, HIGH);

      break;
    case(5):
      //forward right

      Position[0] += Direction[0];
      Position[1] += Direction[1];

      Direction[0] = Direction[0]*cos(-ForwardRotMultiplyer*RotAngleCalibrated) - Direction[1]*sin(-ForwardRotMultiplyer*RotAngleCalibrated);
      Direction[1] = Direction[0]*sin(-ForwardRotMultiplyer*RotAngleCalibrated) + Direction[1]*cos(-ForwardRotMultiplyer*RotAngleCalibrated);

      digitalWrite(Flashing, HIGH);

      break;
    case(6):
      //crawl
      Position[0] += Direction[0]/2;
      Position[1] += Direction[1]/2;

      digitalWrite(Flashing, HIGH);      

      break;
    case(7):
      //backwards
      Position[0] -= Direction[0];
      Position[1] -= Direction[1];

      digitalWrite(Flashing, HIGH);   


      break;
  }
  //board size is 2100 x 2100 starting at 100 1050
  if(Position[0] > 600 and (Position[1] > 2400 and Position[1] < 2700)){
    inSearchArea = true;
  }else{
    inSearchArea = false;
  }

  if(Position[0] < 2400 and (Position[1] > 300 and Position[1] < 5000)){
    inStartBox = true;
  }else{
    inStartBox = false;
  }
  
  if(Position[0] > 220 and Position[0] < 1500 and Position[1] > 2100){
    inTunnel = true;
  }else{
    inTunnel = false;
    tunnelStartTrig = false;
  }
  if(Position[0] > 180 and Position[0] < 800 and Position[0] < 2900 and Position[1] > 2700){
    onRamp = true;
  }else{
    onRamp = false;
  }


}
