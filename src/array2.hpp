#pragma once

#include <glm/glm.hpp>
#include <vector>

using namespace glm;

/** \class Array2
 * A 2d array template with a consistent
 * spatial indexing scheme for use in a collocated
 * grid. For translation between worldspace and gridspace,
 * we use a linear offset. The leastmost worldspace coordinate considered in our
 * universe is (0,0)
 */
template <class T> struct Array2 {
  int sx = 0;
  int sy = 0;
  float offset_x = 0;
  float offset_y = 0;
  float h = 0;
  std::vector<T> data;

  /** An empty contructor not intended to be used */
  Array2() {}

  /** Equip the grid with dimensions as well as an "offset" to foolproof
   * converting from worldspace to gridspace and back */
  Array2(int sx_, int sy_, float offset_x_, float offset_y_, float h_)
      : sx(sx_), sy(sy_), offset_x(offset_x_), offset_y(offset_y_), h(h_) {
    init();
  }

  void init(int sx_, int sy_, float offset_x_, float offset_y_, float h_) {
    sx = sx_;
    sy = sy_;
    offset_x = offset_x_;
    offset_y = offset_y_;
    h = h_;
    init();
  }

  /** FIXME creating new vectors might be causing memory leaks on resets
   * Creates a vector to store the grid's elements and clears every value to 0*/
  void init() {
    assert(sx != 0 && sy != 0);
    assert(h != 0);
    data = std::vector<T>(sx * sy);
    clear();
  }

  /** Fills the data vector with the input value */
  void set(T val) { std::fill(data.begin(), data.end(), val); }

  /** Fills the data vector with 0 casted to the template type */
  void clear() { set(static_cast<T>(0)); }

  /** Takes in x and y indice of the grid and returns the value stored at that
   * index. */
  T &operator()(int i, int j) {
    assert(i >= 0 and i < sx);
    assert(j >= 0 and j < sy);
    return data[i + (sx * j)];
  }

  /** Takes in a vec2 index of the grid and returns the value stored at that
   * index. */
  T &operator()(glm::ivec2 ij) { return (*this)(ij.x, ij.y); }

  /** Takes in some position in world coordinates and returns the *grid*
   * coordinates of that position. An example translation is that a
   * center-sampled (eg pressure) value would have offsets -0.5,-0.5   */
  vec2 coordinates_at(vec2 world_coordinates) {
    return vec2((world_coordinates.x / h) + offset_x,
                (world_coordinates.y / h) + offset_y);
  }
};

typedef Array2<double> Array2d;
typedef Array2<float> Array2f;
typedef Array2<int> Array2i;
