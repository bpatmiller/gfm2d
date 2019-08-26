#pragma once
#include "array2.hpp"
#include <algorithm>
#include <stdio.h>

/** \class Particle
 * A simple particle class for use in the particle level set method
 */
class Particle {
public:
  vec2 position;
  float starting_phi;
  float radius;
  bool valid;

  Particle(vec2 position_, float starting_phi_, float radius_)
      : position(position_), starting_phi(starting_phi_), radius(radius_) {
    valid = true;
  }
};

/** \class Fluid
 * each fluid has its own velocity, pressure, and level set which are
 * then composed by way of "ghost values"
 */
class Fluid {
public:
  float density;
  Array2f phi;            // phi, sampled at center
  Array2i particle_count; // counts how many particles are in that area, sampled
                          // in the same manner as pls_phi

  std::vector<Particle> particles;

  Fluid(float density_, int sx_, int sy_, float h) : density(density_) {
    phi.init(sx_, sy_, -0.5, -0.5, h);
    particle_count.init(sx_, sy_, 0.f, 0.f, h);
  }
  /*    */
  void print_information() {
    printf("~~ Fluid information ~~\n density: %.3f\n volume: ~%i%%\n", density,
           (int)(100.f *
                 (float)count_if(phi.data.begin(), phi.data.end(),
                                 [](float f) { return f < 0.f; }) /
                 (float)phi.size()));
  }
};