#ifndef VECTOR_H
#define VECTOR_H

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

#include "matrix.h"

template <std::size_t N, std::size_t M>
struct Matrix;

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

// Specializations for N = 2,3,4 replace the generic array-based layout with
// named members (x, y, z, w), enabling cleaner, semantically meaningful field
// access (v.x, v.y, v.z, v.w) without losing index-based access via operator[].
template <>
struct Vector<2> {
  double x{};
  double y{};

  double& operator[](std::size_t i) {
    assert(i < 2);
    return (i == 0 ? x : y);
  }

  const double& operator[](std::size_t i) const {
    assert(i < 2);
    return (i == 0 ? x : y);
  }
};

template <>
struct Vector<3> {
  double x{};
  double y{};
  double z{};

  double& operator[](std::size_t i) {
    assert(i < 3);
    return (i == 0 ? x : (i == 1 ? y : z));
  }

  const double& operator[](std::size_t i) const {
    assert(i < 3);
    return (i == 0 ? x : (i == 1 ? y : z));
  }
};

template <>
struct Vector<4> {
  double x{};
  double y{};
  double z{};
  double w{};

  double& operator[](std::size_t i) {
    assert(i < 4);
    return (i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)));
  }

  const double& operator[](std::size_t i) const {
    assert(i < 4);
    return (i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)));
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

// Left scalar division
template <std::size_t N>
Vector<N> operator/(double lhd, const Vector<N>& rhv) {
  Vector<N> result{};

  for (std::size_t i{0}; i < N; ++i) {
    result[i] = lhd / rhv[i];
  }

  return result;
}

// Right scalar division
template <std::size_t N>
Vector<N> operator/(const Vector<N>& lhv, double rhd) {
  Vector<N> result{};

  for (std::size_t i{0}; i < N; ++i) {
    result[i] = lhv[i] / rhd;
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

// Cross product
template <std::size_t N>
Vector<N> cross(const Vector<N>& lhv, const Vector<N>& rhv) {
  // clang-format off
  return {lhv[1] * rhv[2] - lhv[2] * rhv[1],
          lhv[2] * rhv[0] - lhv[0] * rhv[2],
          lhv[0] * rhv[1] - lhv[1] * rhv[0]};
  // clang-format on
}

// Norm
template <std::size_t N>
double norm(const Vector<N>& v) {
  return std::sqrt(v * v);
}

// Normalize vector
template <std::size_t N>
Vector<N> normalize(const Vector<N>& v) {
  return v / norm(v);
}

// Vector * Matrix
template <std::size_t N, std::size_t M>
Vector<M> operator*(const Vector<N>& lhv, const Matrix<N, M>& rhm) {
  Vector<N> result{};

  for (std::size_t i{0}; i < N; ++i) {
    result[i] = lhv * rhm[i];
  }

  return result;
}

// Matrix * Vector
template <std::size_t N, std::size_t M>
Vector<N> operator*(const Matrix<N, M>& lhm, const Vector<M>& rhv) {
  Vector<N> result{};

  for (std::size_t i{0}; i < N; ++i) {
    result[i] = lhm[i] * rhv;
  }

  return result;
}

#endif
