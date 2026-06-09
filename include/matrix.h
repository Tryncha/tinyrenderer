#ifndef MATRIX_H
#define MATRIX_H

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

// Matrix implementation
template <std::size_t N, std::size_t M>
struct Matrix {
  std::array<std::array<double, M>, N> values{};

  std::array<double, M>& operator[](std::size_t i) {
    assert(i < N);
    return values[i];
  }

  const std::array<double, M>& operator[](std::size_t i) const {
    assert(i < N);
    return values[i];
  }
};

template <std::size_t N, std::size_t M>
std::ostream& operator<<(std::ostream& out, const Matrix<N, M>& m) {
  std::cout << '[';

  for (std::size_t i{0}; i < N; ++i) {
    if (i != 0) out << ", ";
    std::cout << '[';

    for (std::size_t j{0}; j < M; ++j) {
      if (j != 0) out << ", ";
      out << m[i][j];
    }

    std::cout << ']';
  }

  std::cout << ']';

  return out;
}

// Addition
template <std::size_t N, std::size_t M>
Matrix<N, M> operator+(const Matrix<N, M>& lhm, const Matrix<N, M>& rhm) {
  Matrix<N, M> result{};

  for (std::size_t i{0}; i < N; ++i) {
    for (std::size_t j{0}; j < M; ++j) {
      result[i][j] = lhm[i][j] + rhm[i][j];
    }
  }

  return result;
}

// Substraction
template <std::size_t N, std::size_t M>
Matrix<N, M> operator-(const Matrix<N, M>& lhm, const Matrix<N, M>& rhm) {
  Matrix<N, M> result{};

  for (std::size_t i{0}; i < N; ++i) {
    for (std::size_t j{0}; j < M; ++j) {
      result[i][j] = lhm[i][j] - rhm[i][j];
    }
  }

  return result;
}

// Left scalar multiplication
template <std::size_t N, std::size_t M>
Matrix<N, M> operator*(double lhd, const Matrix<N, M>& rhm) {
  Matrix<N, M> result{};

  for (std::size_t i{0}; i < N; ++i) {
    for (std::size_t j{0}; j < M; ++j) {
      result[i][j] = lhd * rhm[i][j];
    }
  }

  return result;
}

// Right scalar multiplication
template <std::size_t N, std::size_t M>
Matrix<N, M> operator*(const Matrix<N, M>& lhm, double rhd) {
  Matrix<N, M> result{};

  for (std::size_t i{0}; i < N; ++i) {
    for (std::size_t j{0}; j < M; ++j) {
      result[i][j] = lhm[i][j] * rhd;
    }
  }

  return result;
}

// Multiplication
template <std::size_t N, std::size_t M, std::size_t P>
Matrix<N, P> operator*(const Matrix<N, M>& lhm, const Matrix<M, P>& rhm) {
  Matrix<N, P> result{};

  for (std::size_t i{0}; i < N; ++i) {
    for (std::size_t j{0}; j < P; ++j) {
      for (std::size_t k{0}; k < M; ++k) {
        result[i][j] += lhm[i][k] * rhm[k][j];
      }
    }
  }

  return result;
}

// Transposition
template <std::size_t N, std::size_t M>
Matrix<M, N> transpose(const Matrix<N, M>& m) {
  Matrix<M, N> result{};

  for (std::size_t j{0}; j < M; ++j) {
    for (std::size_t i{0}; i < N; ++i) {
      result[j][i] = m[i][j];
    }
  }

  return result;
}

// Determinant using Laplace Expansion O(n!)
// Faster than Gaussian Elimination (gaussian_det) when N < 10
template <std::size_t N>
double laplace_det(const Matrix<N, N>& m) {
  // if constexpr: Branches are evaluated at compile time, preventing
  // instantiation of Matrix<0,0> when the recursion bottoms out

  // Base Case 1: 1x1 Matrix
  if constexpr (N == 1) {
    return m[0][0];
  }

  // Base Case 2: 2x2 Matrix (Optimization)
  else if constexpr (N == 2) {
    return (m[0][0] * m[1][1]) - (m[0][1] * m[1][0]);
  }

  else {
    double det{};
    int sign{1};

    // Expand along the first row
    for (std::size_t i{0}; i < N; ++i) {
      // Create submatrix of size (N - 1) x (N - 1)
      Matrix<N - 1, N - 1> submatrix{};

      for (std::size_t r{1}; r < N; ++r) {
        std::size_t col{};

        for (std::size_t c{0}; c < N; ++c) {
          // Skip the column of the current pivot
          if (c == i) continue;

          submatrix[r - 1][col] = m[r][c];
          ++col;
        }
      }

      // Alternate signs: +, -, +, -
      det += sign * m[0][i] * laplace_det(submatrix);
      sign = -sign;
    }

    return det;
  }
}

// Determinant using Gaussian Elimination O(n^3)
// Faster than Laplace Expansion (laplace_det) when N > 10
// Maybe is more expensive due to copy
template <std::size_t N>
double gaussian_det(const Matrix<N, N>& m) {
  // Local copy, leave the original untouched
  Matrix<N, N> m_copy{m};
  double det{1.0};

  for (std::size_t i{0}; i < N; ++i) {
    // Find the pivot element
    std::size_t pivot{i};

    for (std::size_t j{i + 1}; j < N; ++j) {
      if (std::abs(m_copy[j][i]) > std::abs(m_copy[pivot][i])) {
        pivot = j;
      }
    }

    // If the pivot entry is 0, the determinant is 0
    if (std::abs(m_copy[pivot][i]) < 1e-9) {
      return 0.0;
    }

    // Swap the current row with the pivot row
    if (i != pivot) {
      std::swap(m_copy[i], m_copy[pivot]);
      det *= -1.0;  // Row swap flips the sign
    }

    // Multiply the determinant by the diagonal element
    det *= m_copy[i][i];

    // Eliminate the entries below the pivot
    for (std::size_t j{i + 1}; j < N; ++j) {
      double factor{m_copy[j][i] / m_copy[i][i]};

      for (std::size_t k{i}; k < N; ++k) {
        m_copy[j][k] -= factor * m_copy[i][k];
      }
    }
  }
  return det;
}

// Cofactor
template <std::size_t N>
Matrix<N, N> cofactor(const Matrix<N, N>& m) {
  Matrix<N, N> result{};

  for (std::size_t r{0}; r < N; ++r) {
    for (std::size_t c{0}; c < N; ++c) {
      // Build the (N - 1) x (N - 1) submatrix excluding row r and column c
      Matrix<N - 1, N - 1> submatrix{};
      std::size_t sub_r{};

      for (std::size_t i{0}; i < N; ++i) {
        if (i == r) continue;
        std::size_t sub_c{};

        for (std::size_t j{0}; j < N; ++j) {
          if (j == c) continue;
          submatrix[sub_r][sub_c] = m[i][j];
          ++sub_c;
        }

        ++sub_r;
      }

      // C(r,c) = (-1)^(r+c) * det(submatrix)
      int sign{((r + c) % 2 == 0) ? 1 : -1};
      result[r][c] = sign * laplace_det(submatrix);
    }
  }

  return result;
}

// Adjoint
template <std::size_t N>
Matrix<N, N> adj(const Matrix<N, N>& m) {
  return transpose(cofactor(m));
}

// Inverse
template <std::size_t N>
Matrix<N, N> inverse(const Matrix<N, N>& m) {
  return (1 / laplace_det(m)) * adj(m);
}

#endif
