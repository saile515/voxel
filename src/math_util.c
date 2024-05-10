#include "math_util.h"

double clamp(double value, double min, double max) {
  return value < min ? min : value > max ? max : value;
};
