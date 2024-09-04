//
// Created by azazo1 on 2024/9/4.
//

#ifndef WRITINGARM_H
#define WRITINGARM_H
#include <MyServo.h>


/// 写字机械臂三个舵机的驱动类,
class WritingArm {
    static constexpr double THETA_DELAY_FACTOR = 30;
    static constexpr double ALPHA_DELAY_FACTOR = 5;
    static constexpr double BETA_DELAY_FACTOR = 3;
    /// r 过大时, z 实际坐标和理论坐标会相差较大, 即动臂会下垂, 加此参数修正, 此参数是反复测试得来.
    /// 此参数越大, 末端补偿越大.
    static constexpr double Z_COMPENSATION_FACTOR = 20;
    /// 同样同于 z 坐标补偿, 此参数越大, 较小的 r 得到的补偿就越大, 即补偿增长的越快.
    static constexpr double COMPENSATION_K = 0.5;

    LimitedServo servoA;
    LimitedServo servoB;
    LimitedServo servoC;

public:
    WritingArm(int servoAPin, int servoBPin, int servoCPin);

    /// 设置舵机的角度, 不自动添加延迟.
    /// 任一参数给负数表示选用当前舵机的角度.
    void setDegree(double theta, double alpha, double beta);

    /// 获取转动舵机到指定角度所需要的合适的延迟, setDegree 后添加合适延迟可以防止舵机过分抖动.
    /// 单位为毫秒
    ///
    /// 任一参数给负数表示选用当前舵机的角度.
    double getAppropriateDelay(double theta, double alpha, double beta) const;

    /// 获取舵机的角度, 0 表示 A 轴舵机, 1 表示 B 轴舵机, 2 表示 C 轴舵机.
    /// 返回 -1 表示参数不合理.
    double getDegree(int servo) const;

    /// 笔移动到纸面坐标 xy , 以及 z 坐标, 是对 setDegree 的封装.
    void moveTo(double x, double y, double z);

    /// 移动到极坐标, 以及 z 坐标, 是对 setDegree 的封装.
    void moveToPolar(double theta, double r, double z);
};


#endif //WRITINGARM_H
