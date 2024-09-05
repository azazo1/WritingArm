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

void WritingArm::setDegree(double theta, double alpha, double beta) {
    if (theta < 0 || isnan(theta)) {
        theta = servoA.getDegree();
    }
    if (alpha < 0 || isnan(alpha)) {
        alpha = servoB.getDegree();
    }
    if (beta < 0 || isnan(beta)) {
        beta = servoC.getDegree();
    }
    servoA.setDegree(theta);
    servoB.setDegree(alpha);
    servoC.setDegree(beta);
}

double WritingArm::getAppropriateDelay(double theta, double alpha, double beta) const {
    const double rawTheta = servoA.getDegree();
    const double rawAlpha = servoB.getDegree();
    const double rawBeta = servoC.getDegree();
    if (theta < 0 || isnan(theta)) {
        theta = rawTheta;
    }
    if (alpha < 0 || isnan(alpha)) {
        alpha = rawAlpha;
    }
    if (beta < 0 || isnan(beta)) {
        beta = rawBeta;
    }
    const double thetaDelay = abs(theta - rawTheta) * THETA_DELAY_FACTOR;
    const double alphaDelay = abs(alpha - rawAlpha) * ALPHA_DELAY_FACTOR;
    const double betaDelay = abs(beta - rawBeta) * BETA_DELAY_FACTOR;
    return max(thetaDelay, max(alphaDelay, betaDelay));
}

double WritingArm::getDegree(const int servo) const {
    switch (servo) {
        case 0:
            return servoA.getDegree();
        case 1:
            return servoB.getDegree();
        case 2:
            return servoC.getDegree();
        default: return -1;
    }
}

void WritingArm::moveTo(const double x, const double y, const double z) {
    double r;
    const double theta = getTheta(x, y, r);
    moveToPolar(theta, r, z);
}

void WritingArm::moveTo(const double x, const double y) {
    moveTo(x, y, z);
}

void WritingArm::moveToPolar(const double theta, double r, double z) {
    this->z = z;
    getXY(theta, r, x, y);
    // 最大半径 r 为 200mm.
    const double ratio = r / MAX_R;
    // 前半是一个凸函数, 使小的 r 的补偿比普通的线性补偿小.
    z += (std::exp(COMPENSATION_K * ratio) - 1) / (std::exp(COMPENSATION_K) - 1)
            * Z_COMPENSATION_FACTOR;

    // 限制 r 的值, 防止 alpha 和 beta 值为 nan.
    if (z * z + r * r > MAX_RZ_SQUARES) {
        r = sqrt(MAX_RZ_SQUARES - z * z);
    }

    const double alpha = getAlpha(r, z);
    const double beta = getBeta(r, z);
    setDegree(theta, alpha, beta);
}

void WritingArm::moveToPolar(const double theta, const double r) {
    moveToPolar(theta, r, z);
}

void WritingArm::setZ(const double z) {
    moveTo(x, y, z);
}

double WritingArm::getX() const {
    return x;
}

double WritingArm::getY() const {
    return y;
}
