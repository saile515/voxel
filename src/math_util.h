#pragma once

#include <math.h>

static const double DEG_TO_RAD = M_PI / 180;

double clamp(double value, double min, double max);

unsigned int mod(int a, unsigned int b);
