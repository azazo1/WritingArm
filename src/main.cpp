#include <Arduino.h>
#include <MyServo.h>

#define SERVO_A_PIN 14
#define SERVO_B_PIN 12
#define SERVO_C_PIN 13

MyServo servoA(0, SERVO_B_PIN);
MyServo servoB(1, SERVO_A_PIN);
MyServo servoC(2, SERVO_C_PIN);

void setup() {
    servoA.setDegree(0);
    servoB.setDegree(0);
    servoC.setDegree(0);
}

void loop() {
}
