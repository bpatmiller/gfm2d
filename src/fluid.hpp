#pragma once
#include "array2.hpp"
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

  Particle(vec3 position_, float starting_phi_, float radius_)
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
  Array2f phi;     // phi, sampled at center
  Array2f pls_phi; // an artificial resampling of phi for use in the particle
                   // level set
  Array2i particle_count; // counts how many particles are in that area, sampled
                          // in the same manner as pls_phi

  std::vector<Particle> particles;

  Fluid(float density_, int sx_, int sy_, float h) : density(density_) {
    // center-located quantities
    phi.init(sx_, sy_, -0.5, -0.5, h);
    // vertex-located quantities
    pls_phi.init(sx_ - 1, sy_ - 1, -1.0, -1.0, h);
    particle_count.init(sx_ - 1, sy_ - 1, -1.0, -1.0, h);
  }
  /*    */
  void print_information() {
    printf("~~ Fluid information ~~\n density: %f\n", density);
  }
};