/*! \file array2.h
    \brief a 2d array template with a consistent
    spatial indexing scheme for use in a collocated
    grid
*/
#pragma once

#include <glm/glm.hpp>
#include <vector>

template <class T> struct Array2 {
  int sx = 0;
  int sy = 0;
  int size = 0;
  std::vector<T> data;

  Array2() {}

  Array2(int sx_, int sy_) : sx(sx_), sy(sy_) { init(); }

  void init() {
    size = sx * sy;
    data = std::vector<T>(size);
    clear();
  }

  voic clear() { set(static_cast<T>(0)); }

  void set(T val) { std::fill(data.begin(), data.end(), val); }
}
