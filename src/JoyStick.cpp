//
// Created by azazo1 on 2024/9/10.
//

#include "JoyStick.h"

#include <esp32-hal-gpio.h>

JoyStick::JoyStick(int pinX, int pinY, int pinSW): pinX(pinX), pinY(pinY), pinSW(pinSW) {
    pinMode(pinSW, INPUT);
}

double JoyStick::getX() {
    return 2.0 * analogRead(pinX) / 256 - 0.5;
}

double JoyStick::getY() {
    return 2.0 * analogRead(pinY) / 256 - 0.5;
}

bool JoyStick::isPressed() {
    return !digitalRead(pinSW);
}
