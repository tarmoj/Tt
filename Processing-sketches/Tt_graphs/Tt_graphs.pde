// Creating graphs for Tt by Udo Kasemets


int HORIZONTAL = 0, VERTICAL = 1, DIAGONAL = 2, OTHER = 3; // takes 2 bits
int SOLID = 0, DASHED = 1; 

float f, a, c, i;
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
  println("f: ", f, " a: ",a, " c: ",  c, " i: ", i);
}

void drawLine(char letter, float value) 
{
  float x=0, y=0;
  int color_ = 0;
  int direction = -1, type = -1;
  switch (letter) {
    case ('f'): direction = VERTICAL; type = SOLID; color_ = 0; break;
    case ('a'): direction = HORIZONTAL; type = SOLID; color_ = 0; break;
    case ('c'): direction = VERTICAL;   type = DASHED; color_ = 180; break;
    case ('i'): direction = HORIZONTAL;   type = DASHED; color_ = 170; break;
  }
  println("direction, ", direction, " type ", type);
  // TODO: set drawing type - solid // dashed etc perhaps by letter, f et
  
  // stroke(255); - for color
  stroke(color_);
  strokeWeight(2);
  if (direction == HORIZONTAL ) {
    y = margin + value * side;
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

void drawLines()
{
  randomValues();
  background(bgColor);
  fill(255);
  rect(margin,margin,side, side);
  
  // 1 vertical f
  drawLine('f', f);
  drawLine('a', a);
  drawLine('c', c);
  drawLine('i', i);

  
}



void keyPressed() 
{
  if (key==32) {  // space
    drawLines();
  }
}
