#ifndef VECTOR_H
#define VECTOR_H

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>

// Vector (mathematics and physics) implementation
template <std::size_t N>
struct Vector {
  std::array<double, N> values{};

  double& operator[](std::size_t i) {
    assert(i < N);
    return values[i];
  }

  const double& operator[](std::size_t i) const {
    assert(i < N);
    return values[i];
  }
};

template <std::size_t N>
std::ostream& operator<<(std::ostream& out, const Vector<N>& v) {
  std::cout << '[';

  for (std::size_t i{0}; i < N; ++i) {
    if (i != 0) out << ", ";
    out << v[i];
  }

  std::cout << ']';

  return out;
}

// Addition
template <std::size_t N>
Vector<N> operator+(const Vector<N>& lhv, const Vector<N>& rhv) {
  Vector<N> result{};

  for (std::size_t i{0}; i < N; ++i) {
    result[i] = lhv[i] + rhv[i];
  }

  return result;
}

// Substraction
template <std::size_t N>
Vector<N> operator-(const Vector<N>& lhv, const Vector<N>& rhv) {
  Vector<N> result{};

  for (std::size_t i{0}; i < N; ++i) {
    result[i] = lhv[i] - rhv[i];
  }

  return result;
}

// Left scalar multiplication
template <std::size_t N>
Vector<N> operator*(double lhd, const Vector<N>& rhv) {
  Vector<N> result{};

  for (std::size_t i{0}; i < N; ++i) {
    result[i] = lhd * rhv[i];
  }

  return result;
}

// Right scalar multiplication
template <std::size_t N>
Vector<N> operator*(const Vector<N>& lhv, double rhd) {
  Vector<N> result{};

  for (std::size_t i{0}; i < N; ++i) {
    result[i] = lhv[i] * rhd;
  }

  return result;
}

// Dot product (algebraic)
template <std::size_t N>
double operator*(const Vector<N>& lhv, const Vector<N>& rhv) {
  double result{};

  for (std::size_t i{0}; i < N; ++i) {
    result += lhv[i] * rhv[i];
  }

  return result;
}

#endif
