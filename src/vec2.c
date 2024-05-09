#include "vec2.h"

#include <math.h>

void vec2_normalize(Vec2 out, const Vec2 vector) {
  double magnitude = sqrt(pow(vector[0], 2) + pow(vector[1], 2));

  if (magnitude == 0) {
    return;
  }

  out[0] = vector[0] / magnitude;
  out[1] = vector[1] / magnitude;
}

double vec2_length(const Vec2 vector) {
  return sqrt(pow(vector[0], 2) + pow(vector[1], 2));
}
