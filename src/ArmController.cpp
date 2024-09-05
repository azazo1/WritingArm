//
// Created by azazo1 on 2024/9/5.
//

#include "ArmController.h"

ArmController::ArmController(WritingArm *arm): arm(arm) {
}

void ArmController::dropPen(double strength) {
    if (strength < 0) {
        strength = 0;
    } else if (strength > 1) {
        strength = 1;
    }
    arm->setZ(PAPER_Z - strength * 3);
}

void ArmController::liftPen() {
    arm->setZ(PAPER_Z + 20);
}

void ArmController::movePen(double x, double y) {
    arm->moveTo(x, y);
}
