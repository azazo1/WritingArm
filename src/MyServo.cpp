//
// Created by azazo1 on 2024/8/28.
//

#include "MyServo.h"

#include <Arduino.h>
#include <esp32-hal-ledc.h>
#include <conversion.h>

constexpr int SERVO_PWM_RESOLUTION = 20; // GPT 说 ledcWrite分辨率最高是 20.
constexpr int SERVO_PWM_FREQUENCY = 50;

MyServo::MyServo(const int channel, const int pin): channel(channel), pin(pin) {
    ledcSetup(channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
    ledcAttachPin(pin, channel);
}

void MyServo::setDegree(const double degree, const uint32_t delay_ms) {
    setDegree(degree);
    delay(delay_ms);
}

void MyServo::setDegree(const double degree) {
    ledcWrite(channel, static_cast<uint32_t>(map(
                  degree,
                  0, 180,
                  1048575 * 0.025,
                  1048575 * 0.125
              )));
    currentDegree = degree;
}

MyServo::~MyServo() {
    ledcDetachPin(pin);
}

double MyServo::getDegree() const {
    return currentDegree;
}

LimitedServo::LimitedServo(const int channel, const int pin): MyServo(channel, pin) {
}

LimitedServo::LimitedServo(const int channel, const int pin, const double minDegree,
                           const double maxDegree, const LimitedMode mode)
    : MyServo(channel, pin), maxDegree(maxDegree), minDegree(minDegree), mode(mode) {
}

void LimitedServo::setMaxDegree(const double degree) {
    maxDegree = degree;
    if (degree < minDegree) {
        minDegree = degree;
    }
}

void LimitedServo::setMinDegree(const double degree) {
    minDegree = degree;
    if (degree > maxDegree) {
        maxDegree = degree;
    }
}

void LimitedServo::setMode(const LimitedMode mode) {
    this->mode = mode;
}

void LimitedServo::setDegree(double degree, const uint32_t delay_ms) {
    switch (mode) {
        case SATURATING: {
            if (degree > maxDegree) {
                degree = maxDegree;
            } else if (degree < minDegree) {
                degree = minDegree;
            }
            break;
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

void LimitedServo::setDegree(double degree) {
    switch (mode) {
        case SATURATING: {
            if (degree > maxDegree) {
                degree = maxDegree;
            } else if (degree < minDegree) {
                degree = minDegree;
            }
            break;
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
