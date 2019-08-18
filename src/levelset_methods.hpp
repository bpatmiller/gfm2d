#pragma once
#include "array2.hpp"
#include "calculus.hpp"
#include "fluid.hpp"

using namespace glm;

/** guarantees we will have no overlaps or gaps */
void project_phi(std::vector<Fluid> &fluids) {
  assert(!fluids.empty());
  int number_grid_points = fluids[0].phi.size();
  for (int i = 0; i < number_grid_points; i++) {
    float min1 = number_grid_points;
    float min2 = number_grid_points;
    for (auto &f : fluids) {
      if (f.phi(i) < min1) {
        min2 = min1;
        min1 = f.phi(i);
      } else if (f.phi(i) < min2) {
        min2 = f.phi(i);
      }
    }
    assert(min1 != number_grid_points && min2 != number_grid_points);

    if (min1 * min2 > 0) {
      float avg = (min1 + min2) * 0.5;
      for (auto &f : fluids) {
        f.phi(i) -= avg;
      }
    }
  }
}

Array2f gradient_norm(Array2f phi, Array2f sigmoid) {
  Array2f gradnorm(phi);
  gradnorm.set(1.0f);

  // for (auto it = phi.begin(); it != phi.end(); it++) {
  //   vec2 ij = it.ij();
  // }
  return gradnorm;
}

Array2f compute_sigmoid(Array2f phi) {
  Array2f sigmoid(phi);
  sigmoid.clear();
  for (int i = 0; i < phi.size(); i++) {
    sigmoid(i) = phi(i) / sqrt(pow(phi(i), 2.0f) + pow(phi.h, 2.0f));
  }
  return sigmoid;
}

void reinitialize_phi(std::vector<Fluid> fluids) {
  for (auto &f : fluids) {
    Array2f sigmoid = compute_sigmoid(f.phi);
    Array2f gradnorm = gradient_norm(f.phi, sigmoid);

    float err = 0;
    float tol = 1e-1f;
    int max_iters = 50;
    float dt = 0.1f * f.phi.h;

    for (int iter = 0; iter < max_iters; iter++) {
      assert(iter != max_iters);
      // apply the update
      for (int i = 0; i < f.phi.size(); i++) {
        f.phi(i) -= sigmoid(i) * (gradnorm(i) - 1.0f) * dt;
      }
      // check updated error
      gradnorm = gradient_norm(f.phi, sigmoid);
      err = 0;
      for (int i = 0; i < f.phi.size(); i++) {
        err += abs(gradnorm(i) - 1.0f);
      }
      err /= static_cast<float>(f.phi.size());
      if (err < tol)
        break;
    }
  }
}

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
