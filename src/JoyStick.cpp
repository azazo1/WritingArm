//
// Created by azazo1 on 2024/9/10.
//

#include "JoyStick.h"

#include <esp32-hal-gpio.h>

JoyStick::JoyStick(int pinX, int pinY, int pinSW): pinX(pinX), pinY(pinY), pinSW(pinSW) {
    pinMode(pinSW, INPUT);
}

int JoyStick::getX() {
    const int val = analogRead(pinX);
    if (val < 1000) {
        return -1;
    }
    if (val > 4000) {
        return 1;
    }
    return 0;
}

int JoyStick::getY() {
    const int val = analogRead(pinY);
    if (val < 1000) {
        return -1;
    }
    if (val > 4000) {
        return 1;
    }
    return 0;
}

bool JoyStick::isPressed() {
    const bool cond = !digitalRead(pinSW);
    if (cond) {
        if (currentEndurance > MAX_ENDURANCE) {
            return true;
        }
        currentEndurance++;
    } else {
        currentEndurance = 0;
    }
    return false;
}
