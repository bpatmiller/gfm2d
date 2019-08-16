#pragma once
#include "simulation.hpp"
#include <fstream>
#include <iostream>

void export_fluid_ids(Simulation &sim) {
  sim.get_fluid_ids();

  std::ofstream fluid_id_file;
  fluid_id_file.open("plot/data/phi.txt");
  fluid_id_file << "#x\ty\tid\tphi\n";

  for (auto it = sim.fluid_id.begin(); it != sim.fluid_id.end(); it++) {
    vec2 ij = it.ij();
    vec2 wp = sim.fluid_id.worldspace_of(ij);
    fluid_id_file << wp.x << "\t" << wp.y << "\t" << *it << "\t"
                  << sim.fluids[*it].phi(ij) << "\n";
  }
  fluid_id_file.close();
}