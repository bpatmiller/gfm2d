#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "array2.hpp"

TEST_CASE("this is a null test") {
  REQUIRE(1 == 1);
}

TEST_CASE("testing basic array2 operations") {
  float h = 0.1;
  Array2i arri(10, 10, -0.5, -0.5, h);  // eg pressure, phi

  vec2 world_coordinates(0.1,0.1);

  // coordinates_at(position) takes a position and returns the grid's relative coordinates
  REQUIRE( arri.coordinates_at(world_coordinates) == vec2(0.5,0.5) );
}