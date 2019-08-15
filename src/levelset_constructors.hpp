#pragma once
#include "json.hpp"
#include "simulation.hpp"
#include <string>

using json = nlohmann::json;

/** \class FluidConfig
 * A container for a fluid's initial phi conditions.
 * - a string which lets the user choose the shape
 * - three scalar values which describe its transformations
 * - a boolean which negates the phi values
 *
 *  supported shapes are: circle */
struct FluidConfig {
  std::string name;
  float p1;
  float p2;
  float p3;
  bool negate;

  FluidConfig(json j) {
    name = j["shape"].get<std::string>();
    p1 = j["property1"].get<float>();
    p2 = j["property2"].get<float>();
    p3 = j["property3"].get<float>();
    negate = j["negate"].get<bool>();
  };
};

/** TODO */
void construct_levelset(Fluid &f, int sx, int sy, float h, std::string name,
                        FluidConfig fconf) {
  f.phi.clear();

  // for (auto i : irange(1, 10)) {
  //   std::cout << "10 loop\n";
  // }
}
