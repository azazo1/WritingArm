//
// Created by azazo1 on 2024/9/4.
//

#ifndef CONVERSION_H
#define CONVERSION_H

constexpr double PAPER_Z = -10;

double getAlpha(double r, double z);

double getBeta(double r, double z);

double getTheta(double x, double y, double &r);

void getXY(double theta, double r, double &x, double &y);

double map(double x, double l, double r, double a, double b);

#endif //CONVERSION_H
