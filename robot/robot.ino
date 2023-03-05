/* Bluetooth Interfacing with Arduino to drive a rover.


This code allows users on both android and windows divices to controll their robots. Despite using different apps, both windows and android
users can use the same functions and achieve the same functionality when driving and controlling their robots.

Android:

Bellow is linked the app that you must use to controll yur robot.
https://play.google.com/store/apps/details?id=uncia.robotics.joystick&hl=en_SG&gl=US
This app gives students the ability to use a joystick to control their robot aswel as giving them 4 buttons to use for other actuations.
Bellow you can see the output from this app to get a better idea of how it is parsed (although you wont have to do this yourself if you
dont want to):

joystick values: abcdefg

abc are from 0-360 and represent the angle in cartesian coordinates.
def are from 0-100 and represent how far the stick is from the center position.
g is the push button with 0 indicating no button pressed and int between 1 and 4 indicating that buttons 1-4 are pressed.




Windows:

Bellow is linked the app that you must use to controll your robot.
https://apps.microsoft.com/store/detail/universal-bluetooth-pad/9NBLGGH2PL8B
It has the same functionallity as the other app only includes an additional slider which can be usefull for servo controll.

A quirk of the Windows app include that the channel wont be sent untill either the joystick, or slider have been moved. It is
also important to select both the "Text" and "Channel" boxes (found on the right hand side) before data can be sent.




***IMPORTANT NOTES***: When using the Android app the GetBTCommand() function must be given '#' as an input and when using the Windows app
you must give it '/n' as its input. You must also unplug the the bluetooth modules TX and RX wires from the arduino when uploading code to
since it wont upload with the bluetooth module plugged in (power and ground hoever can stay connected).

*/

//_________________________________________________________________________Your Input Goes Below_______________________________________________________________________________________________________________
// Here the pin values used for motor controll are established. The current pin values are valid and will work as is if you dont want to change them.
// In some casses your rover will not move as expected. This might be because the motor outputs are reversed in hardware.

#include <Servo.h>

Servo scooper;

int DIR_L_F = 8; // Left Direction pin that will indicate forewards movement (1 for forewards, 0 for backwards).
int DIR_L_B = 12; // Left Direction pin that will indicate backwards movement (1 for backwards, 0 for forewards).
int PWM_L = 6;   // Speed controll pin. *** This pin must be plugged into an output pin on the arduino that is labled PWM ***.

int DIR_R_F = 4; // Right Direction pin that will indicate forewards movement (1 for forewards, 0 for backwards)
int DIR_R_B = 7; // Left Direction pin that will indicate backwards movement (1 for backwards, 0 for forewards).
int PWM_R = 5;   // Speed controll pin. *** This pin must be plugged into an output pin on the arduino that is labled PWM ***.

int IR_L = A0; // Left IR sensor pin.
int IR_R = A1; // Right IR sensor pin.

// Here you can also declaire your own variables and functions:

//__________________________________________________________________________Your Input Goes Above__________________________________________________________________________________________________________________

void GetBTCommand();                 // Outputs a list containing the raw controller inputs. This function is given a list pointer, and a character teling it which app is being used.
void UnicaTranslateCommand();        // Gets used by GetBTCommand() when '#' is sent as an input. This function decodes the string message and fills the "TranslatedCommand" list.
void BluetoothPadTranslateCommand(); // Gets used by GetBTCommand() when '\n' is sent as an input. This function decodes the string message and fills the "TranslatedCommand" list.
void SimpleMapInput();               // A pre write function That takes in TransatedCommand as an imput. It uses the Angle and Power inputs (first 2 indexes of the input list) and output both motor speed and direction information for the left and right motors.
void ExecuteCommand_L298N();         // Takes in a 4 element list (Left Direction, Right Direction, Left Speed, Right Speed) and applies the correct outputs to the 6 pins used to drive the motors. (these pins are determined by you at the begining of the code)

float ControllerInput[8]; // Angle, speed, button 1-4 (Android)   or    Angle, speed, slider value, channel 0-3 (Windows)
float MotorOutputs[4];    // Left Direction, Right Direction, Left Speed, Right Speed.

