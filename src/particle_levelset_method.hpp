#pragma once
#include "calculus.hpp"
#include "simulation.hpp"
using namespace glm;

void advect_particles(Fluid &f, VelocityField &vel, Array2f &solid_phi,
                      float dt) {
  for (auto &p : f.particles) {
    p.position = rk4(p.position, vel, dt);
    if (solid_phi.value_at(p.position) < 0.0) {
      p.position -= solid_phi.value_at(p.position) *
                    interpolate_gradient(solid_phi, p.position);
    }
  }
}