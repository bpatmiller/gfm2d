#include "simulation.hpp"
#include "export_data.hpp"
#include "levelset_methods.hpp"
#include "particle_levelset_method.hpp"
#include <eigen3/Eigen/IterativeLinearSolvers>
#include <eigen3/Eigen/SparseCore>

/** TODO Returns a timestep that ensures the simulation is stable */
float Simulation::cfl() {
  float reciprocal = (u.infnorm() + v.infnorm()) / h;
  return 1.0 / reciprocal;
}

/** Runs the main simulation loop. Exports simulation data at each timestep,
 * and breaks the timestep into substeps based on the CFL condition
 * time_elapsed - current runtime of the simulation
 * max_t        - total amount of time the simulation will run
 * timestep     - amount of time between "frames"
 * t            - tracks the amount of time traversed in a given frame
 * substep      - a length of time given by cfl() */
void Simulation::run() {
  // delete old datafiles, fix after initializing
  clear_exported_data();
  reinitialize_phi(fluids);
  project_phi(fluids, solid_phi);
  while (time_elapsed < max_t) {
    export_simulation_data(vel, fluids, time_elapsed, frame_number);
    frame_number += 1;
    if (time_elapsed + timestep > max_t)
      timestep = max_t - time_elapsed;
    // break the timestep up
    float t = 0;
    while (t < timestep) {
      float substep = std::min(cfl(), timestep / 10.0f);
      if (t + substep > timestep)
        substep = timestep - t;
      advance(substep);
      t += substep;
    }
    time_elapsed += timestep;
  }
}

/* The central method in the Simulation class. This performs all of our
 * computations for a given timestep that it assumed to be safe. */
void Simulation::advance(float dt) {
  assert(dt > 0);
  for (auto &f : fluids) {
    advect_phi(u, v, f.phi, dt);
    advect_particles(f, vel, solid_phi, dt);
    // correct_levelset();
    reinitialize_phi(fluids);
    // correct_levelset();
    // adjust_particle_radii();
    // reseed_particles();
  }
  project_phi(fluids, solid_phi);

  advect_velocity(dt);
  add_gravity(dt);

  enforce_boundaries();
  solve_pressure(dt);
  // apply_pressure_gradient(dt);
}

void Simulation::get_fluid_ids() {
  Array2f min_phi(sx, sy, -0.5, -0.5, h);
  min_phi.set(99999.9);

  for (uint n = 0; n < fluids.size(); n++) {
    auto &f = fluids[n];
    for (auto it = fluid_id.begin(); it != fluid_id.end(); it++) {
      vec2 ij = it.ij();
      if (f.phi(ij) < min_phi(ij)) {
        min_phi(ij) = f.phi(ij);
        fluid_id(ij) = n;
      }
    }
  }
}

void Simulation::add_gravity(float dt) {
  for (auto &face : v.data) {
    face -= 9.8 * dt;
  }
}

void Simulation::advect_velocity(float dt) {
  Array2f new_u(u);
  Array2f new_v(v);

  for (auto it = u.begin(); it != u.end(); it++) {
    vec2 new_position = rk4(it.wp(), vel, -dt);
    *it = u.value_at(new_position);
  }

  for (auto it = v.begin(); it != v.end(); it++) {
    vec2 new_position = rk4(it.wp(), vel, -dt);
    *it = v.value_at(new_position);
  }

  u = new_u;
  v = new_v;
}

/** Sets the velocity on solid boundaries to 0 so that fluids do not flow in or
 * out of solids */
void Simulation::enforce_boundaries() {
  for (auto it = solid_phi.begin(); it != solid_phi.end(); it++) {
    if (*it < 0) {
      vec2 ij = it.ij();
      u(ij) = 0;
      u(ij + vec2(1, 0)) = 0;
      v(ij) = 0;
      v(ij + vec2(0, 1)) = 0;
    }
  }
}

/* Returns an int array which gives each fluid cell a corresponding nonnegative
 * integer index. Nonfluid cells are marked with a -1 */
Array2i Simulation::count_fluid_cells() {
  Array2i fluid_cell_count(sx, sy, -0.5, -0.5, h);
  fluid_cell_count.set(-1);
  int counter = 0;
  for (int i = 0; i < fluid_cell_count.size(); i++) {
    if (solid_phi(i) <= 0)
      continue;
    fluid_cell_count(i) = counter++;
  }
  assert(counter > 0);
  return fluid_cell_count;
}

/** Returns the density between two voxels, either as naively expected in the
 * case where the voxels contain the same fluid, or as defined in eqn. 55 in Liu
 * et al*/
float Simulation::sample_density(vec2 ij, vec2 kl) { return 0; }

/** Assembles a varying coefficient matrix for the possion equation. The lhs is
 * discretized as in eqn. 77 in liu et all */
Eigen::SparseMatrix<double>
Simulation::assemble_poisson_coefficient_matrix(Array2i fluid_cell_count,
                                                int nf) {
  std::vector<Eigen::Triplet<double>> coefficients;

  // assemble our coefficient matrix
  for (int it = 0; it < p.size(); it++) {
    if (solid_phi(it) <= 0)
      continue;
    // auto &f = fluids[fluid_id(it)]; // map from grid location to fluid type
    // vec2 ij = f.phi.ij_from_index(it);
    // int index =
    //     fluid_cell_count(it); // map from grid location to fluid cell index

    // float Fl = 0;
    // float Fr = 0;
    // float Fb = 0;
    // float Ft = 0;
    // // skeleton code for solution math
    // vec2 N = normalize(gradient(f.phi, ij));
  }

  Eigen::SparseMatrix<double> A(nf, nf);
  return A;
}

Eigen::VectorXd Simulation::assemble_poisson_rhs(Array2i fluid_cell_count,
                                                 int nf) {
  Eigen::VectorXd rhs(nf);
  return rhs;
}

/** Sets up a linear system Ax=b to solve the discrete poission equation with
 * varying coefficients and a discontinuous solution as in "A Boundary Condition
 * Capturing Method for Poisson's Equation on Irregular Domains" Liu et al
 */
void Simulation::solve_pressure(float dt) {
  /* Count each fluid cell, and find which voxels contain which fluids */
  get_fluid_ids();
  Array2i fluid_cell_count = count_fluid_cells();
  int nf = fluid_cell_count.max() + 1; // number of fluid cells

  /* Create our coefficint matrix and rhs to discretize the poission equation */
  Eigen::SparseMatrix<double> A =
      assemble_poisson_coefficient_matrix(fluid_cell_count, nf);
  Eigen::VectorXd rhs = assemble_poisson_rhs(fluid_cell_count, nf);

  /* Solve the linear system with the PCG method */
  Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;
  Eigen::VectorXd pressures(nf);
  solver.compute(A);
  pressures = solver.solve(rhs);

  /* Copy the new pressure values over */
  p.clear();
  for (int i = 0; i < p.size(); i++) {
    if (fluid_cell_count(i) < 0)
      continue;
    p(i) = pressures(fluid_cell_count(i));
  }
}

/** Applies the discrete pressure gradient using a similar method as how the
 * coefficient matrix in solve_pressure is constructed. */
void Simulation::apply_pressure_gradient(float dt) {}
