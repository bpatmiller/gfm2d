/*! \file settings.h
    \brief Loads data from config.json and returns
    parameters for initializing the simulation.
*/

#include "json.hpp"
#include <fstream>
#include <iostream>

// for convenience
using json = nlohmann::json;

void print_settings() {
  std::ifstream i("config.json");
  json j;
  i >> j;

  std::cout << j["horizontal_cells"] << '\n';
}