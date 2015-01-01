import processing.serial.*;
Serial myPort;
int odl;
PImage img;
void setup()
{
  img = loadImage("ultragami_butterfly.jpg");
  size(800,600);
  myPort = new Serial(this, Serial.list()[5], 9600);  
  myPort.bufferUntil('\n');
  
}
void draw()
{
  //background(odl/20,-125+odl/30,odl/40,0.00000001);
 // stroke(0);
  fill(5, 0,0 ,25);
rect(0,0,800, 600);
  try{
  image(img, 20, 40, odl, odl);
  }catch(Exception e)
  {
  }
  
}

public void serialEvent(Serial myPort)
{
  try
  {
   String fufu=myPort.readStringUntil('\n');
    println(trim(fufu));
    odl= Integer.valueOf(trim(fufu));
  }
  catch(Exception e)
  {
  }
  println(odl);
}
