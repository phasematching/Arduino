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
String units = "steps";  // Sets the units to be output to display to be number of steps taken
int distance;  // Holds the distance traversed by platform
int thousands = 0;  // Holds the thousands place of distance
int hundreds = 0;  // Holds the hundreds place of distance
int tens = 0;  // Holds the tens place of distance
int ones = 0;  // Holds the ones place of distance
int m = 0;  // Holds hexadecimal value of thousands place
int h = 0;  // Holds hexadecimal value of hundreds place
int t = 0;  // Holds hexadecimal value of tens place
int o = 0;  // Holds hexadecimal value of ones place
int hex [] = {0x003F, 0x1200, 0x00DB, 0x00CF, 0x00E6, 0x00ED, 0x00FD, 0x0007, 0x00FF, 0x00E7};  // Array holding the corrisponding hexadecimal value of the index value
int hexDot [] = {0x403F, 0x5200, 0x40DB, 0x40CF, 0x40E6, 0x40ED, 0x40FD, 0x4007, 0x40FF, 0x40E7};  // Array holding the corrisponding hexadecimal value, w/ dot, of the index value

void setup()
{
  Serial.begin(9600);
  alpha4.begin(0x70);  // pass in the address
  pinMode(PWA, OUTPUT);
  pinMode(PWB, OUTPUT);
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  stepper.setSpeed(rpms);  // set the speed of the motor (RPMs)
  alpha4.clear();  // Clear display
  alpha4.writeDisplay();
  Serial.println("Begin motor control");
  Serial.println();
  //Print function list for user selection
  Serial.println("Enter number for control option:");
  Serial.println("1. Set steps, speed and units");
  Serial.println("2. Check steps, speed and units");
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
  Serial.println("Enter the RPMs you wish to move at:");  // Start a loop to wait for user to input something
  entered = false;
  while(entered == false){
    newRpm = Serial.readString();
    if(newRpm != ""){
      entered = true;
    }
  }
  Serial.println("Enter the units you wish to use: (mm, steps)");  // Start a loop to wait for user to input something
  entered = false;
  while (entered == false){
    units = Serial.readString();
    units.toLowerCase();
    if (units == "mm" || units == "steps"){ 
      entered = true;
    }
  }
  steps = newSteps.toInt();  // Sets steps equal to integer value of user input 
  rpms = newRpm.toInt();
  Serial.println();
  Serial.print("Steps:");
  Serial.println(steps);
  Serial.print("RPMs:");
  Serial.println(rpms);
  Serial.print("Units:");
  Serial.println(units);
  Serial.println();
  DisplayChoices();  // Displays the choices for user to pick again
}

void CheckSettings()
{
  Serial.print("Steps:");
  Serial.println(steps);
  Serial.print("Speed:");
  Serial.println(rpms);
  Serial.print("Units:");
  Serial.println(units);
  DisplayChoices();  // Displays the choices for user to pick again
}

void MoveForward()  // Moves forward in a precise but slow way
{
  Serial.println("Moving forward");
  Serial.println();
  digitalWrite(PWA, HIGH);  // Turn on power to run motor
  digitalWrite(PWB, HIGH);
  while(turned < steps){
    stepper.step(1);  // Motor steps one step
    totalSteps++;  // Add on to total steps count
    turned++;
    if (units == "steps"){
      distance = totalSteps;  // Set distance equal to totalSteps to avoid changing totalSteps 
      thousands = (int)floor(distance/1000);  // Gives a single digit that is the thousands spot of the distance value
      distance = distance % 1000;  // Subtracts the thousands spot 
      hundreds = (int)floor(distance/100);
      distance = distance % 100;
      tens = (int)floor(distance / 10);
      distance = distance % 10;
      ones = (int)distance;
      m = hex[thousands];  // Get corrisponding hexadecimal value for the location
      h = hex[hundreds];
      t = hex[tens];
      o = hex[ones];
    }
    if (units == "mm"){
      distance = totalSteps * 5;  // Corrisponds to thread being turned, being used is five micometers per step
      thousands = (int)floor(distance/1000);  // Gives a single digit that is the thousands spot of the distance value
      distance = distance % 1000;  // Subtracts the thousands spot 
      hundreds = (int)floor(distance/100);
      distance = distance % 100;
      tens = (int)floor(distance / 10);
      distance = distance % 10;
      ones = (int)distance;
      m = hexDot[thousands];  // Add a decimal to the beginning of the first digit to give read out in millimeters
      h = hex[hundreds];
      t = hex[tens];
      o = hex[ones];
    }
    alpha4.writeDigitRaw(0, m);  // Set display to value
    alpha4.writeDigitRaw(1, h);
    alpha4.writeDigitRaw(2, t);
    alpha4.writeDigitRaw(3, o);
    alpha4.writeDisplay();
  }
  turned = 0;
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  CheckForRotation();
  Serial.print("Number of rotations: ");
  Serial.println(rotations);
  Serial.print("Total steps taken from origin: ");
  Serial.println(totalSteps);
  Serial.println();
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
      distance = abs(totalSteps); 
      thousands = (int)floor(distance/1000);
      distance = distance % 1000;
      hundreds = (int)floor(distance/100);
      distance = distance % 100;
      tens = (int)floor(distance / 10);
      distance = distance % 10;
      ones = (int)distance;
      m = hex[thousands];
      h = hex[hundreds];
      t = hex[tens];
      o = hex[ones];
    }
    if (units == "mm"){
      distance = abs(totalSteps) * 5;  // Corrisponds to thread being turned, being used is five micometers per step
      thousands = (int)floor(distance/1000);  // Gives a single digit that is the thousands spot of the distance value
      distance = distance % 1000;  // Subtracts the thousands spot 
      hundreds = (int)floor(distance/100);
      distance = distance % 100;
      tens = (int)floor(distance / 10);
      distance = distance % 10;
      ones = (int)distance;
      m = hexDot[thousands];  // Add a decimal to the beginning of the first digit to give read out in millimeters
      h = hex[hundreds];
      t = hex[tens];
      o = hex[ones];
    }
    alpha4.writeDigitRaw(0, m);  // Set display to value
    alpha4.writeDigitRaw(1, h);
    alpha4.writeDigitRaw(2, t);
    alpha4.writeDigitRaw(3, o);
    alpha4.writeDisplay();
  }
  turned = 0;
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  CheckForRotation();
  Serial.print("Number of rotations: ");
  Serial.println(rotations);
  Serial.print("Total steps taken from origin: ");
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
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  Serial.println();
  DisplayChoices();  // Displays the choices for user to pick again
  DisplayChoices();  // Displays the choices for user to pick again
}

void MoveFastB()  // Moves backward less precisely but in a timely manner
{
  digitalWrite(PWA, HIGH);// Turn on power to run motor
  digitalWrite(PWB, HIGH);
  Serial.println("Moving Backward");
  stepper.step(-steps);
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  DisplayChoices();  // Displays the choices for user to pick again
}

void DisplayChoices()
{
  Serial.println("Enter number for control option:");
  Serial.println("1. Set steps, speed and units");
  Serial.println("2. Check steps, speed and units");
  Serial.println("3. Set total steps to 0 (Set origin)");
  Serial.println("4. Move away from motor (Counter clockwise)");
  Serial.println("5. Move toward motor (Clockwise)");
//  Serial.println("6. Move away from motor Quickly");
//  Serial.println("7. Move toward motor Quickly");
  Serial.println();
}

