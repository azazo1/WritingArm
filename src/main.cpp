#include <Arduino.h>
#include <NetAdapter.h>
#include <UI.h>
#include <WritingArm.h>
#include <event/ButtonEvent.h>
#include <event/KnobEvent.h>
#include <propotion_mapping/LinearMapping.h>
#include <sche/ScalaTransition.h>
#include <sche/SchedulableButtonEvent.h>
#include <sche/SchedulableKnobEvent.h>
#include <esp_now.h>
#include <JoyStick.h>

#include "view/Screen.h"
#include "Knob.h"

#define FONT_DATA ArialMT_Plain_16

#define SERVO_A_PIN 17
#define SERVO_B_PIN 5
#define SERVO_C_PIN 18

#define KNOB_PIN_A 15
#define KNOB_PIN_B 4
#define BUTTON_PIN 16

#define JOYSTICK_VRX_PIN 25
#define JOYSTICK_VRY_PIN 26
#define JOYSTICK_SW_PIN 27

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
UI ui(&screen);

WritingArm arm(SERVO_A_PIN, SERVO_B_PIN, SERVO_C_PIN);
ArmController controller(&arm);
NetAdapter netAdapter(&controller, SERVER_PORT, &scheduler);

JoyStick js(JOYSTICK_VRX_PIN, JOYSTICK_VRY_PIN, JOYSTICK_SW_PIN);

struct ArmInstruction {
    double x;
    double y;
    double strength;
};

void onInstruction(const uint8_t *max_addr, const uint8_t *data, int data_len) {
    const auto ins = reinterpret_cast<const ArmInstruction *>(data);
    controller.movePen(ins->x, ins->y);
    if (ins->strength < 0.1) {
        controller.liftPen();
    } else {
        controller.dropPen(ins->strength);
    }
}

void setup() {
    Serial.begin(9600);

    WiFiClass::mode(WIFI_MODE_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing esp-now");
    } else {
        esp_now_register_recv_cb(onInstruction);
    }

    controller.movePen(50, 50);
    controller.liftPen();

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

    scheduler.addSchedule(new SchedulableFromLambda([](mtime_t) {
        Serial.println(String("x: ") + js.getX() + " y: " + js.getY() + " p: " + js.isPressed());
        // error: adc2 不能和 wifi 一起使用.
        return true;
    }));

    ui.build(arm, netAdapter);
}

void loop() {
    scheduler.mainloop();
}
