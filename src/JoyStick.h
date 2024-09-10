//
// Created by azazo1 on 2024/9/10.
//

#ifndef JOYSTICK_H
#define JOYSTICK_H


class JoyStick {
    const int pinX;
    const int pinY;
    const int pinSW;
public:
    JoyStick(int pinX, int pinY, int pinSW);

    double getX();

    double getY();

    bool isPressed();
};


#endif //JOYSTICK_H
