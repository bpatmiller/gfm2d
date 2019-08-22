#include "simulation.hpp"
#include "export_data.hpp"
#include "levelset_methods.hpp"
#include "particle_levelset_method.hpp"
#include <eigen3/Eigen/IterativeLinearSolvers>
#include <eigen3/Eigen/SparseCore>

/**  Returns a timestep that ensures the simulation is stable */
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
  auto start_time = std::chrono::high_resolution_clock::now();
  // delete old datafiles, fix after initializing
  clear_exported_data();
  for (auto &f : fluids) {
    reinitialize_phi(f);
  }
  project_phi(fluids, solid_phi, vec4(-1));
  advance(std::min(cfl(), 1e-7f));
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      end_time - start_time);
  float ms = duration.count();
  // printf("[ %.2fs elapsed ] ", ms / 1000.f);
  // export_simulation_data(p, vel, fluids, time_elapsed, frame_number);
  while (time_elapsed < max_t) {
    frame_number += 1;
    if (time_elapsed + timestep > max_t)
      timestep = max_t - time_elapsed;
    // break the timestep up
    float t = 0;
    while (t < timestep) {
      float substep = cfl();
      if (t + substep > timestep)
        substep = timestep - t;
      advance(substep);
      t += substep;
      /* progress bar code */
      std::cout << "  ";
      int pos = 20 * (t / timestep);
      for (int b = 0; b < 20; ++b) {
        if (b < pos)
          std::cout << ".";
        else if (b == pos)
          std::cout << ">";
        else
          std::cout << " ";
      }
      std::cout << "   " << int((t / timestep) * 100.f) << "%\r";
      std::cout.flush();
      /* ^ consider refactoring this */
    }
    std::cout << "\r";
    std::cout.flush();
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    ms = duration.count();
    time_elapsed += timestep;
    printf("[ %3.2fs elapsed ] ", ms / 1000.f);
    export_simulation_data(p, vel, fluids, time_elapsed, frame_number);
  }
}

/* The central method in the Simulation class. This performs all of our
 * computations for a given timestep that it assumed to be safe. */
void Simulation::advance(float dt) {
  assert(dt > 0);
  for (auto &f : fluids) {
    advect_phi(u, v, f.phi, dt);
    advect_particles(f, vel, solid_phi, dt);
    correct_levelset(f);
    reinitialize_phi(f);
    correct_levelset(f);
    adjust_particle_radii(f);
    if (reseed_counter++ % 5 == 0)
      reseed_particles(f, solid_phi);
  }
  project_phi(fluids, solid_phi, rxn);

  advect_velocity(dt);
  add_gravity(dt);

  float max_velocity = 200.f;
  u.clamp(-max_velocity, max_velocity);
  v.clamp(-max_velocity, max_velocity);
  enforce_boundaries();
  solve_pressure(dt);
  apply_pressure_gradient(dt);
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
float Simulation::sample_density(vec2 ij, vec2 kl) {
  if (fluid_id(ij) == fluid_id(kl)) {
    return 1.f / fluids[fluid_id(ij)].density;
  } else {
    int ij_id = fluid_id(ij);
    int kl_id = fluid_id(kl);
    float ij_phi = fluids[ij_id].phi(ij);
    float kl_phi = fluids[kl_id].phi(kl);
    float b_minus = 1.f / fluids[ij_id].density;
    float b_plus = 1.f / fluids[kl_id].density;
    float theta = abs(ij_phi) / (abs(ij_phi) + abs(kl_phi));
    return (b_minus * b_plus) / (theta * b_plus + (1.f - theta) * b_minus);
  }
}

/** Assembles a varying coefficient matrix for the possion equation. The lhs is
 * discretized as in eqn. 77 in liu et all */
Eigen::SparseMatrix<double>
Simulation::assemble_poisson_coefficient_matrix(Array2i fluid_cell_count,
                                                int nf) {
  std::vector<Eigen::Triplet<double>> coefficients;
  for (int it = 0; it < p.size(); it++) {
    if (solid_phi(it) <= 0)
      continue;
    vec2 ij = p.ij_from_index(it);
    float scale = 1.f / (h * h);
    int center_index = fluid_cell_count(ij);
    float center_coefficient = 0;

    /* loop through all four neighboring cells */
    for (auto offset : {vec2(1, 0), vec2(-1, 0), vec2(0, 1), vec2(0, -1)}) {
      vec2 neighbor_position = ij + offset;
      int neighbor_index = fluid_cell_count(neighbor_position);
      if (neighbor_index >= 0) {
        float b_hat = sample_density(ij, neighbor_position);
        float neighbor_coefficient = scale * b_hat;
        center_coefficient -= scale * b_hat;
        coefficients.push_back(Eigen::Triplet<double>(
            center_index, neighbor_index, neighbor_coefficient));
      }
    }
    coefficients.push_back(
        Eigen::Triplet<double>(center_index, center_index, center_coefficient));
  }

  Eigen::SparseMatrix<double> A(nf, nf);
  A.setFromTriplets(coefficients.begin(), coefficients.end());
  return A;
}

/** Sets up a linear system Ax=b to solve the discrete poission equation with
 * varying coefficients.
 */
void Simulation::solve_pressure(float dt) {
  /* Count each fluid cell, and find which voxels contain which fluids */
  get_fluid_ids();
  Array2i fluid_cell_count = count_fluid_cells();
  int nf = fluid_cell_count.max() + 1; // number of fluid cells

  /* Compute the discrete divergence of each fluid cell */
  Eigen::VectorXd rhs(nf);
  for (int i = 0; i < solid_phi.size(); i++) {
    if (solid_phi(i) <= 0)
      continue;
    vec2 ij = solid_phi.ij_from_index(i);
    rhs(fluid_cell_count(i)) = (1.f / (h * dt)) * (u(ij + vec2(1, 0)) - u(ij) +
                                                   v(ij + vec2(0, 1)) - v(ij));
  }

  /* Assemble the coefficient matrix */
  Eigen::SparseMatrix<double> A =
      assemble_poisson_coefficient_matrix(fluid_cell_count, nf);

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
void Simulation::apply_pressure_gradient(float dt) {
  for (auto it = u.begin(); it != u.end(); it++) {
    vec2 ij = it.ij();
    if (ij.x < 1 || ij.x >= u.sx - 1)
      continue;
    if (solid_phi(ij) <= 0 || solid_phi(ij - vec2(1, 0)) <= 0)
      continue;
    float du = sample_density(ij, ij - vec2(1, 0)) * (dt / h) *
               (p(ij) - p(ij - vec2(1, 0)));
    u(ij) -= du;
  }

  for (auto it = v.begin(); it != v.end(); it++) {
    vec2 ij = it.ij();
    if (ij.y < 1 || ij.y >= v.sy - 1)
      continue;
    if (solid_phi(ij) <= 0 || solid_phi(ij - vec2(0, 1)) <= 0)
      continue;
    float dv = sample_density(ij, ij - vec2(0, 1)) * (dt / h) *
               (p(ij) - p(ij - vec2(0, 1)));
    v(ij) -= dv;
  }
}
