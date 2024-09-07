//
// Created by azazo1 on 2024/9/5.
//

#include "ArmController.h"

#include <sche/ConsumerSchedulable.h>
#include <sche/DelaySchedulable.h>
#include <sche/SchedulableFromLambda.h>

ArmController::ArmController(WritingArm *arm): arm(arm) {
}

double ArmController::dropPen(double strength) const {
    if (strength < 0) {
        strength = 0;
    } else if (strength > 1) {
        strength = 1;
    }
    const double z = PAPER_Z - strength * 2;
    arm->setZ(z);
    return z;
}

void ArmController::liftPen() const {
    arm->setZ(OFF_PAPER_Z);
}

void ArmController::movePen(const double x, const double y) const {
    arm->moveTo(x, y);
}

void ArmController::drawActionSequence(std::vector<double> sequence, sche::Scheduler &scheduler) {
    this->sequence = std::move(sequence);
    sequenceExecutionIdx = 0;
    const auto currentSequenceBatch = ++sequenceBatch;
    scheduler.addSchedule(new sche::ConsumerSchedulable([this, currentSequenceBatch]() {
        if (currentSequenceBatch != sequenceBatch) {
            // 序列被中断.
            return static_cast<sche::Schedulable *>(nullptr);
        }
        if (sequenceExecutionIdx + 2 >= this->sequence.size()) {
            // 序列执行结束.
            return static_cast<sche::Schedulable *>(nullptr);
        }
        const double x = this->sequence[sequenceExecutionIdx];
        const double y = this->sequence[sequenceExecutionIdx + 1];
        const double pressure = this->sequence[sequenceExecutionIdx + 2];
        // 上个 action 延迟结束, 发送新的action 到机械臂
        sequenceExecutionIdx += 3;
        double z;
        if (pressure < 0.1) {
            liftPen();
            z = OFF_PAPER_Z;
        } else {
            z = dropPen(pressure);
        }
        movePen(x, y);
        double r;
        const double theta = getTheta(x, y, r);
        const double alpha = getAlpha(r, z);
        const double beta = getBeta(r, z);
        return static_cast<sche::Schedulable *>(new sche::DelaySchedulable(
            static_cast<sche::mtime_t>(arm->getAppropriateDelay(theta, alpha, beta)), 0,
            new sche::SchedulableFromLambda([](sche::mtime_t) {
                return false;
            })
        ));
    }));
}
