#pragma once
#include "calculus.hpp"
#include "simulation.hpp"
#include <glm/gtc/random.hpp>
using namespace glm;

// TODO remove solid phi
void reseed_particles(Fluid &f, Array2f solid_phi) {
  /* start by removing invalid particles */
  for (auto &p : f.particles) {
    float local_phi = f.phi.value_at(p.position);
    p.valid = (abs(local_phi) < 3.f * f.phi.h);
  }
  f.particles.erase(std::remove_if(f.particles.begin(), f.particles.end(),
                                   [](Particle const &p) { return !p.valid; }),
                    f.particles.end());

  /* count particles in voxels */
  f.particle_count.clear();
  for (auto &p : f.particles) {
    vec2 grid_coordinates = f.particle_count.coordinates_at(p.position);
    f.particle_count(grid_coordinates) += 1;
  }

  /* seed new particles to non-full voxels */
  for (int i = 0; i < f.phi.size(); i++) {
    // FIXME
    vec2 ij = f.phi.ij_from_index(i);
    if (abs(f.phi(i)) > 3.f * f.phi.h || ij.x < 2 || ij.y < 2 ||
        ij.x > f.phi.sx - 3 || ij.y > f.phi.sy - 3 || solid_phi(i) <= 0)
      continue;
    while (f.particle_count(i) < 16) {
      vec2 position = f.particle_count.wp_from_index(i) +
                      linearRand(vec2(0), vec2(f.phi.h));
      float initial_phi = f.phi.value_at(position);
      float goal_phi =
          (initial_phi > 0)
              ? clamp(initial_phi, 0.1f * f.phi.h, 1.0f * f.phi.h)
              : clamp(initial_phi, -1.0f * f.phi.h, -0.1f * f.phi.h);
      vec2 normal = normalize(interpolate_gradient(f.phi, position));
      vec2 new_position = position + (goal_phi - initial_phi) * normal;
      new_position =
          clamp(new_position, 2.001f * f.phi.h,
                (max((float)f.phi.sx, (float)f.phi.sy) - 2.001f) * f.phi.h);
      float new_phi = f.phi.value_at(new_position);
      float radius = clamp(abs(new_phi), 0.1f * f.phi.h, 0.5f * f.phi.h);
      f.particles.push_back(Particle(new_position, new_phi, radius));
      f.particle_count(i) += 1;
    }
  }
}

void adjust_particle_radii(Fluid &f) {
  for (auto &p : f.particles) {
    float local_phi = f.phi.value_at(p.position);
    p.radius = clamp(abs(local_phi), 0.1f * f.phi.h, 0.5f * f.phi.h);
  }
}

/** Correct a levelset using the particle level set method */
void correct_levelset(Fluid &f) {
  /* Compute phi+ and phi- */
  Array2f phi_minus(f.phi);
  Array2f phi_plus(f.phi);
  for (auto &p : f.particles) {
    float local_phi = f.phi.value_at(p.position);
    if (p.starting_phi * local_phi >= 0 || abs(local_phi) < p.radius)
      continue;
    float sign_p = (p.starting_phi > 0) ? 1.f : -1.f;
    vec2 grid_position = vec2(ivec2(f.phi.coordinates_at(p.position)));
    for (auto offset : {vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1)}) {
      float phi_p =
          sign_p *
          (p.radius -
           distance(p.position, f.phi.worldspace_of(grid_position + offset)));
      if (sign_p > 0) {
        phi_plus(grid_position + offset) =
            max(phi_p, phi_plus(grid_position + offset));
      } else {
        phi_minus(grid_position + offset) =
            min(phi_p, phi_minus(grid_position + offset));
      }
    }
  }
  /* Merge phi+ and phi- */
  for (int i = 0; i < f.phi.size(); i++) {
    if (abs(phi_plus(i)) >= abs(phi_minus(i))) {
      f.phi(i) = phi_minus(i);
    } else {
      f.phi(i) = phi_plus(i);
    }
  }
}

void advect_particles(Fluid &f, VelocityField &vel, Array2f &solid_phi,
                      float dt) {
  for (auto &p : f.particles) {
    p.position = rk4(p.position, vel, dt);
    if (solid_phi.value_at(p.position) < 0.0) {
      p.position -= solid_phi.value_at(p.position) *
                    interpolate_gradient(solid_phi, p.position);
    }
    p.position.x =
        clamp(p.position.x, solid_phi.h, (solid_phi.sx - 1.f) * solid_phi.h);
    p.position.y =
        clamp(p.position.y, solid_phi.h, (solid_phi.sy - 1.f) * solid_phi.h);
  }
}