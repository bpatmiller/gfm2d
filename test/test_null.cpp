#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("this is a null test") {
  REQUIRE(1 == 1);
  REQUIRE(2 == 2);
}
