#include <Servo.h>

Servo scooper;

void up(){
  scooper.write(255);
}
void down(){
  scooper.write(0);
}


void setup(){
  scooper.attach(11); // pwm pin
}

void loop(){

}