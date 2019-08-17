#include "fluid.hpp"
#include <fstream>
#include <vector>
// TODO
#include <iostream>
#include <stdarg.h>

/** we make use of the fact that by our projection method, only one fluid at any
 * point has a negative phi value. so if we only include negative phi values, we
 * are guaranteed both no overlaps (because at most 1 is negative) and no gaps
 * (because we will never have no gaps).
 * */
void export_fluid_ids(std::vector<Fluid> &fluids, float time,
                      int frame_number) {
  // std::ofstream fluid_id_file;

  // fluid_id_file.open("plot/data/phi.txt");
  std::fstream fluid_id_file("plot/data/phi.txt",
                             fluid_id_file.out | fluid_id_file.app);

  fluid_id_file << "#BLOCK HEADER time:" << time << "\n";
  fluid_id_file << "#x\ty\tid\tphi\n";
  fluid_id_file << "\n";

  for (int n = 0; n < (int)fluids.size(); n++) {
    auto &f = fluids[n];
    for (auto it = f.phi.begin(); it != f.phi.end(); it++) {
      if (*it > 0)
        continue;
      vec2 ij = it.ij();
      vec2 wp = f.phi.worldspace_of(ij);
      fluid_id_file << wp.x << "\t" << wp.y << "\t" << *it << "\t" << n << "\n";
    }
  }
  fluid_id_file.close();
}

void export_simulation_data(std::vector<Fluid> &sim, float time,
                            int frame_number) {
  export_fluid_ids(sim, time, frame_number);
}

void clear_exported_data() {
  std::ofstream phi_file;
  phi_file.open("plot/data/phi.txt");
  phi_file << "# CLEAR\n";
  phi_file.close();
}