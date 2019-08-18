#define CATCH_CONFIG_MAIN
// #include "calculus.hpp"
#include "catch.hpp"
#include "settings.hpp"

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

  // test that our iterator allows modification
  phi.clear();
  for (auto it = phi.begin(); it != phi.end(); it++) {
    *it = 50;
  }
  REQUIRE(phi.min() == 50);
  REQUIRE(phi.max() == 50);

  new_phi.set(0);
  phi.set(5);

  phi = new_phi;
  REQUIRE(phi.max() == 0);
  new_phi.set(2);
  REQUIRE(phi.max() == 0);
  REQUIRE(new_phi.max() == 2);

  Array2f assign_phi(new_phi);
  REQUIRE(assign_phi.h == new_phi.h);

  // // TODO check that rk4 is the same as forward euler in 0 error conditions
  // Array2f u(10, 10, -0.5f, 0.f, 0.1f);
  // Array2f v(10, 10, 0.f, -0.5f, 0.1f);
  // VelocityField vel(&u, &v);
  // v.set(-9.8);
  // vec2 position(0.5, 0.5);
  // float dt = -0.01f;

  // REQUIRE(forward_euler(position, vel, dt) == rk4(position, vel, dt));
}

TEST_CASE("test creating a basic simulation") {
  Simulation sim;
  initialize_simulation(sim);
  sim.print_information();

  sim.run();
}