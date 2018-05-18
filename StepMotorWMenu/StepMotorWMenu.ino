#include <Stepper.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define STEPS 200  // This is the number of steps for one rotation, this is dependant on stepper motor being used
#define PWA 3  // This is the digital pin that power for A motor is connected to, these are selected on the internal connections of project box DO NOT CHANGE
#define PWB 8  // This is the digital pin that power for B motor is connected to, these are selected on the internal connections of project box DO NOT CHANGE

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 4, 5, 6, 7);  // These pins have been internally selected inside of project box being used, do not change

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();


long int totalSteps = 0;  // Holds how many turns the motor has turned away from 0 since beginning (- for Clockwise steps)
int steps = 200;  // The number of steps the motor will turn
int stepsTurned = 0;  // Holds stepsTurned in current rotation
int rotations = 0;  // Holds the number of rotations completed
int lastRotation = 0; // Holds value for which last full rotation was completed
int pos;  // The position of platform relative to zero position
int rpms = 1;  // Rotations per minute that the motor will move at
char user_input;  // Holds input
int turned = 0;  // Holds the number of steps turned so far in an execution
String newSteps;  // Used to change the number of steps while program is running
bool entered;  // Used to tell if the user has entered something
String newRpm;  // Used to change rpms while the program is running
char units = "steps";
int distance;
char h;
char t;
char o;
int hex [] = {0x003F, 0x1200, 0x00DB, 0x00CF, 0x00E6, 0x00ED, 0x00FD, 0x0007, 0x00FF, 0x00E7};

void setup()
{
  Serial.begin(9600);
  alpha4.begin(0x70);  // pass in the address
  pinMode(PWA, OUTPUT);
  pinMode(PWB, OUTPUT);
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  stepper.setSpeed(rpms);  // set the speed of the motor (RPMs)
  alpha4.writeDigitRaw(0, 0x0);
  alpha4.writeDigitRaw(1, 0x0);
  alpha4.writeDigitRaw(2, 0x0);
  alpha4.writeDigitRaw(3, 0x0);
  alpha4.writeDisplay();
  Serial.println("Begin motor control");
  Serial.println();
  //Print function list for user selection
  Serial.println("Enter number for control option:");
  Serial.println("1. Set steps and speed");
  Serial.println("2. Check steps and speed");
  Serial.println("3. Set total steps to 0 (Set origin)");
  Serial.println("4. Move away from motor (Counter clockwise)");
  Serial.println("5. Move toward motor (Clockwise)");
//  Serial.println("6. Move away from motor Quickly");
//  Serial.println("7. Move toward motor Quickly");
  Serial.println();
}

void loop()
{
  while(Serial.available()){
    user_input = Serial.read(); //Read user input and trigger appropriate function
      if (user_input =='1')
      {
         Settings();
      }

      if (user_input =='2')
      {
        CheckSettings();
      }

      if (user_input =='3')
      {
        SetTotalZero();
      }

      if (user_input =='4')
      {
        MoveForward();
      }

      if (user_input =='5')
      {
        MoveBackward(); 
      }
      if (user_input =='6')
      {
        MoveFastF();
      }

      if (user_input =='7')
      {
        MoveFastB();
      }
  }
}

