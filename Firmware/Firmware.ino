
// Include Libraries
#include "Arduino.h"
#include "Buzzer.h"
#include "DCMDriverL298.h"
#include "NewPing.h"
#include "Button.h"


// Pin Definitions & Wire Colours
#define BUZZER_PIN_SIG	2 //Red
#define DCMOTORDRIVERL298_PIN_INT1	3 //Yellow
#define DCMOTORDRIVERL298_PIN_ENB	6 //Red
#define DCMOTORDRIVERL298_PIN_INT2	4 //Green
#define DCMOTORDRIVERL298_PIN_ENA	5 //Brown
#define DCMOTORDRIVERL298_PIN_INT3	7 //Blue
#define DCMOTORDRIVERL298_PIN_INT4	8 //Purple
#define HCSR04_PIN_TRIG	10 //Black
#define HCSR04_PIN_ECHO	9 //Grey
#define PUSHBUTTON_1_PIN_2	12 // Red
#define PUSHBUTTON_2_PIN_2	13 //Red
#define PUSHBUTTON_3_PIN_2	A0 //Red
#define TCS230_PIN_S1	A1 //Brown
#define TCS230_PIN_S0	A2 //Blue
#define TCS230_PIN_S3	A3 //Green
#define TCS230_PIN_S2	A4 //Yellow
#define TCS230_PIN_OUT	A5 //Orange



// object initialization
Buzzer buzzer(BUZZER_PIN_SIG);
DCMDriverL298 dcMotorDriverL298(DCMOTORDRIVERL298_PIN_ENA,DCMOTORDRIVERL298_PIN_INT1,DCMOTORDRIVERL298_PIN_INT2,DCMOTORDRIVERL298_PIN_ENB,DCMOTORDRIVERL298_PIN_INT3,DCMOTORDRIVERL298_PIN_INT4);
NewPing hcsr04(HCSR04_PIN_TRIG,HCSR04_PIN_ECHO);
Button pushButton_1(PUSHBUTTON_1_PIN_2);
Button pushButton_2(PUSHBUTTON_2_PIN_2);
Button pushButton_3(PUSHBUTTON_3_PIN_2);

// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// Stores the red. green and blue colors
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

//Outlines motor speed as a variable since it will change
int dcmotorspeed = 200;   // MAX 255
int turn_speed = 230;    // MAX 255 
int turn_delay = 10;

//Declares the colour detecting variables
int colourDetected = 0;
int colourToDetect = 0;
int detectionStatus = 0;

