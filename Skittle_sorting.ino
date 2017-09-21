/*
This uses a 10k resistor in series with ICC photoresistor (about 2-16kohm) 10k is grounded and
photoresistor is on +5v.
you can tie the LED pins of two sensors together and measure two sensors with a total of 3 digital pins
and 2 analog pins.
*/
#include <Servo.h>
#include <EEPROM.h>

#define SENSOR 14
#define RLED 10
#define GLED 11
#define BLED 12
#define SERVO1 3
#define SERVO2 6

const int NSAMPLES = 10;
const int delay_time = 400;

// Ranges below for Skittles on top of black pipe
/*
const int R_PURPLERANGE[] = {240, 255};
const int G_PURPLERANGE[] = {95, 110};
const int B_PURPLERANGE[] = {90, 105};

const int R_GREENRANGE[] = {275, 290};
const int G_GREENRANGE[] = {185, 200};
const int B_GREENRANGE[] = {140, 150};

const int R_REDRANGE[] = {435, 455};
const int G_REDRANGE[] = {130, 148};
const int B_REDRANGE[] = {120, 135};

const int R_ORANGERANGE[] = {530, 545};
const int G_ORANGERANGE[] = {175, 185};
const int B_ORANGERANGE[] = {165, 180};

const int R_YELLOWRANGE[] = {510, 520};
const int G_YELLOWRANGE[] = {245, 260};
const int B_YELLOWRANGE[] = {175, 190};
*/

Servo servo1, servo2;

void setup() {
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);
  //digitalWrite(RLED, HIGH);
  //digitalWrite(GLED, HIGH);
  //digitalWrite(BLED, HIGH);

  Serial.begin(9600);
}

void loop() {
  //debug();
  //calibrateSensor(12);
  method();
  //readMem();
}

// The method to do the Skittle sorting
void method()
{
  int color_int = colordetect();
  turnTable(color_int);
  delay(100);
  pushCandy();
  delay(100);
}

void debug()
{
  int val = analogRead(SENSOR);
  Serial.println(val);
  delay(200);
}

// Calibrate a color to the environment.
// Pass in 0 for red, 3 for orange, 6 for yellow, 9 for green, 12 for purple.
void calibrateSensor(int addr)
{
  digitalWrite(RLED, HIGH);
  delay(1000);
  int val=0;
  for (int i = 0; i < NSAMPLES; i++)
    val += analogRead(SENSOR);
  val /= NSAMPLES;
  Serial.print(val);
  Serial.print("  ");  
  EEPROM.write(addr, val/4);
  Serial.println(EEPROM.read(addr));
  digitalWrite(RLED, LOW);
  
  digitalWrite(GLED, HIGH);
  delay(1000);
  val=0;
  for (int i = 0; i < NSAMPLES; i++)
    val += analogRead(SENSOR);
  val /= NSAMPLES;  
  Serial.print(val);
  Serial.print("  ");
  EEPROM.write(addr+1, val/4);
  Serial.println(EEPROM.read(addr+1));
  digitalWrite(GLED, LOW);
  
  digitalWrite(BLED, HIGH);
  delay(1000);
  val=0;
  for (int i = 0; i < NSAMPLES; i++)
    val += analogRead(SENSOR);
  val /= NSAMPLES;  
  Serial.print(val);
  Serial.print("  ");
  EEPROM.write(addr+2, val/4);
  Serial.println(EEPROM.read(addr+2));
  digitalWrite(BLED, LOW);

  delay(100000);
}

// See stored calibrated values.
void readMem()
{
  delay(3000);
  int val;
  for (int i = 0; i < 15; i++)
  {
    val = EEPROM.read(i);
    Serial.print(i);
    Serial.print(": ");
    Serial.println(val);
  }
  delay(1000000);
}

