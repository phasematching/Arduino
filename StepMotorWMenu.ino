#include <Stepper.h>
#include <math.h>

#define STEPS 200  // This is the number of steps for one rotation, this is dependant on stepper motor being used
#define PWA 3  // This is the digital pin that power for A motor is connected to, these are selected on the internal connections of project box DO NOT CHANGE
#define PWB 8  // This is the digital pin that power for B motor is connected to, these are selected on the internal connections of project box DO NOT CHANGE

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 4, 5, 6, 7);  // These pins have been internally selected inside of project box being used, do not change

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


void setup()
{
  Serial.begin(9600);
  pinMode(PWA, OUTPUT);
  pinMode(PWB, OUTPUT);
  digitalWrite(PWA, LOW);  // Set power pins to LOW (off) to keep motor from over heating
  digitalWrite(PWB, LOW);
  stepper.setSpeed(rpms);  // set the speed of the motor (RPMs)
  Serial.println("Begin motor control");
  Serial.println();
  //Print function list for user selection
  Serial.println("Enter number for control option:");
  Serial.println("1. Set steps and speed");
  Serial.println("2. Check steps and speed");
  Serial.println("3. Set total steps to 0 (Set origin)");
  Serial.println("4. Move away from motor (Counter clockwise)");
  Serial.println("5. Move tpward motor (Clockwise)");
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
//  Serial.println("Enter the RPMs you wish to move at:");
//  entered = false;
//  while(entered == false){
//    newRpm = Serial.readString();
//    if(newRpm != ""){
//      entered = true;
//    }
//  }
  steps = newSteps.toInt();  // Sets steps equal to integer value of user input 
//  rpms = newRpm.toInt();
  Serial.print("Steps:");
  Serial.println(steps);
//  Serial.print("RPMs:");
//  Serial.println(rpms);
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
    if(turned%100 == 0)
    {
      Serial.println(turned);
    }
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

