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

#define SERVO_A_PIN 14
#define SERVO_B_PIN 12
#define SERVO_C_PIN 13

#define KNOB_PIN_A 32
#define KNOB_PIN_B 35
#define BUTTON_PIN 23

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

    Scheduler scheduler;
    const LinearMapping linearMapping;
    Screen screen(&display);
    screen.attachToScheduler(&scheduler);
    display.init();
    display.setBrightness(50);
    display.flipScreenVertically();
    View::setFont(FONT_DATA);
    display.setFont(FONT_DATA);

    auto lf1 = LabeledFrame("Degree");
    auto sb1 = Seekbar();
    sb1.setMin(0);
    sb1.setMax(180);
    sb1.setOnChangeListener([&](const int16_t val) {
        servoC.setDegree(val);
        lf1.setTitle(String("Degree ") + val);
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
