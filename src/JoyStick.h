//
// Created by azazo1 on 2024/9/10.
//

#ifndef JOYSTICK_H
#define JOYSTICK_H


class JoyStick {
    const int pinX;
    const int pinY;
    const int pinSW;

    static constexpr int MAX_ENDURANCE = 5;
    int currentEndurance = 0;

public:
    JoyStick(int pinX, int pinY, int pinSW);

    int getX();

    int getY();

    bool isPressed();
};


#endif //JOYSTICK_H
