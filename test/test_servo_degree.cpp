#include <Arduino.h>
#include <MyServo.h>
#include <event/ButtonEvent.h>
#include <event/KnobEvent.h>
#include <propotion_mapping/LinearMapping.h>
#include <sche/SchedulableButtonEvent.h>
#include <sche/SchedulableKnobEvent.h>
#include <view/LabeledFrame.h>

#include "view/Seekbar.h"
#include "view/Screen.h"
#include "Knob.h"

#define FONT_DATA ArialMT_Plain_16

#define SERVO_A_PIN 17
#define SERVO_B_PIN 5
#define SERVO_C_PIN 18

#define KNOB_PIN_A 15
#define KNOB_PIN_B 4
#define BUTTON_PIN 16

using namespace knob;
using namespace sche;
using namespace propmap;
using namespace view;
using namespace event;

LimitedServo servoA(0, SERVO_A_PIN, 25, 160, SATURATING);
LimitedServo servoB(1, SERVO_B_PIN, 25, 125, SATURATING);
LimitedServo servoC(2, SERVO_C_PIN, 6, 70, SATURATING);
auto display = SSD1306Wire(0x3C, SDA, SCL);

void setup() {
    servoA.setDegree(90);
    servoB.setDegree(70);
    servoC.setDegree(30);

    Scheduler scheduler;
    const LinearMapping linearMapping;
    Screen screen(&display);
    screen.attachToScheduler(&scheduler);
    display.init();
    display.setBrightness(50);
    display.flipScreenVertically();
    View::setFont(FONT_DATA);
    display.setFont(FONT_DATA);

    const int currentDegree = servoA.getCurrentDegree();
    auto lf1 = LabeledFrame(String("A Degree ") + currentDegree);
    auto sb1 = Seekbar();
    int servoSelect = 0;
    sb1.setMin(0);
    sb1.setMax(180);
    sb1.setCurrent(static_cast<int16_t>(currentDegree));
    sb1.setOnChangeListener([&lf1, &servoSelect, &sb1](const int16_t val) {
        switch (servoSelect) {
            case 0: {
                lf1.setTitle(String("A Degree ") + val);
                servoA.setDegree(val);
                sb1.setCurrent(static_cast<int16_t>(servoA.getCurrentDegree()));
            }
            break;
            case 1: {
                lf1.setTitle(String("B Degree ") + val);
                servoB.setDegree(val);
                sb1.setCurrent(static_cast<int16_t>(servoB.getCurrentDegree()));
            }
            break;
            case 2: {
                lf1.setTitle(String("C Degree ") + val);
                servoC.setDegree(val);
                sb1.setCurrent(static_cast<int16_t>(servoC.getCurrentDegree()));
            }
            break;
            default: ;
        }
    });
    sb1.setOnConfirmListener([&sb1, &lf1, &servoSelect](const int16_t) {
        if (++servoSelect > 2) {
            servoSelect = 0;
        }
        switch (servoSelect) {
            case 0: {
                const int degree = servoA.getCurrentDegree();
                lf1.setTitle(String("A Degree ") + degree);
                sb1.setCurrent(static_cast<int16_t>(degree));
            }
            break;
            case 1: {
                const int degree = servoB.getCurrentDegree();
                lf1.setTitle(String("B Degree ") + degree);
                sb1.setCurrent(static_cast<int16_t>(degree));
            }
            break;
            case 2: {
                const int degree = servoC.getCurrentDegree();
                lf1.setTitle(String("C Degree ") + degree);
                sb1.setCurrent(static_cast<int16_t>(degree));
            }
            break;
            default:
                lf1.setTitle("Error");
        }
    });
    sb1.setStep(1);
    lf1.addChild(&sb1);

    screen.pushRootView(&lf1);

    scheduler.addSchedule(new SchedulableKnobEvent(
        KNOB_PIN_A, KNOB_PIN_B, [&screen](const int delta) {
            screen.dispatchEvent(KnobEvent(screen, delta));
            return screen.isAlive();
        }));
    scheduler.addSchedule(new SchedulableButtonEvent(
        BUTTON_PIN, [&screen](const mtime_t pressedDuration) {
            screen.dispatchEvent(ButtonEvent(screen, static_cast<int>(pressedDuration)));
            return screen.isAlive();
        }));
    scheduler.mainloop();
}

void loop() {
}
