// Creating graphs for Tt by Udo Kasemets

float f, a, c, i;
int bgColor = 240;
int margin = 10;
int side;


void setup() 
{
  size(640, 650);
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


void drawLines()
{
  randomValues();
  background(bgColor);
  rect(margin,margin,side, side);
  
  // 1 vertical f
  float x = margin + f*side;
  line (x, 0, x, height); 

  
}



void keyPressed() {
  
}
