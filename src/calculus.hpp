#pragma once
#include "velocityfield.hpp"
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

/** "Classic" 4th order Runge-Kutta integration */
vec2 rk4(vec2 position, VelocityField &vel, float dt) {
  vec2 k1 = vel(position);
  vec2 k2 = vel(position + (dt * 0.5f) * k1);
  vec2 k3 = vel(position + (dt * 0.5f) * k2);
  vec2 k4 = vel(position + k3);
  return position + (1.0f / 6.0f) * (k1 + 2.0f * k2 + 2.0f * k3 + k4);
}

/** Forward euler integration, only for testing purposes */
vec2 forward_euler(vec2 position, VelocityField &vel, float dt) {
  return position + (vel(position) * dt);
}

/** returns the central difference gradient of a point on a grid */
vec2 gradient(Array2f &field, vec2 ij) {
  float dx = field(ij + vec2(1, 0)) - field(ij - vec2(1, 0));
  float dy = field(ij + vec2(0, 1)) - field(ij - vec2(0, 1));
  return vec2(dx, dy) / (2.0f * field.h);
}

vec2 bilerp(vec2 v00, vec2 v10, vec2 v01, vec2 v11, vec2 xy) {
  return ((1.0f - xy.x) * v00 + xy.x * v10) * (1.0f - xy.y) +
         ((1.0f - xy.x) * v01 + xy.x * v11) * (xy.y);
}

/** returns the interpolated central differenced gradient of a point in
 * worldspace */
vec2 interpolate_gradient(Array2f &field, vec2 world_position) {
  vec2 ij = field.coordinates_at(world_position);
  vec2 xy = field.subcell_coordinates(ij);

  vec2 g00 = gradient(field, ivec2(ij));
  vec2 g10 = gradient(field, ivec2(ij) + ivec2(1, 0));
  vec2 g01 = gradient(field, ivec2(ij) + ivec2(0, 1));
  vec2 g11 = gradient(field, ivec2(ij) + ivec2(1, 1));

  return bilerp(g00, g10, g01, g11, xy);
}