// The setup function is were variables can be given initial values (although you can do that when declaring the variable too) and where pins are given their desired type.
// This function only runs once at the begining and can should be where you declare what pins will be used for things like servos and sensors.
void setup()
{
    Serial.begin(9600); // Initializing the Serial Port to Baud rate 9600. You can change this to be faster if you wish.

    // Setting up pins:
    pinMode(DIR_L_F, OUTPUT);
    pinMode(DIR_L_B, OUTPUT);
    pinMode(PWM_L, OUTPUT);

    pinMode(DIR_R_F, OUTPUT);
    pinMode(DIR_R_B, OUTPUT);
    pinMode(PWM_R, OUTPUT);

    pinMode(IR_L, INPUT);
    pinMode(IR_R, INPUT);

    scooper.attach(3);
}

//________________________________________________________________________YOUR CODE GOES BELOW_____________________________________________________________________________________________________________

void loop()
{
    //auton();
    GetBTCommand('\n', ControllerInput); // '\n' for Windows and '#' for android
    SimpleMapInput(MotorOutputs, ControllerInput);
    ExecuteCommand_L298N(MotorOutputs);
}

//________________________________________________________________________YOUR CODE GOES ABOVE____________________________________________________________________________________________________________
/*Below you are the pre writen functions given to you that when used together allow you to controll your rover.


Using the variables "ControllerInput" and either the '\n' (Windows) or the '#' (Android) characters,
GetBTCommand() should be called first to get the raw controller inputs. The 2 conficurations you will use are:

GetBTCommand('#', ControllerInput); (Andorid)
GetBTCommand('\n', ControllerInput); (Windows)

This fuction bassically fills the "ControllerImput" list. This list has 8 indexes split up in the followig way:

Windows: [Joystick Angle, Joystick Distcance from Center, Slider Value, Channel 0, Channel 1, Channel 2, Channel 3]
Android: [Joystick Angle, Joystick Distcance from Center, Button 1, Button 2, Button 3, Button 4, No Functionallity]


This list is then used to decide the motor speeds and directions. You have the option of using the pre writen function
SimpleMapInput() which takes in the previously filled list "ControllerInput", and the return list "MotorOutput". The
function uses the first 2 elements of "ControllerInput" and maps them to directio and PWM values for the left and right
motors. these 4 values are placed in the "MotorOutput" list. An implimentation of this function and a breakdown of the
output can be seen below:

SimpleMapInput(MotorOutputs, ControllerInput);

MotorOutputs = [Left Motor Direction, Right Motor Direction, Left Motor Speed, Right Motor Speed]

If you want the extra challange or want slightly different behaviours, you can write your own version of this function
but make sure all your hardware is working correctly before hand to avoid complex debugging.


Once your output has been calculated it is time to apply these states to the pins. This is done by the ExecuteCommand_L298N()
function which takes in the "MotorOutputs" list. using the speed and direction values stored in the imput list, this function
applies the desired outputs to the 4 direction pins and 2 PWM pins. Make sure that you dont change the names of these pins
when declaring them because the ExecuteCommand_L298N() function references them directly.


This series of events must happen every loop and if done correctly you should be able to drive your rover after writing only
3 lines of code!!!

Note that the buttons and or sliders are free for you to use to add additional functionality such as controlling servos and
switching between manual controll, autonomous controll and standby modes.

*/


void scoopUp(){
  scooper.write(255);
}

void scoopDown(){
  scooper.write(0);
}

void auton(){
    // Read IR sensor values.  
    int IR_L_Value = analogRead(IR_L);
    int IR_R_Value = analogRead(IR_R);
    Serial.print(IR_L_Value);
    Serial.print(" + ");
    Serial.println(IR_R_Value);
    
    // Set motor outputs.
    if(IR_L_Value < 500 && IR_R_Value < 500){
        MotorOutputs[0] = 1;
        MotorOutputs[1] = 1;
        MotorOutputs[2] = 255;
        MotorOutputs[3] = 255;
    }
    else if(IR_L_Value < 500){
        MotorOutputs[0] = 1;
        MotorOutputs[1] = 0;
        MotorOutputs[2] = 255;
        MotorOutputs[3] = 255;
    }
    else if(IR_R_Value < 500){
        MotorOutputs[0] = 0;
        MotorOutputs[1] = 1;
        MotorOutputs[2] = 255;
        MotorOutputs[3] = 255;
    }
    else{
        MotorOutputs[0] = 0;
        MotorOutputs[1] = 0;
        MotorOutputs[2] = 0;
        MotorOutputs[3] = 0;
    }
    
    ExecuteCommand_L298N(MotorOutputs);
}

