/** \file test_calculus.hpp
 *
 * TODO: figure out include errors so i dont have to copy over source code */

#include "catch.hpp"

#include "velocityfield.hpp"
using namespace glm;

float test_distance_to_bounds(vec2 position, vec2 lower_bounds,
                              vec2 upper_bounds) {
  float dx =
      min(abs(lower_bounds.x - position.x), abs(position.x - upper_bounds.x));
  float dy =
      min(abs(lower_bounds.y - position.y), abs(position.y - upper_bounds.y));
  return sqrt(dx * dx + dy * dy);
}

/** "Classic" 4th order Runge-Kutta integration */
vec2 test_rk4(vec2 position, VelocityField &vel, float dt) {
  float l1 = vel(position).x * dt;
  float l2 = dt * vel(vec2(position.x + 0.5f * l1, position.y + 0.5f * dt)).x;
  float l3 = dt * vel(vec2(position.x + 0.5f * l2, position.y + 0.5f * dt)).x;
  float l4 = dt * vel(vec2(position.x + l3, position.y * dt)).x;
  float x = position.x + (1.f / 6.f) * (l1 + 2.f * l2 + 2.f * l3 + l4);

  float k1 = vel(position).y * dt;
  float k2 = dt * vel(vec2(position.x + 0.5f * dt, position.y + 0.5f * k1)).y;
  float k3 = dt * vel(vec2(position.x + 0.5f * dt, position.y + 0.5f * k2)).y;
  float k4 = dt * vel(vec2(position.x * dt, position.y + k3)).y;
  float y = position.y + (1.f / 6.f) * (k1 + 2.f * k2 + 2.f * k3 + k4);

  return vec2(x, y);
}

/** Forward euler integration, only for testing purposes */
vec2 test_forward_euler(vec2 position, VelocityField &vel, float dt) {
  return position + (vel(position) * dt);
}

TEST_CASE("testing calculus operations") {

  // // TODO check that rk4 is the same as forward euler in 0 error conditions
  Array2f u(10, 10, -0.5f, 0.f, 0.1f);
  Array2f v(10, 10, 0.f, -0.5f, 0.1f);
  VelocityField vel(&u, &v);
  u.set(0);
  v.set(-9.8);
  vec2 position(0.5, 0.5);
  float dt = 0.01f;

  vec2 fe = test_forward_euler(position, vel, dt);
  vec2 rk = test_rk4(position, vel, dt);

  REQUIRE(fe.x == rk.x);
  REQUIRE(fe.y == rk.y);
  REQUIRE(1 == 1);
}

TEST_CASE("testing boundary phi") {
  vec2 lower_bounds(0.f, 0.f);
  vec2 upper_bounds(1.f, 1.f);
  vec2 p1(0.f, 0.5f);
  vec2 p2(0.5f, 0.5f);

  REQUIRE(test_distance_to_bounds(p1, lower_bounds, upper_bounds) == 0.5f);
  REQUIRE(test_distance_to_bounds(p2, lower_bounds, upper_bounds) ==
          sqrt(0.5f * 0.5f + 0.5f * 0.5f));
}