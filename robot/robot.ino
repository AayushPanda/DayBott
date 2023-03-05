// Run a two wheeled robot using an h-bridge motor driver

class Motor {
    private:
        uint8_t pin1;
        uint8_t pin2;
        uint8_t speedPin;

    public:
        int speed;
        int direction;
        Motor(uint8_t pin1, uint8_t pin2, uint8_t speedPin){
            this->pin1 = pin1;
            this->pin2 = pin2;
            this->speedPin = speedPin;
        }
        void setSpeed(int speed){
            this->speed = speed;
            if (speed > 0){
                digitalWrite(pin1, 1);
                digitalWrite(pin2, 0);
                analogWrite(speedPin, abs(speed));
            } else if (speed < 0){
                digitalWrite(pin1, 0);
                digitalWrite(pin2, 1);
                analogWrite(speedPin, abs(speed));
            } else {
                digitalWrite(pin1, 0);
                digitalWrite(pin2, 0);
                analogWrite(speedPin, 0);
            }
       }
};

Motor leftMotor(7, 8, 9);
Motor rightMotor(12, 13, 14);

void setup(){

}

void loop(){
    
}
