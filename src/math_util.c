#include "math_util.h"

double clamp(double value, double min, double max) {
  return value < min ? min : value > max ? max : value;
};

unsigned int mod(int a, unsigned int b) { return (a % b + b) % b; }
