#include "fluid.hpp"
#include "simulation.hpp"
#include <fstream>
#include <iostream>
#include <vector>

/* exports velocities sampled at the voxel centers */
void export_velocity(VelocityField &vel, Array2f phi, float time,
                     int frame_number) {
  std::fstream vel_file("plot/data/vel.txt", vel_file.out | vel_file.app);

  vel_file << "#BLOCK HEADER time:" << time << "\n";
  vel_file << "#x\ty\tu\tv\n";
  vel_file << "\n";

  for (auto it = phi.begin(); it != phi.end(); it++) {
    vec2 wp = it.wp();
    vec2 velocity = vel(wp);
    vel_file << wp.x << "\t" << wp.y << "\t" << velocity.x << "\t" << velocity.y
             << "\n";
  }
}

/** we make use of the fact that by our projection method, only one fluid at any
 * point has a negative phi value. so if we only include negative phi values, we
 * are guaranteed both no overlaps (because at most 1 is negative) and no gaps
 * (because we will never have no gaps).
 * */
void export_fluid_ids(Array2f &p, std::vector<Fluid> &fluids, float time,
                      int frame_number) {
  std::fstream fluid_id_file("plot/data/phi.txt",
                             fluid_id_file.out | fluid_id_file.app);

  fluid_id_file << "#BLOCK HEADER time:" << time << "\n";
  fluid_id_file << "#x\ty\tphi\tid\tpressure\n";
  fluid_id_file << "\n";

  for (int n = 0; n < (int)fluids.size(); n++) {
    auto &f = fluids[n];
    for (auto it = f.phi.begin(); it != f.phi.end(); it++) {
      if (*it > 0)
        continue;
      vec2 ij = it.ij();
      vec2 wp = f.phi.worldspace_of(ij);
      fluid_id_file << wp.x << "\t" << wp.y << "\t" << *it << "\t" << n << "\t"
                    << p(ij) << "\n";
    }
  }
  fluid_id_file.close();
}

void export_simulation_data(Array2f &p, VelocityField vel,
                            std::vector<Fluid> &sim, float time,
                            int frame_number) {
  std::printf("exporting frame %i at time %f\n", frame_number, time);
  export_fluid_ids(p, sim, time, frame_number);
  export_velocity(vel, sim[0].phi, time, frame_number);
}

void clear_exported_data() {
  std::ofstream phi_file;
  phi_file.open("plot/data/phi.txt");
  phi_file << "# BEGIN PHI DATASET\n";
  phi_file.close();

  std::ofstream vel_file;
  phi_file.open("plot/data/vel.txt");
  phi_file << "# BEGIN VELOCITY DATASET\n";
  phi_file.close();
}