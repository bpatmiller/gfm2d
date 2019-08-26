#pragma once
/**  \file Loads data from config.json and returns
    parameters for initializing the simulation.
*/

#include <nlohmann/json.hpp>
#include "levelset_constructors.hpp"
#include "simulation.hpp"
#include <fstream>
#include <iostream>
#include <string>

// for convenience
using json = nlohmann::json;

void initialize_boundaries(Simulation &sim) {
  sim.solid_phi.set(0.5f * sim.solid_phi.h);
  for (int i = 0; i < sim.sx; i++) {
    sim.solid_phi(i, 0) = -0.5;
    sim.solid_phi(i, sim.sy - 1) = -0.5;
  }
  for (int j = 0; j < sim.sy; j++) {
    sim.solid_phi(0, j) = -0.5;
    sim.solid_phi(sim.sx - 1, j) = -0.5;
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

  // load the reactions TODO handle multiple
  auto rxn_json = j["reaction"].get<json>();
  sim.rxn =
      vec4(rxn_json["reactant1"].get<int>() - 1,
           rxn_json["reactant2"].get<int>() - 1,
           rxn_json["product"].get<int>() - 1, rxn_json["rate"].get<float>());

  // add each fluid
  for (auto tmp : j["fluids"].get<json>()) {
    sim.add_fluid(tmp["density"].get<float>());
    std::string fluid_name = tmp["name"].get<std::string>();
    std::printf("~~ adding %s...\n", fluid_name.c_str());
    //  then initialize phi
    std::vector<FluidConfig> fluid_phis;
    for (auto p : tmp["phi"].get<json>()) {
      fluid_phis.push_back(FluidConfig(p.get<json>()));
      fluid_phis.back().print_information();
    }
    construct_levelset(sim.fluids.back(), sx, sy, h, fluid_name, fluid_phis);
  }
  initialize_boundaries(sim);
  fix_levelset_walls(sim.fluids, vec2(0, 0), vec2(sx * h, sy * h));
}