#pragma once
#include "simulation.hpp"
#include "calculus.hpp"
using namespace glm;

void advect_particles(Fluid& f, VelocityField& vel, Array2f& solid_phi, float dt) {
    for (auto& p: f.particles) {
        p.position = rk3(p.position, vel, dt);
        if (solid_phi.value_at(p.position) < 0.0) {
            p.position -= solid_phi.value_at(p.position) * interpolate_gradient(solid_phi, p.position);
        }
    }
}
/*
// particle advection with bounds checking
void Simulation::advect_particles(Fluid &fluid, float dt) {
  for (auto &p : fluid.particles) {
    p.position = rk2(p.position, dt);
    // data for bounds checking
    glm::vec3 coords;
    glm::ivec3 index;
    position_to_grid(p.position, glm::vec3(0), index, coords);
    float solid_phi_val = solid_phi.trilerp(index, coords);
    // only if the particle is a NEGATIVE marker particle
    if (solid_phi_val < 0 && p.starting_phi < 0) {
      glm::vec3 grad = solid_phi.gradlerp(index, coords, h);
      if (glm::length(grad) > 0) {
        grad = glm::normalize(grad);
        p.position -= solid_phi_val * grad;
      }
    }
  }
}
*/