#pragma once
#include "array2.hpp"
#include "calculus.hpp"
#include "fluid.hpp"
#include <glm/gtc/random.hpp>

using namespace glm;

/** guarantees we will have no overlaps or gaps. Also checks that our fluids are
 * never defined within solid boundaries.
 *
 * currently adding reactions as an experimental feature
 * */
void project_phi(std::vector<Fluid> &fluids, Array2f &solid_phi, vec4 rxn) {
  assert(!fluids.empty());
  int number_grid_points = fluids[0].phi.size();
  for (int i = 0; i < (int)number_grid_points; i++) {
    float min1 = number_grid_points;
    float min2 = number_grid_points;
    int min1_index = -1;
    int min2_index = -1;
    for (int j = 0; j < (int)fluids.size(); j++) {
      auto &f = fluids[j];
      if (f.phi(i) < min1) {
        min2 = min1;
        min2_index = min1_index;
        min1 = f.phi(i);
        min1_index = j;
      } else if (f.phi(i) < min2) {
        min2 = f.phi(i);
        min2_index = j;
      }
    }

    assert(min1 != number_grid_points && min2 != number_grid_points);
    assert(min1_index != -1 && min2_index != -1);

    bool valid_reaction = (rxn.x >= 0 && rxn.y >= 0 && rxn.z >= 0 && rxn.w > 0);
    bool desired_reactants = ((min1_index == rxn[0] && min2_index == rxn[1]) ||
                              (min1_index == rxn[1] && min2_index == rxn[0]));
    bool overlap = (min1 < 0.35f * fluids[min1_index].phi.h && min2 < 0.35f * fluids[min2_index].phi.h);
    if (valid_reaction && desired_reactants && overlap) {
      auto &pf = fluids[rxn[2]];
      pf.phi(i) = min1 - 1.0f * pf.phi.h;
    }

    if (min1 * min2 > 0) {
      float avg = (min1 + min2) * 0.5f;
      for (auto &f : fluids) {
        f.phi(i) -= avg;
      }
    }
  }
}

/** Computes the gradient norm at each point using Godunov's scheme as described
 * in the osher and fedikew book */
Array2f gradient_norm(Array2f phi, Array2f sigmoid) {
  Array2f gradnorm(phi);
  gradnorm.set(1.0f);

  for (auto it = phi.begin(); it != phi.end(); it++) {
    vec2 ij = it.ij();
    if (ij.x < 1 || ij.x >= phi.sx - 1 || ij.y < 1 || ij.y >= phi.sy - 1)
      continue;
    float a = sigmoid(ij);
    float h = phi.h;

    float dx = 0;
    float dy = 0;

    float dxn = (phi(ij) - phi(ij - vec2(1, 0))) / h;
    float dxp = (phi(ij + vec2(1, 0)) - phi(ij)) / h;
    if (a >= 0) {
      dxn = (dxn > 0) ? dxn * dxn : 0;
      dxp = (dxp < 0) ? dxp * dxp : 0;
      dx = std::max(dxn, dxp);
    } else {
      dxn = (dxn < 0) ? dxn * dxn : 0;
      dxp = (dxp > 0) ? dxp * dxp : 0;
      dx = std::max(dxn, dxp);
    }

    float dyn = (phi(ij) - phi(ij - vec2(0, 1))) / h;
    float dyp = (phi(ij + vec2(0, 1)) - phi(ij)) / h;
    if (a >= 0) {
      dyn = (dyn > 0) ? dyn * dyn : 0;
      dyp = (dyp < 0) ? dyp * dyp : 0;
      dy = std::max(dyn, dyp);
    } else {
      dyn = (dyn < 0) ? dyn * dyn : 0;
      dyp = (dyp > 0) ? dyp * dyp : 0;
      dy = std::max(dyn, dyp);
    }

    gradnorm(ij) = sqrt(dx + dy);
  }
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

void reinitialize_phi(Fluid &f) {
  Array2f sigmoid = compute_sigmoid(f.phi);
  Array2f gradnorm = gradient_norm(f.phi, sigmoid);

  float err = 0;
  float tol = 1e-1;
  int max_iters = 250;
  float dt = 0.5f * f.phi.h;

  for (int iter = 0; iter <= max_iters; iter++) {
    // assert(iter != max_iters);
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
