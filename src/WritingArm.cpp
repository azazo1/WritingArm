//
// Created by azazo1 on 2024/9/4.
//

#include "WritingArm.h"

#include <conversion.h>

WritingArm::WritingArm(const int servoAPin, const int servoBPin, const int servoCPin)
    : servoA(0, servoAPin, 25, 160, SATURATING),
      servoB(1, servoBPin, 25, 125, SATURATING),
      servoC(2, servoCPin, 6, 80, SATURATING) {
}

void WritingArm::setDegree(int theta, int alpha, int beta) {
    if (theta < 0) {
        theta = servoA.getCurrentDegree();
    }
    if (alpha < 0) {
        alpha = servoB.getCurrentDegree();
    }
    if (beta < 0) {
        beta = servoC.getCurrentDegree();
    }
    servoA.setDegree(theta);
    servoB.setDegree(alpha);
    servoC.setDegree(beta);
}

double WritingArm::getAppropriateDelay(int theta, int alpha, int beta) const {
    const int rawTheta = servoA.getCurrentDegree();
    const int rawAlpha = servoB.getCurrentDegree();
    const int rawBeta = servoC.getCurrentDegree();
    if (theta < 0) {
        theta = rawTheta;
    }
    if (alpha < 0) {
        alpha = rawAlpha;
    }
    if (beta < 0) {
        beta = rawBeta;
    }
    const double thetaDelay = abs(theta - rawTheta) * THETA_DELAY_FACTOR;
    const double alphaDelay = abs(alpha - rawAlpha) * ALPHA_DELAY_FACTOR;
    const double betaDelay = abs(beta - rawBeta) * BETA_DELAY_FACTOR;
    return max(thetaDelay, max(alphaDelay, betaDelay));
}

int WritingArm::getDegree(const int servo) const {
    switch (servo) {
        case 0:
            return servoA.getCurrentDegree();
        case 1:
            return servoB.getCurrentDegree();
        case 2:
            return servoC.getCurrentDegree();
        default: return -1;
    }
}

void WritingArm::moveTo(const double x, const double y, const double z) {
    double r;
    const double theta = getTheta(x, y, r);
    moveToPolar(theta, r, z);
}

void WritingArm::moveToPolar(const double theta, const double r, double z) {
    z += r * Z_COMPENSATION_FACTOR;
    const double alpha = getAlpha(r, z);
    const double beta = getBeta(r, z);
    setDegree(
        static_cast<int>(round(theta)),
        static_cast<int>(round(alpha)),
        static_cast<int>(round(beta))
    );
}
