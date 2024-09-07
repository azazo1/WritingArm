//
// Created by azazo1 on 2024/9/4.
//

#ifndef WRITINGARM_H
#define WRITINGARM_H
#include <conversion.h>
#include <MyServo.h>

constexpr double PAPER_WIDTH = 148.5;
constexpr double PAPER_HEIGHT = 210.0;

constexpr double MAX_RZ_SQUARES = 40883.8025; // 使 alpha 和 beta 有效的最大 r 和 z 坐标平方和.
constexpr double MAX_R = sqrt(MAX_RZ_SQUARES); // z 坐标为 0 时的使 alpha 和 beta 不为 nan 的最大 r.

/// 写字机械臂三个舵机的驱动类, 负责写字机械臂的一些基础移动行为.
class WritingArm {
    static constexpr double THETA_DELAY_FACTOR = 20;
    static constexpr double ALPHA_DELAY_FACTOR = 20;
    static constexpr double BETA_DELAY_FACTOR = 20;
    /// r 过大时, z 实际坐标和理论坐标会相差较大, 即动臂会下垂, 加此参数修正, 此参数是反复测试得来.
    /// 此参数越大, 末端补偿越大.
    static constexpr double Z_COMPENSATION_FACTOR = 20;
    /// 同样同于 z 坐标补偿, 此参数越大, 较小的 r 得到的补偿就越大, 即补偿增长的越快.
    static constexpr double COMPENSATION_K = 0.5;

    LimitedServo servoA;
    LimitedServo servoB;
    LimitedServo servoC;

    /// 缓存的 z 坐标, 储存的是补偿前的 z 坐标.
    /// 不能储存补偿后的 z 坐标, 否则在默认 z 值不变的 moveTo, moveToPolar 方法中会过分补偿.
    /// 此成员的初始值不起作用, 仅作占位.
    double z = PAPER_Z + 10;
    /// 缓存的纸面 x 坐标.
    /// 如果是由 moveToPolar 更新, 那么使用的是缩减前的 r 值来计算.
    double x = 100;
    /// 缓存的纸面 y 坐标.
    /// 如果是由 moveToPolar 更新, 那么使用的是缩减前的 r 值来计算.
    double y = 100;
    /// 缓存的 r 值(缩减前).
    double r = 50;

public:
    WritingArm(WritingArm &) = delete;

    WritingArm(WritingArm &&) = delete;

    WritingArm(int servoAPin, int servoBPin, int servoCPin);

    /// 设置舵机的角度, 不自动添加延迟.
    /// 任一参数给负数或 nan 表示选用当前舵机的角度.
    /// \param cache 是否缓存变量.
    void setDegree(double theta, double alpha, double beta, bool cache = true);

    /// 获取转动舵机到指定角度所需要的合适的延迟, setDegree 后添加合适延迟可以防止舵机过分抖动.
    /// 单位为毫秒
    ///
    /// 任一参数给负数或 nan 表示选用当前舵机的角度.
    double getAppropriateDelay(double theta, double alpha, double beta) const;

    /// 获取舵机的角度, 0 表示 A 轴舵机角度 theta, 1 表示 B 轴舵机角度 beta, 2 表示 C 轴舵机角度 alpha.
    /// 返回 -1 表示参数不合理.
    double getDegree(int servo) const;

    /// 笔移动到纸面坐标 xy , 以及 z 坐标, 是对 setDegree 的封装.
    ///
    /// xy给负数或 nan 则使用缓存值.
    /// z 给 nan 则使用缓存值.
    void moveTo(double x, double y, double z);

    /// 笔移动到纸面坐标 xy, 同 moveTo(x, y, z), 但是保持 z 坐标不变.
    /// xy 给负数或 nan 则使用缓存值.
    void moveTo(double x, double y);

    /// 移动到极坐标, 以及 z 坐标, 是对 setDegree 的封装.
    ///
    /// 当 r 和 z 的平方和大于 MAX_RZ_SQUARES 时, 会计算出 alpha 和 beta 为 nan,
    /// 故此时, 固定 z 不动, 收缩 r 值到有效值.
    /// \param theta 给负数或 nan 则使用缓存值.
    /// \param r 给负数或 nan 则使用缓存值.
    /// \param z 给 nan 则使用缓存值.
    void moveToPolar(double theta, double r, double z);

    /// 只改变 z 坐标.
    void setZ(double z);

    double getX() const;

    double getY() const;

    double getZ() const;

    double getR() const;
};


#endif //WRITINGARM_H
