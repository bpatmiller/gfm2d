#pragma once
/**  \file Loads data from config.json and returns
    parameters for initializing the simulation.
*/

#include "json.hpp"
#include "levelset_constructors.hpp"
#include "simulation.hpp"
#include <fstream>
#include <iostream>
#include <string>
//
#include "levelset_methods.hpp"

// for convenience
using json = nlohmann::json;

void initialize_boundaries(Simulation &sim) {
  for (int i = 0; i < sim.sx; i++) {
    sim.solid_phi(i, 0) = -0.5;
    sim.solid_phi(i, sim.sy - 1) = -0.5;
  }
}

/** Initializes a simulation based on the json parameters,
 * then adds fluids and sets their starting phis.
 */
void initialize_simulation(Simulation &sim) {
  std::ifstream i("config.json");
  json j;
  i >> j;

  int sx = j["horizontal_cells"].get<int>();
  int sy = j["vertical_cells"].get<int>();
  float h = j["cell_size"].get<float>();
  float rt = j["runtime"].get<float>();
  float dt = j["timestep"].get<float>();

  // define the computational domain
  sim.init(sx, sy, h, rt, dt);

  // add each fluid
  for (auto tmp : j["fluids"].get<json>()) {
    sim.add_fluid(tmp["density"].get<float>());
    std::string fluid_name = tmp["name"].get<std::string>();
    std::printf("adding %s...\n", fluid_name.c_str());
    //  then initialize phi
    FluidConfig fconf(tmp["phi"].get<json>());
    construct_levelset(sim.fluids.back(), sx, sy, h, fluid_name, fconf);
  }
  initialize_boundaries(sim);
}