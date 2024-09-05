//
// Created by azazo1 on 2024/9/5.
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
    double z = PAPER_Z + 30;
    // arm.moveToPolar(theta, radius, z);
    // arm.moveTo(PAPER_WIDTH / 2, PAPER_HEIGHT / 2, z);
    // arm.moveTo(0, 0, z);
    arm.moveTo(100, 100, z);
    // while (true) {
    // arm.moveTo(0, 0, z);
    // delay(1000);
    // arm.moveTo(0, 100);
    // delay(1000);
    // arm.moveTo(100, 100);
    // delay(1000);
    // arm.moveTo(100, 0);
    // delay(1000);
    // }

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
    sb1.setMin(-10);
    sb1.setMax(30);
    sb1.setCurrent(static_cast<int16_t>(z));
    sb1.setOnChangeListener([&lf1, &arm, &z](const double val) {
        lf1.setTitle(String("Z ") + val);
        z = val;
        arm.setZ(val);
    });
    sb1.setOnConfirmListener([&](const int16_t) {
        scheduler.addSchedule(
            (new SequenceSchedulable())
            ->then(new ScalaTransition(
                0, 100, 3000, &linearMapping, [&](const double val) {
                    arm.moveTo(100, val, z);
                    return true;
                }))
            ->then(new ScalaTransition(
                100, 0, 3000, &linearMapping, [&](const double val) {
                    arm.moveTo(100, val, z);
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