void GetBTCommand(char border, float *ans)
{
    char data;           // Variable to store the data
    String Command = ""; // variable to be returned
    while (true)
    {
        if (Serial.available())
        {                         // Checks the availability of Serial port
            data = Serial.read(); // Read the data and stores it in variable.
            if (data == border)
            { // Different commands are separated by # or \n characters. If you read in a # or \n it is clear that a command has just ended.
                if (border == '#')
                {
                    return;
                }
                else
                {
                    BluetoothPadTranslateCommand(ans, Command);
                    return;
                }
            }
            else
            {
                Command = Command + String(data); // Append the character to the end of the command string
            }
        }
    }
}

void BluetoothPadTranslateCommand(float *ans, String Input)
{
    int X;
    int Y;
    int S;
    int C = Input.substring(0, 1).toInt();
    int start = 2;
    int count = 2;
    int i = 0;

    while (i < 2)
    {
        count++;

        if (Input.substring(count, count + 1) == ",")
        {
            if (i == 0)
            {
                X = Input.substring(start, count).toInt();
                start = count + 1;
                i++;
            }
            else if (i == 1)
            {
                Y = Input.substring(start, count).toInt();
                start = count + 1;
                i++;
                S = Input.substring(start, Input.length()).toInt();
            }
        }
    }

    float angle = 180.0 * atan2(Y, X) / 3.14;
    if (Y < 0)
    {
        angle += 360.0;
    }

    float power = sqrt((X * X) + (Y * Y));
    if (power > 100)
    {
        power = 100.0;
    }
    power = power * 2.55;

    ans[0] = angle;
    ans[1] = power;
    ans[2] = S;
    for (int i = 3; i < 7; i++)
    {
        if ((C + 3) == i)
        {
            ans[i] = 1;
        }
        else
        {
            ans[i] = 0;
        }
    }
}

void SimpleMapInput(float *ans, float *Input)
{
    float angle = Input[0];
    float power = Input[1];
    int L_direction;
    int R_direction;
    float L_speed;
    float R_speed;

    // define direction for L motor (corrasponds with motor A on the motor driver)
    if (angle <= 180 || angle >= 330)
    {
        L_direction = 1;
    }
    else
    {
        L_direction = 0;
    }

    // define direction for R motor (corrasponds with motor B on the motor driver)
    if (angle <= 200)
    {
        R_direction = 1;
    }
    else
    {
        R_direction = 0;
    }

    // define L speed
    if (angle >= 90 && angle <= 270)
    {
        L_speed = abs(((angle - 90.0) / 90.0) - 1.0) * power; // The angle will range from 90 -> 270 and will output a L_Speed range of 1 -> -1, multiplied by the the power variable.
    }
    else if (angle < 90)
    {
        L_speed = (0.5 + (angle / 180.0)) * power; // The angle will range from 0 -> 90 and will output a L_Speed range of 0.5 -> 1, multiplied by the the power variable.
    }
    else
    {
        L_speed = abs(0.7 + (((angle - 360.0) / 90.0) * 1.7)) * power; // The angle will range from 270 -> 360 and will output a L_Speed range of -1 -> 0.5, multiplied by the the power variable.
    }

    // define R speed
    if (angle <= 90)
    {
        R_speed = (angle / 90.0) * power;
    }
    else if (angle >= 270)
    {
        R_speed = abs((angle - 360) / 90) * power;
    }
    else if (angle > 90 && angle < 180)
    {
        R_speed = (1 - ((angle - 90) / 180)) * power;
    }
    else
    {
        R_speed = abs(0.7 - (((angle - 180.0) / 90.0) * 1.7)) * power;
    }

    ans[0] = (L_direction);
    ans[1] = (R_direction);
    ans[2] = (L_speed);
    ans[3] = (R_speed);
}

void ExecuteCommand_L298N(float *Command)
{
    if (Command[0] > 0)
    {
        Serial.print("left forward ");
        digitalWrite(DIR_L_F, HIGH);
        digitalWrite(DIR_L_B, LOW);
    }
    else
    {
        Serial.print("left backward ");
        digitalWrite(DIR_L_B, HIGH);
        digitalWrite(DIR_L_F, LOW);
    }

    if (Command[1] > 0)
    {
        Serial.println("right forward ");
        digitalWrite(DIR_R_F, HIGH);
        digitalWrite(DIR_R_B, LOW);
    }
    else
    {
        Serial.println("right backward ");
        digitalWrite(DIR_R_B, HIGH);
        digitalWrite(DIR_R_F, LOW);
    }

    // Next set speed:
    Serial.print(Command[2]);
    Serial.print(" ");
    Serial.println(Command[3]);
    digitalWrite(PWM_L, HIGH);
    digitalWrite(PWM_R, HIGH);
    
}
