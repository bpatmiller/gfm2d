#pragma once
#include "array2.hpp"
#include "iostream"
#include <glm/glm.hpp>
using namespace glm;

/** Note: this is intended for use with only integer indices */
vec2 upwind_gradient(Array2f &phi, vec2 velocity, vec2 ij) {
  if (ij.x < 1.0 || ij.x > phi.sx - 2.0 || ij.y < 0 || ij.y > phi.sy - 2.0)
    return vec2(0);
  float dx = velocity.x > 0 ? phi(ij) - phi(ij - vec2(1, 0))
                            : phi(ij + vec2(1, 0)) - phi(ij);
  float dy = velocity.y > 0 ? phi(ij) - phi(ij - vec2(0, 1))
                            : phi(ij + vec2(0, 1)) - phi(ij);
  return vec2(dx, dy) / phi.h;
}

vec2 rk3(vec2 position, VelocityField &vel, float dt) { return vec2(0); }

/** returns the central difference gradient of a point on a grid */
vec2 gradient(Array2f &field, vec2 ij) { return vec2(0); }

/** returns the interpolated central differenced gradient of a point in
 * worldspace */
vec2 interpolate_gradient(Array2f &field, vec2 world_position) {
  return vec2(0);
}

/** returns the central differenced divergence of a point on a grid */
float divergence(Array2f &field, vec2 ij) { return 0; }
