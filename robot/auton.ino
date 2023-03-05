// Bangbang ir sensor line follower

float MotorOutputs[4];    // Left Direction, Right Direction, Left Speed, Right Speed.

int DIR_L_F = 9; // Left Direction pin that will indicate forewards movement (1 for forewards, 0 for backwards).
int DIR_L_B = 10; // Left Direction pin that will indicate backwards movement (1 for backwards, 0 for forewards).
int PWM_L = 11;   // Speed controll pin. *** This pin must be plugged into an output pin on the arduino that is labled PWM ***.

int DIR_R_F = 5; // Right Direction pin that will indicate forewards movement (1 for forewards, 0 for backwards)
int DIR_R_B = 6; // Left Direction pin that will indicate backwards movement (1 for backwards, 0 for forewards).
int PWM_R = 3;   // Speed controll pin. *** This pin must be plugged into an output pin on the arduino that is labled PWM ***.

int IR_L = A0; // Left IR sensor pin.
int IR_R = A1; // Right IR sensor pin.

void setup(){
    pinMode(DIR_L_F, OUTPUT);
    pinMode(DIR_L_B, OUTPUT);
    pinMode(PWM_L, OUTPUT);
    pinMode(DIR_R_F, OUTPUT);
    pinMode(DIR_R_B, OUTPUT);
    pinMode(PWM_R, OUTPUT);
    pinMode(IR_L, INPUT);
    pinMode(IR_R, INPUT);
}

void auton(){
    // Read IR sensor values.
    int IR_L_Value = analogRead(IR_L);
    int IR_R_Value = analogRead(IR_R);
    
    // Set motor outputs.
    if(IR_L_Value > 500 && IR_R_Value > 500){
        MotorOutputs[0] = 1;
        MotorOutputs[1] = 1;
        MotorOutputs[2] = 255;
        MotorOutputs[3] = 255;
    }
    else if(IR_L_Value > 500){
        MotorOutputs[0] = 1;
        MotorOutputs[1] = 0;
        MotorOutputs[2] = 255;
        MotorOutputs[3] = 255;
    }
    else if(IR_R_Value > 500){
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

void ExecuteCommand_L298N(float *Command)
{
    if (Command[0] > 0)
    {
        digitalWrite(DIR_L_F, HIGH);
        digitalWrite(DIR_L_B, LOW);
    }
    else
    {
        digitalWrite(DIR_L_B, HIGH);
        digitalWrite(DIR_L_F, LOW);
    }

    if (Command[1] > 0)
    {
        digitalWrite(DIR_R_F, HIGH);
        digitalWrite(DIR_R_B, LOW);
    }
    else
    {
        digitalWrite(DIR_R_B, HIGH);
        digitalWrite(DIR_R_F, LOW);
    }

    // Next set speed:
    analogWrite(PWM_L, Command[2]);
    analogWrite(PWM_R, Command[3]);
}