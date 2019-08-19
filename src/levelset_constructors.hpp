#pragma once
#include "fluid.hpp"
#include "json.hpp"
#include "simulation.hpp"
#include <glm/gtc/random.hpp>
#include <string>

using json = nlohmann::json;

/** \class FluidConfig
 * A container for a fluid's initial phi conditions.
 * - a string which lets the user choose the shape
 * - three scalar values which describe its transformations
 * - a boolean which negates the phi values
 *
 *  supported shapes are: circle, plane */
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
float compute_phi_sphere(vec2 p, FluidConfig &fconf) {
  return distance(p, vec2(fconf.p1, fconf.p2)) - fconf.p3;
}

/* returns the distance a point is ABOVE a plane.
 * p1 is the planes height
 * p2 is the jitter quantity
 */
float compute_phi_plane(vec2 p, FluidConfig &fconf) { return p.y - fconf.p1 + linearRand(-fconf.p2, fconf.p2); }

/** TODO - add documentation and more level set starting configurations */
void construct_levelset(Fluid &f, int sx, int sy, float h, std::string name,
                        FluidConfig fconf) {
  f.phi.clear();

  for (auto it = f.phi.begin(); it != f.phi.end(); it++) {
    vec2 ij = it.ij();
    vec2 scaled_position =
        vec2((static_cast<float>(ij.x) + 0.5f) / static_cast<float>(sx),
             (static_cast<float>(ij.y) + 0.5f) / static_cast<float>(sy));
    float phi_value = 0;

    if (fconf.name == "circle") {
      phi_value = compute_phi_sphere(scaled_position, fconf);
    } else if (fconf.name == "plane") {
      phi_value = compute_phi_plane(scaled_position, fconf);
    } else {
      throw;
    }
    f.phi(ij) = fconf.negate ? -phi_value : phi_value;
  }
}

/** returns the distance from a point to a bounding box */
float distance_to_bounds(vec2 position, vec2 lower_bounds, vec2 upper_bounds) {
  float dx =
      min(abs(lower_bounds.x - position.x), abs(position.x - upper_bounds.x));
  float dy =
      min(abs(lower_bounds.y - position.y), abs(position.y - upper_bounds.y));
  return sqrt(dx * dx + dy * dy);
}

/** Sets the phi value at any point to be no more than the negative distance to
 * the nearest wall */
void fix_levelset_walls(std::vector<Fluid> &fluids, vec2 lower_bounds,
                        vec2 upper_bounds) {
  for (auto &f : fluids) {
    for (auto it = f.phi.begin(); it != f.phi.end(); it++) {
      float box_distance =
          distance_to_bounds(it.wp(), lower_bounds, upper_bounds);
      *it = max(-box_distance, *it);
    }
  }
}