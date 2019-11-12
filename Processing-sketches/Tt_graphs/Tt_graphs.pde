// Creating graphs for Tt by Udo Kasemets


import oscP5.*;
import netP5.*;

OscP5 oscP5;



int HORIZONTAL = 0, VERTICAL = 1, DIAGONAL = 2, OTHER = 3; // takes 2 bits
int SOLID = 0, DASHED = 1; 

float f, a, c, i, y, z, d, e, w, v, o, p; // main parameters for performance
float new_f, new_a, new_i, new_c;
int bgColor = 240;
float margin = 20;
float side;


void setup()  
{
  size(600, 700);
  side = width-2*margin;
  oscP5 = new OscP5(this, 7770);
  frameRate(25);
  //drawLines();
} 

void draw()
{
  f = lerp(f, new_f, 0.1);
  a = lerp(a, new_a, 0.1);
  c = lerp(c, new_c, 0.1);
  i = lerp(i, new_i, 0.1);
  drawLines();
}

void randomValues()
{
  new_f= random(1); // later will be in 10; 
  new_a = random(1);
  new_c = random(1);
  new_i = random(1);
  println("f: ", f, " a: ", a, " c: ", c, " i: ", i);
}

void drawRelative(float x1, float y1, float x2, float y2, int color_)  // coordinates realtive of the rectangle 0 ..1
  // NB! in relative coordinates 0 -  below, 1 - up
{
  stroke(color_);
  //strokeWeight(2);
  line (margin + x1*side, margin + (1-y1)*side, margin + x2*side, margin + (1-y2)*side);
}

// perhaps textRelative(float x, float y, size, color) ? 

void drawLine(char letter, float value)  // for first 4 lines  - f, a, i, c -  does not make much sense...
{
  float x=0, y=0;
  int color_ = 0;
  int direction = -1, type = -1;
  switch (letter) {
    case ('f'): 
    direction = VERTICAL; 
    type = SOLID; 
    color_ = 0; 
    break;
    case ('a'): 
    direction = HORIZONTAL; 
    type = SOLID; 
    color_ = 0; 
    break;
    case ('c'): 
    direction = VERTICAL;   
    type = DASHED; 
    color_ = 180; 
    break;
    case ('i'): 
    direction = HORIZONTAL;   
    type = DASHED; 
    color_ = 170; 
    break;
  }
  //println("direction, ", direction, " type ", type);
  // TODO: set drawing type - solid // dashed etc perhaps by letter, f et

  // stroke(255); - for color
  stroke(color_);
  strokeWeight(2);
  if (direction == HORIZONTAL ) {
    y = margin + (1-value) * side;
    line(margin, y, margin + side, y);
    textSize(18);
    fill(0);
    textAlign(LEFT, CENTER);
    text (letter, margin + side + 3, y);
  } else if (direction==VERTICAL) {
    x = margin + value * side;
    line(x, margin, x, margin+side);
    textSize(18);
    fill(0);
    textAlign(CENTER, CENTER);
    text (letter, x, margin/2-3);
  }
}

float[] getIntersection(float d1_x1, float d1_y1, float d1_x2, float d1_y2, 
  float d2_x1, float d2_y1, float d2_x2, float d2_y2)  // intersection point of two diagonals given in relative coordinates
{
  float[] results= new float[2]; 
  
  float deltay1 = d1_y2 - d1_y1;
  float deltax1 = d1_x2 - d1_x1;
  float deltay2 = d2_y2 - d2_y1;
  float deltax2 = d2_x2 - d2_x1;
  float k1 = deltay1 / deltax1; // tõus
  float k2 = deltay2 / deltax2;
  
  float y, x;
  // formula for y: y = k1 * (x-d1_x1) + d1_y1;
  // x - result or replacing y into  y = k2 * (x-d2_x1) + d2_y1 and resolving for x:
  x = ( d1_y1 - k1*d1_x1 + k2*d2_x1 - d2_y1)/(k2-k1);  
  y = k1 * (x-d1_x1) + d1_y1;
  //println("X: ", x,  " Y: ", y);  
  //circle(margin +x*side, margin +(1-y)*side, 5);

  results[0] = x; results[1]= y;  
  return results;
    
}

