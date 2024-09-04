//
// Created by azazo1 on 2024/9/4.
//
#include <Arduino.h>
#include <conversion.h>
#include <MyServo.h>
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
    double radius = 42;
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

    auto lf1 = LabeledFrame(String("Z ") + z);
    auto sb1 = Seekbar();
    sb1.setMin(-60);
    sb1.setMax(100);
    sb1.setCurrent(static_cast<int16_t>(z));
    sb1.setOnChangeListener([&lf1, &arm, &theta,&radius, &z](const int16_t val) {
        lf1.setTitle(String("Z ") + val);
        z = val;
        arm.moveToPolar(theta, radius, z);
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
                50, 180, 10000, &linearMapping, [&](const int16_t val) {
                    arm.moveToPolar(theta, val, z);
                    Serial.print(arm.getDegree(0));
                    Serial.print(" ");
                    Serial.print(arm.getDegree(1));
                    Serial.print(" ");
                    Serial.println(arm.getDegree(2));
                    return true;
                }))
            ->then(new ScalaTransition(
                180, 50, 10000, &linearMapping, [&](const int16_t val) {
                    arm.moveToPolar(theta, val, z);
                    Serial.print(arm.getDegree(0));
                    Serial.print(" ");
                    Serial.print(arm.getDegree(1));
                    Serial.print(" ");
                    Serial.println(arm.getDegree(2));
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