// Detect color of Skittle
int colordetect()
{
  String color;
  
  digitalWrite(RLED, HIGH);
  delay(delay_time);
  int rval = 0;
  for (int i = 0; i < NSAMPLES; i++)
    rval += analogRead(SENSOR);
  rval /= NSAMPLES;
  digitalWrite(RLED, LOW);
  //delay(delay_time);

  digitalWrite(GLED, HIGH);
  delay(delay_time);
  int gval = 0;
  for (int i = 0; i < NSAMPLES; i++)
    gval += analogRead(SENSOR);
  gval /= NSAMPLES;
  digitalWrite(GLED, LOW);
  //delay(delay_time);

  digitalWrite(BLED, HIGH);
  delay(delay_time);
  int bval = 0;
  for (int i = 0; i < NSAMPLES; i++)
    bval += analogRead(SENSOR);
  bval /= NSAMPLES;
  digitalWrite(BLED, LOW);
  //delay(delay_time);

  /*
  double red_error = (error(R_REDRANGE, rval) + error(G_REDRANGE, gval) + error(B_REDRANGE, bval)) / 3;
  double orange_error = (error(R_ORANGERANGE, rval) + error(G_ORANGERANGE, gval) + error(B_ORANGERANGE, bval)) / 3;
  double yellow_error = (error(R_YELLOWRANGE, rval) + error(G_YELLOWRANGE, gval) + error(B_YELLOWRANGE, bval)) / 3;
  double green_error = (error(R_GREENRANGE, rval) + error(G_GREENRANGE, gval) + error(B_GREENRANGE, bval)) / 3;
  double purple_error = (error(R_PURPLERANGE, rval) + error(G_PURPLERANGE, gval) + error(B_PURPLERANGE, bval)) / 3;
  */

  // Calculate errors
  double red_error = (error(rval, 0) + error(gval, 1) + error(bval, 2));
  double orange_error = (error(rval, 3) + error(gval, 4) + error(bval, 5));
  double yellow_error = (error(rval, 6) + error(gval, 7) + error(bval, 8));
  double green_error = (error(rval, 9) + error(gval, 10) + error(bval, 11));
  double purple_error = (error(rval, 12) + error(gval, 13) + error(bval, 14));

  double color_errors[] = {red_error, orange_error, yellow_error, green_error, purple_error};
  for (int j = 0; j < 5; j++)
  {
    Serial.print(color_errors[j]);
    Serial.print("  ");
  }
  int index = getMinIndex(color_errors, 5); // Get color with minimum error

  switch (index)
  {
    case 0:
      color = "red";
      break;
    case 1:
      color = "orange";
      break;
    case 2:
      color = "yellow";
      break;
    case 3:
      color = "green";
      break;
    case 4:
      color = "purple";
      break;
    default:
      color = "nothing";
      break;
  }
  Serial.println(color);
  return index;
}

/*
// Range is a tuple containing the lower and upper bounds
double error(const int range[], const int val)
{
  if (val >= range[0] && val <= range[1])
    return 0.0;
  double error1 = abs((val - (double)range[0]) / range[0]);
  double error2 = abs((val - (double)range[1]) / range[1]);
  return min(error1, error2);
}
*/

// Similarity matching
double error(const int val, int address)
{
  int expected = 4*EEPROM.read(address);
  return abs((val-(double)expected)/expected);
}

int getMinIndex(const double a[], const int asize)
{
  if (asize <= 0)
    return -1;
  double minval = a[0];
  int index = 0;
  for (int i = 1; i < asize; i++)
  {
    if (a[i] < minval)
    {
      minval = a[i];
      index = i;      
    }
  }
  return index;
}

// Servo 1 pushes candy down ramp
void pushCandy()
{
  servo1.attach(SERVO1);
  int pos;
  for(pos = 60; pos >= 0; pos-=1)
  {
    servo1.write(pos);
    delay(10);
  }
  for (pos = 0; pos <= 60;  pos+=1)
  {
    servo1.write(pos);
    delay(10);
  }
  servo1.detach();
}

// Servo 2 turns turntable to correct section based on color detected
void turnTable(int color_int)
{  
  servo2.attach(SERVO2);
  delay(1000);
  servo2.write(45*color_int);
  delay(1000);
  servo2.detach();
}