void Settings()
{
  Serial.println("Enter the number of steps you wish to move:");
  entered = false;
  while(entered == false){  // Start a loop to wait for user to input something
    newSteps = Serial.readString();  // Get user input
    if(newSteps != ""){
      entered = true;
    }
  }
  Serial.println("Enter the RPMs you wish to move at:");
  entered = false;
  while(entered == false){
    newRpm = Serial.readString();
    if(newRpm != ""){
      entered = true;
    }
  }
  Serial.println("Enter the units you wish to use: (mm, steps)"
  entered = false;
  while (entered == false){
    units == Serial.readString();
    if (units == "mm" || units == "steps"){
      entered == true;
    }
  }
  steps = newSteps.toInt();  // Sets steps equal to integer value of user input 
  rpms = newRpm.toInt();
  Serial.print("Steps:");
  Serial.println(steps);
  Serial.print("RPMs:");
  Serial.println(rpms);
  Serial.println();
  DisplayChoices();  // Displays the choices for user to pick again
}

void CheckSettings()
{
  Serial.print("Steps:");
  Serial.println(steps);
  Serial.print("Speed:");
  Serial.println(rpms);
  DisplayChoices();  // Displays the choices for user to pick again
}

void MoveForward()  // Moves forward in a precise but slow way
{
  Serial.println("Moving forward");
  digitalWrite(PWA, HIGH);  // Turn on power to run motor
  digitalWrite(PWB, HIGH);
  while(turned < steps){
    stepper.step(1);  // Motor steps one step
    totalSteps++;
    turned++;
    distance = totalSteps;
    int hundreds = (int)floor(distance/100);
    distance = distance % 100;
    int tens = (int)floor(distance / 10);
    distance = distance % 10;
    int ones = (int)distance;
    int h = hex[hundreds];
    int t = hex[tens];
    int o = hex[ones];
    alpha4.writeDigitRaw(0, h);
    alpha4.writeDigitRaw(1, t);
    alpha4.writeDigitRaw(2, o);
    alpha4.writeDigitAscii(3, 'm');
    alpha4.writeDisplay();
  }
  turned = 0;
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  CheckForRotation();
  Serial.print("Number of rotations:");
  Serial.println(rotations);
  Serial.print("Total steps taken from origin:");
  Serial.println(totalSteps);
  DisplayChoices();  // Displays the choices for user to pick again
}

void MoveBackward()  // Moves backward in a precise but slow way
{
  Serial.println("Moving backward");
  digitalWrite(PWA, HIGH);  // Turn on power to run motor
  digitalWrite(PWB, HIGH);
  while(turned < steps){
    stepper.step(-1);
    totalSteps -= 1;
    turned++;
    if (units == "steps"){
      distance = totalSteps; 
      int hundreds = (int)floor(distance/100);
      distance = distance % 100;
      int tens = (int)floor(distance / 10);
      distance = distance % 10;
      int ones = (int)distance; 
    }
    if (units == "mm"){
      distance == steps * 0.005;
      int hundreds = (int)floor(distance/0.1);
      distance = distance % 0.1;
      int tens = (int)floor(distance / 0.01);
      distance = distance % 0.001;
      int ones = (int)distance;
    }
    int h = hex[hundreds];
    int t = hex[tens];
    int o = hex[ones];
    alpha4.writeDigitRaw(0, h);
    alpha4.writeDigitRaw(1, t);
    alpha4.writeDigitRaw(2, o);
    alpha4.writeDigitAscii(3, 'm');
    alpha4.writeDisplay();
  }
  turned = 0;
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  CheckForRotation();
  Serial.println(rotations);
  Serial.println(totalSteps);
  DisplayChoices();  // Displays the choices for user to pick again
}

void CheckForRotation()
{
  rotations = floor(totalSteps/200);  // Gets the number of rotations turned from zero
  stepsTurned = totalSteps % 200;
}

void SetTotalZero()
{
  totalSteps = 0;
  DisplayChoices();  // Displays the choices for user to pick again
}

void MoveFastF()  // Moves forward less precisely but in a timely manner
{
  digitalWrite(PWA, HIGH);  // Turn on power to run motor
  digitalWrite(PWB, HIGH);
  Serial.println("Moving forward");
  stepper.step(steps);
  Serial.println();
  DisplayChoices();  // Displays the choices for user to pick again
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  DisplayChoices();  // Displays the choices for user to pick again
}

void MoveFastB()  // Moves backward less precisely but in a timely manner
{
  digitalWrite(PWA, HIGH);// Turn on power to run motor
  digitalWrite(PWB, HIGH);
  Serial.println("Moving Backward");
  stepper.step(-steps);
  DisplayChoices();  // Displays the choices for user to pick again
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  DisplayChoices();  // Displays the choices for user to pick again
}

void DisplayChoices()
{
  Serial.println("Enter number for control option:");
  Serial.println("1. Set steps and speed");
  Serial.println("2. Check steps and speed");
  Serial.println("3. Set total steps to 0 (Set origin)");
  Serial.println("4. Move away from motor (Counter clockwise)");
  Serial.println("5. Move toward motor (Clockwise)");
//  Serial.println("6. Move away from motor Quickly");
//  Serial.println("7. Move toward motor Quickly");
  Serial.println();
}

