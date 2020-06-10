//Libraries
#include <math.h>  
#include <ShiftRegister74HC595.h>
#include <HCSR04.h> 

//Definitions 
//Display Defns
#define LOAD 5
#define SCLK 6
#define SDI  7 
//US1 Defns
#define TRIG1 9 
#define ECHO1 10 
//US2 Defns
#define TRIG2 12
#define ECHO2 11

HCSR04 hc(12,11); 
HCSR04 hc2(9, 10); 


//Variable Definitions
int analogPin = A0; 
int val = 0;  // variable to store the value read
int analogVal = 0; 

int avgSum = 0;
int binVal = 0; 
int isNegative = 0; 
float scaledX = 0.0;

float duration1 = 0.0; 
float duration2 = 0.0; 
float distance1 = 0.0; 
float distance2 = 0.0; 

//Create new Shift Register Object 
ShiftRegister74HC595 sr (2, SDI, SCLK, LOAD); 

//Digit Definition 
int value, digit1, digit2; 
//Array of values based on what digit from a to g + DP needs to turn on 
//Active Low 
uint8_t digits[] = {
    B11000000, //0
    B11111001, //1 
    B10100100, //2
    B10110000, //3 
    B10011001, //4
    B10010010, //5
    B10000010, //6 
    B11111000, //7
    B10000000, //8
    B10010000, //9
    B10001000, //A
    B10000011, //b 
    B11000110, //C
    

  }; 

//setup serial
void setup() {
  Serial.begin(38400);   
  pinMode(TRIG1, OUTPUT); 
  pinMode(ECHO1, INPUT);   
  pinMode(TRIG2, OUTPUT); 
  pinMode(ECHO2, INPUT); 
       
}

//Loop Setup 
void loop() {
  analogVal = analogRead(analogPin);  // read the input pin

  //
  for(int i = 0; i < 10; i++) {
    avgSum += analogVal;    
  }

  val = avgSum/10; 
  avgSum = 0; 
 
  
  if(val > 343 || val == 343) {
      scaledX = 1.139240506 * val - 403.2911392; 
    } 
  else {
      scaledX = 1.285714286 * val - 455.1428571; 
    }   
  
  //Negative Flag
  if(scaledX < 0) {
    isNegative = 1; 
    analogWrite(A3, 255); 
  } else {
    analogWrite(A3, 0); 
    isNegative = 0; 
  }


  //Ultrasonic distance detection 

  //Send the 40HZ pulse & turn it off after 10us
  digitalWrite(TRIG1, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TRIG1, LOW); 
  //Time of return
  duration1 = pulseIn(ECHO1, HIGH); //In microseconds
  distance1 = duration1*(10e-5)*(330/2); 
  
  if (distance1 > 400) {
     distance1 = 0;   
    }

  //US #2 
  digitalWrite(TRIG2, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TRIG2, LOW); 
  //Time of return
  duration2 = pulseIn(ECHO2, HIGH); 
  distance2 = duration2*(10e-5)*(330/2); 

  if (distance2 > 400) {
      distance2 = 0; 
    }

  //Distance difference
  int delta = (int)abs(distance1 - distance2); 

  //uint8_t printNum[] = {digits[10], digits[10]}; 
  int tensPlace = (int)delta/10;
  int onesPlace = (int)delta%10; 
  uint8_t printNum[] = {digits[onesPlace], digits[tensPlace]}; 
  sr.setAll(printNum); 

  
    
  //BCD Conversion 
  binVal = abs((((int)scaledX/10 << 4) | ((int)scaledX % 10)));   
  //Serial.println((int)scaledX); 
  Serial.println("**********");
  Serial.println(distance1); 
  Serial.println(distance2); 
  Serial.println(delta); 
  Serial.println(abs(hc.dist() - hc2.dist())); 
  delay(300);    
  
}
