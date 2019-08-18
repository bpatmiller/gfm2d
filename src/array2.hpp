#pragma once

#include <algorithm>
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
public:
  int sx = 0;
  int sy = 0;
  float offset_x = 0;
  float offset_y = 0;
  float h = 0;
  std::vector<T> data;

  /** \Class Array2::iterator
   *  iterates through our data vecot
   *  call ij() on the iterate to get the index
   */
  class iterator : public std::vector<T>::iterator {
  public:
    Array2<T> const *owner;
    iterator(Array2<T> const *owner, typename std::vector<T>::iterator iter)
        : std::vector<T>::iterator(iter), owner(owner) {}
    using std::vector<T>::iterator::operator++;
    vec2 ij() { return owner->ij_from_index(*this - owner->data.begin()); }
    vec2 wp() { return owner->wp_from_index(*this - owner->data.begin()); }
  };
  iterator begin() { return iterator(this, data.begin()); }
  iterator end() { return iterator(this, data.end()); }

  int size() const { return sx * sy; }

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

  /** returns direct access to the data vector */
  T &operator()(int i) {
    assert(i >= 0 && i <= sx * sy);
    return data[i];
  }

  /** Takes in x and y indice of the grid and returns the value stored at that
   * index. */
  T &operator()(int i, int j) {
    assert(i >= 0 and i < sx);
    assert(j >= 0 and j < sy);
    return data[i + (sx * j)];
  }

  /** Takes in a vec2 index of the grid and returns the value stored at that
   * index. Note: this .can. beused with any position in grid coordinates */
  T &operator()(glm::vec2 const ij) { return (*this)(ij.x, ij.y); }

  /** Takes in some position in world coordinates and returns the *grid*
   * coordinates of that position. An example translation is that a
   * center-sampled (eg pressure) value would have offsets -0.5,-0.5   */
  vec2 coordinates_at(vec2 world_coordinates) {
    assert(!std::isnan(world_coordinates.x) &&
           !std::isnan(world_coordinates.y));
    assert(h != 0);
    float i = (world_coordinates.x / h) + offset_x;
    float j = (world_coordinates.y / h) + offset_y;
    if (i < 0)
      i = 0;
    if (j < 0)
      j = 0;
    if (i > sx - 1)
      i = sx - 1;
    if (j > sy - 1)
      j = sy - 1;
    assert(!std::isnan(i) && !std::isnan(j));
    return vec2(i, j);
  }

  /** Takes in grid coordinates (eg an index) and returns the worldspace
   * position */
  vec2 worldspace_of(vec2 grid_coordinates) const {
    assert(grid_coordinates.x >= 0 && grid_coordinates.x <= sx - 1);
    assert(grid_coordinates.y >= 0 && grid_coordinates.y <= sy - 1);
    return vec2((grid_coordinates.x - offset_x) * h,
                (grid_coordinates.y - offset_y) * h);
  }

  /** converts from a scalar index (indexing the data vector) to a vec2
   * with x and y coordinates */
  vec2 ij_from_index(int index) const {
    assert(index >= 0 && index < sx * sy);
    vec2 ij = vec2(index % sx, index / sx);
    assert(ij.x + (sx * ij.y) == index); // convert back
    return ij;
  }

  vec2 wp_from_index(int index) const {
    return worldspace_of(ij_from_index(index));
  }

  // TODO rearrange this code
  T max() { return *std::max_element(data.begin(), data.end()); }
  T min() { return *std::min_element(data.begin(), data.end()); }
  T infnorm() {
    T best_val = 0;
    for (auto &d : data) {
      if (std::abs(d) > best_val)
        best_val = std::abs(d);
    }
    return best_val;
  }

  inline T lerp(T val1, T val2, float f) {
    return (1.0f - f) * val1 + f * val2;
  }

  inline T lerp_2(T val00, T val10, T val01, T val11, vec2 f) {
    return lerp(lerp(val00, val10, f.x), lerp(val01, val11, f.x), f.y);
  }

  inline int index_from_ij(vec2 ij) { return ij.x + (sx * ij.y); }

  /** The same as (vec2) but it does not interpolate
   * deprecated but i like having it. Note: this forces the coordinates inbound
   */
  T snapped_access(vec2 ij) {
    ivec2 rounded(ij);
    int i = rounded.x;
    int j = rounded.y;
    if (i < 0)
      i = 0;
    if (j < 0)
      j = 0;
    if (i > sx - 1)
      i = sx - 1;
    if (j > sy - 1)
      j = sy - 1;
    int index = i + (sx * j);
    assert(index >= 0);
    assert(index < sx * sy);
    return data[index];
  }

  vec2 subcell_coordinates(vec2 ij) {
    ivec2 rounded(ij);
    return ij - vec2(rounded);
  }

  /* bilerp takes in a location (in grid coordinates) and returns the
   * interpolated value at the coordinate*/
  T const bilerp(vec2 ij) {
    assert(!std::isnan(ij.x) &&
           !std::isnan(ij.y)); // added in the debugging process
    T val00 = snapped_access(ij);
    T val10 = snapped_access(ij + vec2(1, 0));
    T val01 = snapped_access(ij + vec2(0, 1));
    T val11 = snapped_access(ij + vec2(1, 1));
    return lerp_2(val00, val10, val01, val11, subcell_coordinates(ij));
  }

  T value_at(vec2 world_position) {
    return bilerp(coordinates_at(world_position));
  }
};

typedef Array2<double> Array2d;
typedef Array2<float> Array2f;
typedef Array2<int> Array2i;
