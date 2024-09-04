//
// Created by azazo1 on 2024/9/4.
//

#ifndef CONVERSION_H
#define CONVERSION_H

constexpr double PAPER_Z = -44.65;

double getAlpha(double r, double z);

double getBeta(double r, double z);

double getTheta(double x, double y, double &r);

#endif //CONVERSION_H
