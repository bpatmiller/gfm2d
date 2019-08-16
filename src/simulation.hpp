#pragma once
#include "array2.hpp"
#include <stdio.h>
#include <vector>

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

/** \class Simulation
 * The main simulation class that defines our computational domain. It
 * is described spatially by a cell size (h) and a number of cells in both
 * dimensions (sx, sy).
 *
 * stores velocity and pressure,
 * a list of fluids contained in it,
 * a solid phi describing boundary locations
 * a map from voxels to fluid type
 */
class Simulation {
public:
  int sx = 0;  // number of voxels on the x-axis
  int sy = 0;  // number of voxels on the y-axis
  float h = 0; // voxel size

  Array2f u; // horizontal velocity, sampled at cell sides
  Array2f v; // vertical velocity, sampled at cell tops/bottoms
  Array2f p; // pressure, sampled at center

  std::vector<Fluid> fluids;
  Array2f solid_phi; // phi corresponding to solid boundaries, not important as
                     // a SDF just to identify solids. Sampled at cell centers
  Array2i fluid_id;  // describes which fluid occupies a given voxel, sampled at
                     // cell centers.

  Simulation() {}
  Simulation(int sx_, int sy_, float h_) : sx(sx_), sy(sy_), h(h_) {}

  void init() {
    assert(sx != 0 && sy != 0 && h != 0);
    // face-located quantities
    u.init(sx + 1, sy, 0.0, -0.5, h);
    v.init(sx, sy + 1, -0.5, 0.0, h);
    // center-located quantities
    p.init(sx, sy, -0.5, -0.5, h);
    solid_phi.init(sx, sy, -0.5, -0.5, h);
    fluid_id.init(sx, sy, -0.5, -0.5, h);
  }

  void init(int sx_, int sy_, float h_) {
    sx = sx_;
    sy = sy_;
    h = h_;
    init();
  }

  /** Creates a fluid of a given density, but does not equip it with a phi*/
  void add_fluid(float density) { fluids.push_back(Fluid(density, sx, sy, h)); }

  /*      */
  void print_information() {
    printf("~~ Simulation information ~~\n sx: %i, sy: %i, h: %f\n no. "
           "fluids: %i\n",
           sx, sy, h, static_cast<int>(fluids.size()));
    for (auto f : fluids) {
      f.print_information();
    }
  }

  /* SIMULATION METHODS */

  /** Populates the fluid_id array with the fluid that exists at each index in
   * space*/
  void get_fluid_ids();
};