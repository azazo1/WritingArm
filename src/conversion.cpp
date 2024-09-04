//
// Created by azazo1 on 2024/9/4.
// 数据转换.

#include <cmath>
#include <Arduino.h>
#include "conversion.h"

/// 通过 rz 坐标获取 alpha 角度(角度制).
///
/// 见文件: res/动臂角度和距离数值说明.png, res/动臂rz轴转角度公式.png
/// 以下代码由公式图片喂给 gpt4o 生成, 实测较为准确.
double getAlpha(const double r, const double z) {
    constexpr double A = -5.05350076575899e-6;
    constexpr double B = -1.010700135518e-5;
    constexpr double C = 0.20663602919164;

    const double sqrt_inner = A * pow(r, 4) + B * r * r * z * z + C * r * r
                              + A * pow(z, 4) + C * z * z - 1;
    const double numerator = 5110.0 * z - 11121.0 * sqrt(sqrt_inner);
    const double denominator = 25.0 * r * r + 5110.0 * r + 25.0 * z * z + 11121.0;
    const double atan_term = 2.0 * atan(numerator / denominator);
    const double cos_term = cos(atan_term);
    const double result = acos(0.01 * r - 1.022 * cos_term);
    return result * 180 / PI;
}

/// 通过 rz 坐标获取 beta 角度(角度制).
///
/// 见文件: res/动臂角度和距离数值说明.png, res/动臂rz轴转角度公式.png
/// 以下代码由公式图片喂给 gpt4o 生成, 实测较为准确.
double getBeta(const double r, const double z) {
    const double numerator = 5110.0 * z - 11121.0 * sqrt(
                                 -5.05350076575899e-6 * pow(r, 4) - 1.0107001531518e-5 * r * r * z *
                                 z +
                                 0.20663602919164 * r * r - 5.05350076575899e-6 * pow(z, 4) +
                                 0.20663602919164 * z * z - 1);
    const double denominator = 25.0 * r * r + 5110.0 * r + 25.0 * z * z + 11121.0;
    return -2.0 * atan(numerator / denominator) * 180 / PI;
}


/// 通过纸面的 xy 坐标获取 A 轴的旋转角度, A 轴为逆时针旋转, 纸平面内向上为起始角度.
/// 同时通过纸面坐标来确定 r 坐标, r 坐标为提供给 getAlpha 和 getBeta 的参数.
///
/// 纸面坐标为 A5 纸面从左上角开始的坐标, x 轴向右, y 轴向下, 不是直接极坐标转换,
/// 还要考虑到机械臂 A 轴的位置, r 的一个端点和 A 轴重合.
///
/// 见文件: res/机械臂各个空间元素参考.png
double getTheta(const double x, const double y, double &r) {
    const double xFromA = 198.5 - x;
    const double yFromA = 105 - y;
    r = sqrt(xFromA * xFromA + yFromA * yFromA);
    return atan2(yFromA, xFromA);
}
