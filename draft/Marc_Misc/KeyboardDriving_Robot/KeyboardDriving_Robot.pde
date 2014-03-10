import processing.serial.*;

int _FORWARD = 0;
int _RIGHT = 1;
int _LEFT = 2;
int _BACK = 3;
Serial port;

int[] direction = {0,0,0,0};
int[] oldDirection = {0,0,0,0};

void setup() {
  size(640, 360);
  //noStroke();
  background(0);
  
  //setup serial communication
  String portName = Serial.list()[0];
  port = new Serial(this, portName, 9600);
  println(Serial.list()[0]);
}

void draw() { 
  // keep draw() here to continue looping while waiting for keys
  if(compareDirection()==0)
  {
      clear();
      if(direction[_FORWARD] == 1)
      {
        rect((width/2 - width/8), 0, width/4, height/4);
      }
      if(direction[_RIGHT] == 1)
      {
        rect((0.75*width), (height/2 - height/8), width/4, height/4);
      }
      if(direction[_BACK] == 1)
      {
        rect((width/2 - width/8), (0.75*height), width/4, height/4);
      }
      if(direction[_LEFT] == 1)
      {
        rect(0, (height/2 - height/8), width/4, height/4);
      }
  String msg = "" + direction[0]+direction[1]+direction[2]+direction[3];
  println(msg);
  int msgI = (direction[0]*1000)+(direction[1]*100)+(direction[2]*10)+(direction[3]);
  port.write(direction[0]);
  }
}

void keyPressed()
{
  //int[] direction = {0,0,0,0};
  if (key == 'W' || key == 'w')
    direction[_FORWARD] = 1;
  if (key == 'A' || key == 'a')
    direction[_LEFT] = 1;
  if (key == 'D' || key == 'd')
    direction[_RIGHT] = 1;
  if (key == 'S' || key == 's')
    direction[_BACK] = 1;
}
void keyReleased()
{
  for(int i =0; i<4; i++)
  {
    direction[i] = 0;
  }
}

int compareDirection()
{
  for(int i =0; i<4; i++)
  {
    //println("Direction:", i, direction[i], oldDirection[i]);
    if (direction[i] != oldDirection[i])
    {
      oldDirection[0] = direction[0];
      oldDirection[1] = direction[1];
      oldDirection[2] = direction[2];
      oldDirection[3] = direction[3];
      return 0;
    }
  }
//    printl  n("all good");
  return 1;
}


