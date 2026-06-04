#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cassert>
#include <cmath>
#include <iostream>

template <int N>
struct vec {
  double data[N] = {0};
  double& operator[](const int i) {
    assert(i >= 0 && i < N);
    return data[i];
  }
  double operator[](const int i) const {
    assert(i >= 0 && i < N);
    return data[i];
  }
};

template <int N>
std::ostream& operator<<(std::ostream& out, const vec<N>& v) {
  for (int i = 0; i < N; i++) out << v[i] << " ";
  return out;
}

template <>
struct vec<3> {
  double x = 0, y = 0, z = 0;
  double& operator[](const int i) {
    assert(i >= 0 && i < 3);
    return i ? (1 == i ? y : z) : x;
  }
  double operator[](const int i) const {
    assert(i >= 0 && i < 3);
    return i ? (1 == i ? y : z) : x;
  }
};

typedef vec<3> vec3;

#endif
