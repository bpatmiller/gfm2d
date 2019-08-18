#pragma once
#include "array2.hpp"
#include <glm/glm.hpp>

struct VelocityField {
  Array2f *up;
  Array2f *vp;
  VelocityField() {}
  VelocityField(Array2f *u_, Array2f *v_) : up(u_), vp(v_) {}
  vec2 operator()(glm::vec2 world_position) {
    return vec2(up->value_at(world_position), vp->value_at(world_position));
  }
};