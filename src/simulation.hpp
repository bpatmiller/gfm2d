#pragma once
#include "array2.hpp"
#include <vector>

/* A simple particle class for use in the particle level set method
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

/* each fluid has its own velocity, pressure, and level set which are then
 * composed by way of "ghost values"
 */
class Fluid {
public:
  float density;
  Array2f u;   // horizontal velocity, sampled at cell sides
  Array2f v;   // vertical velocity, sampled at cell tops/bottoms
  Array2f p;   // pressure, sampled at center
  Array2f rho; // density, sampled at center but often used for boundaries (PLS)
  Array2f div; // divergence, sampled at center
  Array2f phi; // phi, sampled at center
  Array2f pls_phi; // an artificial resampling of phi for use in the particle
                   // level set
  Array2i particle_count; // counts how many particles are in that area, sampled
                          // in the same manner as pls_phi

  std::vector<Particle> particles;

  Fluid(float density_, int sx_, int sy_, float h) : density(density_) {
    // face-located quantities
    u.init(sx_ + 1, sy_, 0.0, -0.5, h);
    u.init(sx_, sy_ + 1, -0.5, 0.0, h);
    // center-located quantities
    p.init(sx_, sy_, -0.5, -0.5, h);
    rho.init(sx_, sy_, -0.5, -0.5, h);
    div.init(sx_, sy_, -0.5, -0.5, h);
    phi.init(sx_, sy_, -0.5, -0.5, h);
    // vertex-located quantities
    pls_phi.init(sx_ + 1, sy_ + 1, 0.0, 0.0, h);
    p.init(sx_ + 1, sy_ + 1, 0.0, 0.0, h);
  }
};

/* The main simulation class that defines our computational domain. It is
 * described spatially by a cell size (h) and a number of cells in both
 * dimensions (sx, sy).
 *
 */
class Simulation {
public:
  int sx;  // number of voxels on the x-axis
  int sy;  // number of voxels on the y-axis
  float h; // voxel size

  std::vector<Fluid> fluids;
  Array2f solid_phi; // phi corresponding to solid boundaries, not important as
                     // a SDF just to identify solids. Sampled at cell centers
  Array2i fluid_id;  // describes which fluid occupies a given voxel, sampled at
                     // cell centers.

  Simulation() {}
  Simulation(int sx_, int sy_, float h_) : sx(sx_), sy(sy_), h(h_) {}

  void init(int sx_, int sy_, float h_) {
    sx = sx_;
    sy = sy_;
    h = h_;
  }

  /* Creates a fluid of a given density, but does not equip it with a phi*/
  void add_fluid(float density) { fluids.push_back(Fluid(density, sx, sy, h)); }
};