//
// Created by azazo1 on 2024/8/28.
//

#include "MyServo.h"

#include <Arduino.h>
#include <esp32-hal-ledc.h>
constexpr int SERVO_PWM_RESOLUTION = 16;
constexpr int SERVO_PWM_FREQUENCY = 50;

MyServo::MyServo(const int channel, const int pin): channel(channel), pin(pin) {
    ledcSetup(channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
    ledcAttachPin(pin, channel);
}

void MyServo::setDegree(const int degree, const uint32_t delay_ms) {
    setDegree(degree);
    delay(delay_ms);
}

void MyServo::setDegree(const int degree) {
    ledcWrite(channel, map(
                  degree,
                  0, 180,
                  static_cast<uint32_t>(65535 * 0.025),
                  static_cast<uint32_t>(65535 * 0.125)
              ));
    currentDegree = degree;
}

MyServo::~MyServo() {
    ledcDetachPin(pin);
}

int MyServo::getCurrentDegree() const {
    return currentDegree;
}

LimitedServo::LimitedServo(const int channel, const int pin): MyServo(channel, pin) {
}

LimitedServo::LimitedServo(const int channel, const int pin, const int minDegree,
                           const int maxDegree, const LimitedMode mode)
    : MyServo(channel, pin), minDegree(minDegree), maxDegree(maxDegree), mode(mode) {
}

void LimitedServo::setMaxDegree(const int degree) {
    maxDegree = degree;
    if (degree < minDegree) {
        minDegree = degree;
    }
}

void LimitedServo::setMinDegree(const int degree) {
    minDegree = degree;
    if (degree > maxDegree) {
        maxDegree = degree;
    }
}

void LimitedServo::setMode(const LimitedMode mode) {
    this->mode = mode;
}

void LimitedServo::setDegree(int degree, const uint32_t delay_ms) {
    switch (mode) {
        case SATURATING: {
            if (degree > maxDegree) {
                degree = maxDegree;
            } else if (degree < minDegree) {
                degree = minDegree;
            }
        }
        case CHECKED: {
            if (degree > maxDegree || degree < minDegree) {
                return;
            }
        }
        case UNCHECKED:
        default: ;
    }
    MyServo::setDegree(degree, delay_ms);
}

void LimitedServo::setDegree(int degree) {
    switch (mode) {
        case SATURATING: {
            if (degree > maxDegree) {
                degree = maxDegree;
            } else if (degree < minDegree) {
                degree = minDegree;
            }
        }
        case CHECKED: {
            if (degree > maxDegree || degree < minDegree) {
                return;
            }
        }
        case UNCHECKED:
        default: ;
    }
    MyServo::setDegree(degree);
}
