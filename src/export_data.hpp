#pragma once
#include "simulation.hpp"
#include <fstream>
#include <iostream>

void export_fluid_ids(Simulation &sim) {
  sim.get_fluid_ids();

  std::ofstream fluid_id_file;
  fluid_id_file.open("plot/data/ids.txt");
  fluid_id_file << "Writing this to a file.\n";
  fluid_id_file.close();

  // for (auto it = sim.fluid_id.begin(); it != sim.fluid_id.end(); it++) {
  //     vec2 ij = it.ij();

  // }
}