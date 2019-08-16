#define CATCH_CONFIG_MAIN
#include "array2.hpp"
#include "catch.hpp"
#include "export_data.hpp"
#include "settings.hpp"
#include "simulation.hpp"

TEST_CASE("testing array2 operations") {
  float h = 0.1;
  Array2i arri(10, 10, -0.5, -0.5, h); // eg pressure, phi

  vec2 world_coordinates(0.1, 0.1);

  // coordinates_at(position) takes a position and returns the grid's relative
  // coordinates
  REQUIRE(arri.coordinates_at(world_coordinates) == vec2(0.5, 0.5));

  Array2f phi(20, 20, 0, 0, 1);
  Array2f new_phi(phi);
  phi.set(6);
  new_phi.set(0);
  REQUIRE(phi.max() != new_phi.max());
  phi = new_phi;
  REQUIRE(phi(5, 5) == new_phi(5, 5));
  phi(6, 7) = 10;
  REQUIRE(new_phi(6, 7) == 0);
}

TEST_CASE("test creating a basic simulation") {
  Simulation sim;
  initialize_simulation(sim);
  sim.print_information();
  export_fluid_ids(sim);

  sim.run();
}