#define CATCH_CONFIG_MAIN
#include "array2.hpp"
#include "catch.hpp"
#include "export_data.hpp"
#include "settings.hpp"
#include "simulation.hpp"

TEST_CASE("this is a null test") { REQUIRE(1 == 1); }

TEST_CASE("testing basic array2 operations") {
  float h = 0.1;
  Array2i arri(10, 10, -0.5, -0.5, h); // eg pressure, phi

  vec2 world_coordinates(0.1, 0.1);

  // coordinates_at(position) takes a position and returns the grid's relative
  // coordinates
  REQUIRE(arri.coordinates_at(world_coordinates) == vec2(0.5, 0.5));
}

TEST_CASE("test creating a basic simulation") {
  Simulation sim;
  initialize_simulation(sim);
  sim.print_information();
  export_fluid_ids(sim);

  sim.run();
}