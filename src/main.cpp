#include <Arduino.h>
#include <NetAdapter.h>
#include <WritingArm.h>
#include <event/ButtonEvent.h>
#include <event/KnobEvent.h>
#include <propotion_mapping/LinearMapping.h>
#include <sche/ScalaTransition.h>
#include <sche/SchedulableButtonEvent.h>
#include <sche/SchedulableKnobEvent.h>
#include <tiny_websockets/server.hpp>

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

#define SERVER_PORT 13300

using namespace knob;
using namespace sche;
using namespace propmap;
using namespace view;
using namespace event;


auto display = SSD1306Wire(0x3C, SDA, SCL);
Scheduler scheduler;
const LinearMapping linearMapping;
Screen screen(&display);
auto sb1 = Seekbar();

WritingArm arm(SERVO_A_PIN, SERVO_B_PIN, SERVO_C_PIN);
ArmController controller(&arm);
NetAdapter netAdapter(&controller, SERVER_PORT, &scheduler);

void setup() {
    Serial.begin(9600);

    controller.liftPen();
    controller.movePen(100, 100);
    netAdapter.modeAP("esp32-azo", "asdfghjkl");

    screen.attachToScheduler(&scheduler);
    display.init();
    display.setBrightness(50);
    display.flipScreenVertically();
    View::setFont(FONT_DATA);
    display.setFont(FONT_DATA);

    scheduler.addSchedule(new SchedulableKnobEvent(
        KNOB_PIN_A, KNOB_PIN_B, [](const int delta) {
            screen.dispatchEvent(KnobEvent(screen, delta));
            return screen.isAlive();
        }));
    scheduler.addSchedule(new SchedulableButtonEvent(
        BUTTON_PIN, [](const mtime_t pressedDuration) {
            screen.dispatchEvent(ButtonEvent(screen, static_cast<int>(pressedDuration)));
            return screen.isAlive();
        }));

    sb1.setMax(100);
    sb1.setMin(1);
    sb1.setStep(1);
    sb1.setCurrent(50);
    screen.pushRootView(&sb1);
}

void loop() {
    scheduler.mainloop();
}