//Declares the number of buttons pressed
int buttonPressCounter = 0;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() 
{

  pinMode(BUZZER_PIN_SIG, OUTPUT); 

  pushButton_1.init();
  pushButton_2.init();
  pushButton_3.init();

  pinMode(TCS230_PIN_S0, OUTPUT);
  pinMode(TCS230_PIN_S1, OUTPUT);
  pinMode(TCS230_PIN_S2, OUTPUT);
  pinMode(TCS230_PIN_S3, OUTPUT);
  
  // Setting the sensorOut as an input
  pinMode(TCS230_PIN_OUT, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(TCS230_PIN_S0,HIGH);
  digitalWrite(TCS230_PIN_S1,LOW);
  
  // Begins serial communication
  Serial.begin(9600);
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() 
{

  //Declare Ultrasonic distance variable constantly
  int hcsr04Dist = hcsr04.ping_cm();
  //delay(10);
  //Serial.print(F("Distance: ")); Serial.print(hcsr04Dist); Serial.println(F("[cm]"));

  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(TCS230_PIN_S2,LOW);
  digitalWrite(TCS230_PIN_S3,LOW);
  
  // Reading the output frequency
  redFrequency = pulseIn(TCS230_PIN_OUT, LOW);
  // Remaping the value of the RED (R) frequency from 0 to 255
  // You must replace with your own values. Here's an example: 
  // redColor = map(redFrequency, 70, 120, 255,0);
  redColor = map(redFrequency, 26, 136, 255,0);
  
  // Printing the RED (R) value
  //Serial.print("R = ");
  //Serial.print(redColor);
  delay(100);
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(TCS230_PIN_S2,HIGH);
  digitalWrite(TCS230_PIN_S3,HIGH);
  
  // Reading the output frequency
  greenFrequency = pulseIn(TCS230_PIN_OUT, LOW);
  // Remaping the value of the GREEN (G) frequency from 0 to 255
  // You must replace with your own values. Here's an example: 
  // greenColor = map(greenFrequency, 100, 199, 255, 0);
  greenColor = map(greenFrequency, 46, 108, 255, 0);
  
  // Printing the GREEN (G) value  
  //Serial.print(" G = ");
  //Serial.print(greenColor);
  delay(100);
 
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(TCS230_PIN_S2,LOW);
  digitalWrite(TCS230_PIN_S3,HIGH);
  
  // Reading the output frequency
  blueFrequency = pulseIn(TCS230_PIN_OUT, LOW);
  // Remaping the value of the BLUE (B) frequency from 0 to 255
  // You must replace with your own values. Here's an example: 
  // blueColor = map(blueFrequency, 38, 84, 255, 0);
  blueColor = map(blueFrequency, 62, 156, 255, 0);
  
  // Printing the BLUE (B) value 
  //Serial.print(" B = ");
  //Serial.print(blueColor);
  delay(100);

  // Checks the current detected color and prints
  // a message in the serial monitor
  if(redColor > greenColor && redColor > blueColor){
    Serial.println(" - RED detected!");
    colourDetected = 2;
  }
  if(greenColor > redColor && greenColor > blueColor){
    Serial.println(" - GREEN detected!");
    colourDetected = 1;
  }
  if(blueColor > redColor && blueColor > greenColor && blueColor < 40){
    Serial.println(" - BLUE detected!");
    colourDetected = 3;
  }
  //Code Logic added to detect the colour white
  if(blueColor > greenColor && blueColor > redColor && redColor > 1 && greenColor > 1){
    Serial.println(" - WHITE detected!");
    colourDetected = 4;
  }

  //Ulstrasonic Sensor to update speed variable based on distance (cm)
  if (hcsr04Dist <= 20) {
    dcmotorspeed = 150;
  }
  if (hcsr04Dist <= 15) {
    dcmotorspeed = 100;
  }
  if (hcsr04Dist <= 10) {
    dcmotorspeed = 50;
  }
  if (hcsr04Dist >1 && hcsr04Dist <= 5) {
    dcmotorspeed = 0;
  }
  if (hcsr04Dist > 20 || hcsr04Dist < 1) {
    dcmotorspeed = 200;
  }

  //Serial.print("Speed: "); Serial.print(dcmotorspeed); Serial.print(" "); Serial.print(buttonPressCounter);
  //Serial.print(colourDetected);
  //delay(10);
  //Button press determines colour to follow
  //Repressing the sma ebutton will stop the system
  if (pushButton_1.onPress() && buttonPressCounter == 0) {
    detectionStatus = 1;
    buttonPressCounter = 1;
    colourToDetect = 'G';
    bool pushButton_1Val = pushButton_1.read();
    //Serial.print(F("Val: ")); Serial.println(pushButton_1Val);
    dcMotorDriverL298.setMotorA(dcmotorspeed, 0);
    dcMotorDriverL298.setMotorB(dcmotorspeed, 1);
    }
  else if (pushButton_1.onPress() && buttonPressCounter == 1) {
    detectionStatus = 'F';
    buttonPressCounter = 0;
    dcMotorDriverL298.stopMotors();
  }

  if (pushButton_2.onPress() && buttonPressCounter == 0) {
    detectionStatus = 1;
    buttonPressCounter = 1;
    colourToDetect = 2;
    bool pushButton_2Val = pushButton_2.read();
    //Serial.print(F("Val: ")); Serial.println(pushButton_2Val);
  }
  else if (pushButton_2.onPress() && buttonPressCounter == 1) {
    //detectionStatus = 'F';
    buttonPressCounter = 0;
    //dcmotorspeed = 0;
    //dcMotorDriverL298.stopMotors();
  }

  if (pushButton_3.onPress() && buttonPressCounter == 0) {
    detectionStatus = 'T';
    buttonPressCounter = 1;
    colourToDetect = 'B';
    bool pushButton_3Val = pushButton_3.read();
    //Serial.print(F("Val: ")); Serial.println(pushButton_3Val);
    dcMotorDriverL298.setMotorA(dcmotorspeed, 0);
    dcMotorDriverL298.setMotorB(dcmotorspeed, 1);
  }
  else if (pushButton_3.onPress() && buttonPressCounter == 1) {
    detectionStatus = 'F';
    buttonPressCounter = 0;
    dcmotorspeed = 0;
    dcMotorDriverL298.stopMotors();
  }

  if (detectionStatus == 1) {
    while (colourDetected == colourToDetect) {
    //move forward
      //Serial.print("going forward"); 
      //delay(100);
      //Serial.println(colourDetected); Serial.print(" "); Serial.println(colourToDetect);
      dcMotorDriverL298.setMotorA(dcmotorspeed, 0);
      dcMotorDriverL298.setMotorB(dcmotorspeed, 1);
      //digitalWrite (DCMOTORDRIVERL298_PIN_INT2,LOW);
      //digitalWrite(DCMOTORDRIVERL298_PIN_INT1,HIGH);                       
      //digitalWrite (DCMOTORDRIVERL298_PIN_INT4,HIGH);
      //digitalWrite(DCMOTORDRIVERL298_PIN_INT3,LOW);

      //analogWrite (DCMOTORDRIVERL298_PIN_ENA, dcmotorspeed);
      //analogWrite (DCMOTORDRIVERL298_PIN_ENB, dcmotorspeed);

      delay(turn_delay);
      if (colourDetected != colourToDetect) {
        break;
      }
      
    }
    while (colourDetected != colourToDetect) {
      //turn left
      //Serial.println("turning left");
      dcMotorDriverL298.setMotorA(dcmotorspeed, 1);
      dcMotorDriverL298.setMotorB(dcmotorspeed, 1);
  
      //digitalWrite (motorA1,HIGH);
      //digitalWrite(motorA2,LOW);                       
      //digitalWrite (motorB1,HIGH);
      //digitalWrite(motorB2,LOW);

      //analogWrite (DCMOTORDRIVERL298_PIN_ENA, turn_speed);
      //analogWrite (DCMOTORDRIVERL298_PIN_ENB, dcmotorspeed);

      //delay(turn_delay);
      
      if (colourDetected == colourToDetect) {
        break;
      }

      //turn right
      //Serial.println("turning left");
      //dcMotorDriverL298.setMotorA(dcmotorspeed, 0);
      //dcMotorDriverL298.setMotorB(dcmotorspeed, 0);
  
      //digitalWrite (motorA1,HIGH);
      //digitalWrite(motorA2,LOW);                       
      //digitalWrite (motorB1,HIGH);
      //digitalWrite(motorB2,LOW);

      //analogWrite (DCMOTORDRIVERL298_PIN_ENA, turn_speed);
      //analogWrite (DCMOTORDRIVERL298_PIN_ENB, dcmotorspeed);

      //delay(turn_delay);
    }
  }

  //Buzzer will sound when it is not looking at the colour it is set to follow
  if (colourDetected == 4) {
      buzzer.on();       // 1. turns on
      tone(BUZZER_PIN_SIG, 1000); // Send 1KHz sound signal
  }
  else if (colourDetected == 1 || colourDetected == 2 || colourDetected == 3) {
      noTone(BUZZER_PIN_SIG);     // Stop sound
      buzzer.off();      // 2. turns off.
  }

}