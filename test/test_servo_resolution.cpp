//
// Created by azazo1 on 2024/9/4.
//
#include <Arduino.h>
#include <conversion.h>
#include <WritingArm.h>
#include <event/ButtonEvent.h>
#include <event/KnobEvent.h>
#include <propotion_mapping/LinearMapping.h>
#include <sche/ScalaTransition.h>
#include <sche/SchedulableButtonEvent.h>
#include <sche/SchedulableKnobEvent.h>
#include <sche/SequenceSchedulable.h>
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


auto display = SSD1306Wire(0x3C, SDA, SCL);

void setup() {
    Serial.begin(9600);

    WritingArm arm(SERVO_A_PIN, SERVO_B_PIN, SERVO_C_PIN);
    double radius = 70;
    double theta = 90;
    double z = PAPER_Z + 60;
    arm.moveToPolar(theta, radius, z);

    Scheduler scheduler;
    const LinearMapping linearMapping;
    Screen screen(&display);
    screen.attachToScheduler(&scheduler);
    display.init();
    display.setBrightness(50);
    display.flipScreenVertically();
    View::setFont(FONT_DATA);
    display.setFont(FONT_DATA);

    auto lf1 = LabeledFrame(String("Beta ") + arm.getDegree(2));
    auto sb1 = Seekbar();
    sb1.setMin(600);
    sb1.setMax(10000);
    sb1.setCurrent(static_cast<int16_t>(arm.getDegree(2) * 100.0));
    sb1.setOnChangeListener([&lf1, &arm](const int16_t val) {
        const double beta = val * 0.01;
        lf1.setTitle(String("Beta ") + beta);
        arm.setDegree(-1, -1, beta);
        Serial.print(arm.getDegree(0));
        Serial.print(" ");
        Serial.print(arm.getDegree(1));
        Serial.print(" ");
        Serial.println(arm.getDegree(2));
    });
    sb1.setOnConfirmListener([&](const int16_t) {
        scheduler.addSchedule(
            (new SequenceSchedulable())
            ->then(new ScalaTransition(
                6, 60, 30000, &linearMapping, [&](const double val) {
                    const double beta = val;
                    lf1.setTitle(String("Beta ") + beta);
                    arm.setDegree(-1, -1, beta);
                    return true;
                }))
            ->then(new ScalaTransition(
                60, 6, 30000, &linearMapping, [&](const int16_t val) {
                    const double beta = val;
                    lf1.setTitle(String("Beta ") + beta);
                    arm.setDegree(-1, -1, beta);
                    return true;
                }))
        );
    });
    sb1.setOnCancelListener([](const int16_t) {
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
