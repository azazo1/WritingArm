//
// Created by azazo1 on 2024/9/5.
//

#include "ArmController.h"

ArmController::ArmController(WritingArm *arm): arm(arm) {
}

void ArmController::dropPen(double strength) const {
    if (strength < 0) {
        strength = 0;
    } else if (strength > 1) {
        strength = 1;
    }
    arm->setZ(PAPER_Z - strength * 5);
}

void ArmController::liftPen() const {
    arm->setZ(PAPER_Z + 20);
}

void ArmController::movePen(const double x, const double y) const {
    arm->moveTo(x, y);
}
