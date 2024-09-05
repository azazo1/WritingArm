//
// Created by azazo1 on 2024/8/28.
//

#ifndef MYSERVO_H
#define MYSERVO_H
#include <Arduino.h>


class MyServo {
    const int channel;
    const int pin;
    double currentDegree = 0;

public:
    MyServo(MyServo &) = delete;

    MyServo(MyServo &&) = delete;

    MyServo(int channel, int pin);

    virtual void setDegree(double degree, uint32_t delay_ms);

    /// 无延迟版本的 setDegree.
    virtual void setDegree(double degree);

    virtual ~MyServo();

    /// 获取当前的角度值.
    double getDegree() const;
};

enum LimitedMode {
    /// 跨越最大值或者最小值的时候取最大值和最小值.
    SATURATING,
    /// 跨越最大值和最小值的时候取消本次操作, 取消本次操作时, 不会产生延迟操作.
    CHECKED,
    /// 不对最大值和最小值进行检查, 和 MyServo 没有区别.
    UNCHECKED,
};

/// 限制角度的舵机.
/// 舵机的角度限制在 maxDegree 和 minDegree 间(两端包含).
class LimitedServo final : public MyServo {
    double maxDegree = 180;
    double minDegree = 0;
    LimitedMode mode = SATURATING;

public:
    LimitedServo(LimitedServo &) = delete;

    LimitedServo(LimitedServo &&) = delete;

    LimitedServo(int channel, int pin);

    LimitedServo(int channel, int pin, double minDegree, double maxDegree, LimitedMode mode);

    /// 会确保 maxDegree 大于等于 minDegree.
    void setMaxDegree(double degree);

    /// 会确保 maxDegree 大于等于 minDegree.
    void setMinDegree(double degree);

    /// 间 LimitedMode.
    void setMode(LimitedMode mode);

    void setDegree(double degree, uint32_t delay_ms) override;

    void setDegree(double degree) override;
};

#endif //MYSERVO_H
