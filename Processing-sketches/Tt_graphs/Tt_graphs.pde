// Creating graphs for Tt by Udo Kasemets


int HORIZONTAL = 0, VERTICAL = 1, DIAGONAL = 2, OTHER = 3; // takes 2 bits
int SOLID = 0, DASHED = 1; 

float f, a, c, i, y, z;
int bgColor = 240;
float margin = 20;
float side;


void setup()  
{
  size(400, 420);
  side = width-2*margin;
  drawLines();
} 

void draw()
{
}

void randomValues()
{
  f= random(1); // later will be in 10; 
  a = random(1);
  c = random(1);
  i = random(1);
  println("f: ", f, " a: ", a, " c: ", c, " i: ", i);
}

void drawRelative(float x1, float y1, float x2, float y2, int color_)  // coordinates realtive of the rectangle 0 ..1
  // NB! in relative coordinates 0 -  below, 1 - up
{
  stroke(color_);
  //strokeWeight(2);
  line (margin + x1*side, margin + (1-y1)*side, margin + x2*side, margin + (1-y2)*side);
}
void drawLine(char letter, float value)  // maybe better x1, y1, y1, y2, color)
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
  println("direction, ", direction, " type ", type);
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
  println("X: ", x,  " Y: ", y);
  
  circle(margin +x*side, margin +(1-y)*side, 5);

  results[0] = x; results[1]= y;
  
  return results;
    
}

void drawLines()
{
  randomValues();
  background(bgColor);
  fill(255);
  rect(margin, margin, side, side);

  // main components 
  drawLine('f', f);
  drawLine('a', a);
  drawLine('c', c);
  drawLine('i', i);

  // diagonals 1 ----------------------------------

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
  
}



void keyPressed() 
{
  if (key==32) {  // space
    drawLines();
  }
}
