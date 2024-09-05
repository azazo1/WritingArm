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
/// 见文件: res/机械臂各个空间元素参考.png, res/动臂部分固定尺寸参考.png.
double getTheta(const double x, const double y, double &r) {
    const double xFromA = 198.5 - x;
    const double yFromA = 105 - y;
    // 动臂不是只是由两个杆组成的, 还有一些偏移量, 参考 动臂部分固定尺寸参考.png.
    r = sqrt(xFromA * xFromA + yFromA * yFromA);
    r -= 20.15 - 3.5;
    // 注意 atan2 第一个参数是平面直角坐标系的 y 轴, 但是这里根据空间管理, 填纸面 x 轴.
    return atan2(xFromA, yFromA) * 180 / PI;
}

/// 通过 theta 和 r 获取 纸面 xy 坐标, 是 getTheta 的逆向过程.
void getXY(const double theta, double r, double &x, double &y) {
    r += 20.15 - 3.5;
    x = 198.5 - r * sin(theta / 180 * PI);
    y = 105 - r * cos(theta / 180 * PI);
}

/// Arduino 中的 map 函数的 double 版本.
///
/// 此函数不会检查任何边界与超界情况.
double map(const double x, const double l, const double r, const double a, const double b) {
    const double ratio = (x - l) / (r - l);
    return a + (b - a) * ratio;
}
