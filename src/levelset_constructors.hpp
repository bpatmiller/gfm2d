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

/* x1, x2 is a position in space (scaled from 0-1)
 * p1, p2 are the spheres center
 * p3 is the radius
 */
float compute_phi_sphere(float x1, float x2, FluidConfig &fconf) {
  return glm::distance(glm::vec2(x1, x2), glm::vec2(fconf.p1, fconf.p2)) -
         fconf.p3;
}

/** TODO - add documentation and more level set starting configurations */
void construct_levelset(Fluid &f, int sx, int sy, float h, std::string name,
                        FluidConfig fconf) {
  f.phi.clear();

  for (int x = 0; x < sx; x++) {
    for (int y = 0; y < sy; y++) {
      // TODO add conditionals for non-spheres
      float phi_value = compute_phi_sphere(
          (static_cast<float>(x) + 0.5f) / static_cast<float>(sx),
          (static_cast<float>(y) + 0.5f) / static_cast<float>(sy), fconf);
      f.phi(x, y) = fconf.negate ? -phi_value : phi_value;
    }
  }
}
