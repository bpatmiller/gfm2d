#pragma once
#include "fluid.hpp"
#include "velocityfield.hpp"
#include <eigen3/Eigen/SparseCore>
#include <stdio.h>
#include <vector>
#include <chrono>

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

  float time_elapsed = 0; // amount of time elapsed
  float max_t = 0;        // full simulation runtime
  float timestep = 0;     // timestep per frame
  int frame_number = 0;   // current frame
  int reseed_counter = 0; // used for PLS

  Array2f u; // horizontal velocity, sampled at cell sides
  Array2f v; // vertical velocity, sampled at cell tops/bottoms
  Array2f p; // pressure, sampled at center
  VelocityField vel;

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
    vel.up = &u;
    vel.vp = &v;
  }

  void init(int sx_, int sy_, float h_, float max_t_, float dt_) {
    sx = sx_;
    sy = sy_;
    h = h_;
    max_t = max_t_;
    timestep = dt_;
    init();
  }

  /** Creates a fluid of a given density, but does not equip it with a phi*/
  void add_fluid(float density) { fluids.push_back(Fluid(density, sx, sy, h)); }

  void print_information() {
    printf("~~ Simulation information ~~\n sx: %i, sy: %i, h: %f\n no. "
           "fluids: %i\n",
           sx, sy, h, static_cast<int>(fluids.size()));
    for (auto f : fluids) {
      f.print_information();
    }
  }

  void run();
  void advance(float dt);

  /* SIMULATION METHODS */
  float cfl();
  void add_gravity(float dt);
  void advect_velocity(float dt);
  void enforce_boundaries();
  /* Methods specifically used for solving for pressure */
  void solve_pressure(float dt);
  void apply_pressure_gradient(float dt);
  float sample_density(vec2 ij, vec2 kl);
  Eigen::SparseMatrix<double>
  assemble_poisson_coefficient_matrix(Array2i fluid_cell_count, int nf);
  Array2i count_fluid_cells();
  void get_fluid_ids();
};