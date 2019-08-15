/**  \file Loads data from config.json and returns
    parameters for initializing the simulation.
*/
#pragma once

#include "json.hpp"
#include "levelset_constructors.hpp"
#include "simulation.hpp"
#include <fstream>
#include <iostream>
#include <string>
// for convenience
using json = nlohmann::json;

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

  // define the computational domain
  sim.init(sx, sy, h);

  // add each fluid
  for (auto tmp : j["fluids"].get<json>()) {
    sim.add_fluid(tmp["density"].get<float>());
    std::string fluid_name = tmp["name"].get<std::string>();
    std::printf("adding %s...\n", fluid_name.c_str());
    //  then initialize phi
    FluidConfig fconf(tmp["phi"].get<json>());
    construct_levelset(sim.fluids.back(), sx, sy, h, fluid_name, fconf);
  }
}