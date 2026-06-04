#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>

template <std::size_t N>
struct Vec {
  std::array<double, N> data{};

  double& operator[](std::size_t i) {
    assert(i < N);
    return data[i];
  }

  const double& operator[](std::size_t i) const {
    assert(i < N);
    return data[i];
  }
};

template <std::size_t N>
std::ostream& operator<<(std::ostream& out, const Vec<N>& v) {
  for (std::size_t i{0}; i < N; ++i) {
    if (i != 0) out << ' ';
    out << v[i];
  }

  return out;
}

#endif
