#pragma once
#include "array2.hpp"
#include "calculus.hpp"

using namespace glm;

void advect_phi(Array2f &u, Array2f &v, Array2f &phi, float dt) {
  Array2f new_phi(phi);
  for (auto it = phi.begin(); it != phi.end(); it++) {
    vec2 ij = it.ij();
    vec2 world_position = phi.worldspace_of(ij);
    vec2 velocity(u.value_at(world_position), v.value_at(world_position));
    vec2 del_phi = upwind_gradient(phi, velocity, ij);
    new_phi(ij) = phi(ij) - dt * dot(velocity, del_phi);
  }
  phi = new_phi;
}