void drawLines()
{
  //randomValues();
  background(bgColor);
  fill(255);
  rect(margin, margin, side, side);

  // main components 
  drawLine('f', f);
  drawLine('a', a);
  drawLine('c', c);
  drawLine('i', i);

  // diagonals 1 ----------------------------------
  strokeWeight(1);
  
  float d1_x1, d1_y1, d1_x2, d1_y2 ; // two points of diagonal
  d1_x1 = f; 
  d1_y1= a; 
  d1_y2 = i; 
  d1_x2 = (f>=c) ? 1 : 0;
  drawRelative(d1_x1, d1_y1, d1_x2, d1_y2, 0);

  float d2_x1, d2_y1, d2_x2, d2_y2 ; // two points of diagonal
  d2_x1 = c; 
  d2_y1= i; 
  d2_y2 = a; 
  d2_x2 = (f>=c) ? 1 : 0;
  drawRelative(d2_x1, d2_y1, d2_x2, d2_y2, 0);

  // perpendiculars y, z ------------------------

  // find intersection of the diagonals (connecting lines) 
  float[] interSection = new float[2];
  interSection = getIntersection(d1_x1, d1_y1, d1_x2, d1_y2,  d2_x1, d2_y1, d2_x2, d2_y2);
  z = interSection[0]; y = interSection[1];
  
  drawRelative(z,y, z,0  ,0);
  textSize(18);
  fill(0);
  textAlign(CENTER, TOP);
  text ("z", margin + side*z, margin + side + 3);
  
  drawRelative(z,y, 0,y  ,0);
  textAlign(RIGHT, CENTER);
  text ("y", margin - 3, margin + (1-y)*side);
  
  
  // diagonals 2 ---------------------------------------
  float helperX = (f>=c) ? 0 : 1;
  drawRelative(c,i, helperX,a, 0  );
  drawRelative(f,a, helperX,i, 0  );
  
  // perpendiculars d, e ------------------------
  
  interSection = getIntersection(c,i, helperX,a,  f,a, helperX,i    );
  d = interSection[0]; e = interSection[1];
  
  drawRelative(d,e, d,1  , 180);
  textSize(18);
  fill(0);
  textAlign(CENTER, BOTTOM);
  text ("d", margin + side*d, margin -3 );
  
  drawRelative(d, e, 1, e, 180 );
  textAlign(LEFT, CENTER);
  text ("e", margin + side  +3, margin + (1-e)*side);
  
  // diagonals 3 ----------------------------
  float helperY = (a>=i) ? 0 : 1;
  drawRelative(c,i, f,helperY, 0  );
  drawRelative(f,a, c, helperY, 0  );
  
  // perpendiculars w,v ----------------------
  interSection = getIntersection(c,i, f,helperY,  f,a, c, helperY    );
  v = interSection[0]; w = interSection[1];
  
  drawRelative(v,w, v,0  , 0);
  textAlign(CENTER, TOP);
  text ("v", margin + side*v, margin + side + 3);
  
  drawRelative(v,w, 0,w  , 0);
  textAlign(RIGHT, CENTER);
  text ("w", margin - 3, margin + (1-w)*side);
  
  // diagonals 4 ---------------------------------------
  
  helperY = (a>=i) ? 1 : 0;
  drawRelative(f,a, c,helperY, 0  );
  drawRelative(c,i, f, helperY, 0  );
  
  // perpendiculars o, p
  
  interSection = getIntersection(f,a, c,helperY,  c,i, f, helperY    );
  p = interSection[0]; o = interSection[1];
  
  drawRelative(p,o, p,1   , 180); // same color / line type as cc and ii (dashed)
  textAlign(CENTER, BOTTOM);
  text ("p", margin + side*p, margin -3 );
  
  drawRelative(p,o, 1, o   , 180);
  textAlign(LEFT, CENTER);
  text ("o", margin + side  +3, margin + (1-o)*side);
  
  // output parameters:
  String parameterText = String.format("f:%.2f a:%.2f c:%.2f i:%.2f y:%.2f z:%.2f d:%.2f e:%.2f w:%.2f v:%.2f o:%.2f p:%.2f",  
  f, a, c, i, y, z, d, e, w, v, o, p ) ;
  float textY = side + 2* margin;
  text(parameterText, margin, textY, side, height-textY );
  
  
}



void keyPressed() 
{
  if (key==32) {  // space
    randomValues();
    drawLines();
  }
}

void oscEvent(OscMessage theOscMessage) {
  String messAddr =  theOscMessage.addrPattern();
  print("OSC message: " + messAddr);
  if (messAddr.equals("/Tt")) {
    new_f = theOscMessage.get(0).floatValue();
    new_a = theOscMessage.get(1).floatValue();
    new_c = theOscMessage.get(2).floatValue();
    new_i = theOscMessage.get(3).floatValue();
  }
}
