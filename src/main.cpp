#include "constants.h"
#include "matrix.h"
#include "tgaimage.h"
#include "timer.h"
#include "vector.h"

struct Point2D {
  int x{};
  int y{};
};

double signed_triangle_area(Point2D a, Point2D b, Point2D c) {
  return 0.5 * ((b.y - a.y) * (b.x + a.x) + (c.y - b.y) * (c.x + b.x) +
                (a.y - c.y) * (a.x + c.x));
}

void draw_triangle(Point2D a, int intensity_a, Point2D b, int intensity_b,
                   Point2D c, int intensity_c, TGAImage& framebuffer) {
  // bounding box for the triangle
  // defined by its top left and bottom right corners
  Point2D bounding_box_min{std::min(std::min(a.x, b.x), c.x),
                           std::min(std::min(a.y, b.y), c.y)};
  Point2D bounding_box_max{std::max(std::max(a.x, b.x), c.x),
                           std::max(std::max(a.y, b.y), c.y)};

  double total_area{signed_triangle_area({a.x, a.y}, {b.x, b.y}, {c.x, c.y})};

  // backface culling + discarding triangles that cover less than a pixel
  if (total_area < 1) return;

  for (int x{bounding_box_min.x}; x <= bounding_box_max.x; ++x) {
    for (int y{bounding_box_min.y}; y <= bounding_box_max.y; ++y) {
      // clang-format off
      double alpha{signed_triangle_area({x, y}, {b.x, b.y}, {c.x, c.y}) / total_area};
      double beta {signed_triangle_area({x, y}, {c.x, c.y}, {a.x, a.y}) / total_area};
      double gamma{signed_triangle_area({x, y}, {a.x, a.y}, {b.x, b.y}) / total_area};
      // clang-format on

      // negative barycentric coordinate => the pixel is outside the triangle
      if (alpha < 0 || beta < 0 || gamma < 0) {
        continue;
      }

      // Draw a "wireframe" omiting center points
      // clang-format off
      if ((constants::lower_limit < alpha && alpha < constants::upper_limit) &&
          (constants::lower_limit < beta  && beta  < constants::upper_limit) &&
          (constants::lower_limit < gamma && gamma < constants::upper_limit)) {
        // clang-format on
        continue;
      }

      // Using z coordinates to create a grayscale,
      // interpolating the color as the weighted sum below
      // clang-format off
      auto intensity_red{
          static_cast<std::uint8_t>(alpha * intensity_a)};
      auto intensity_green{
          static_cast<std::uint8_t>(beta  * intensity_b)};
      auto intensity_blue{
          static_cast<std::uint8_t>(gamma * intensity_c)};
      // clang-format on

      framebuffer.set(x, y, {intensity_red, intensity_green, intensity_blue});
      // Following the inverse order of colors, it should be
      // {intensity_blue, intensity_green, intensity_red} instead
    }
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  // Timer t{};

  // TGAImage framebuffer(constants::width, constants::height,
  //                      TGAImage::format::rgb);

  // Point2D a{17, 4};
  // Point2D b{55, 39};
  // Point2D c{23, 59};

  // draw_triangle(a, 255, b, 255, c, 255, framebuffer);

  // framebuffer.write_tga_file("framebuffer.tga");
  // t.print_time_elapsed();

  // std::cout << "Vector examples:\n";

  // Vector<3> v1{3, -2, 1};
  // Vector<3> v2{-1, 4, 2};

  // std::cout << v1 << " + " << v2 << " = " << v1 + v2 << '\n';
  // std::cout << v1 << " - " << v2 << " = " << v1 - v2 << '\n';
  // std::cout << v1 << " * " << v2 << " = " << v1 * v2 << '\n';

  // std::cout << 2.0 << " * " << v1 << " = " << 2.0 * v1 << '\n';
  // std::cout << v1 << " * " << 2.0 << " = " << v1 * 2.0 << '\n';

  std::cout << '\n';
  std::cout << "Matrix examples:\n";

  Matrix<2, 3> m1{{{{3, -2, 1}, {-5, 3, 1}}}};
  Matrix<2, 3> m2{{{{5, 3, -3}, {1, 4, -5}}}};
  Matrix<3, 2> m3{{{{5, 3}, {1, 4}, {-3, -5}}}};

  std::cout << m1 << " + " << m2 << " = " << m1 + m2 << '\n';
  std::cout << m1 << " - " << m2 << " = " << m1 - m2 << '\n';
  std::cout << m1 << " * " << m3 << " = " << m1 * m3 << '\n';

  std::cout << 2.0 << " * " << m1 << " = " << 2.0 * m1 << '\n';
  std::cout << m1 << " * " << 2.0 << " = " << m1 * 2.0 << '\n';

  std::cout << m1 << ", transpose: " << transpose(m1) << '\n';

  Matrix<2, 2> sqm2{{{{6, 1}, {4, -2}}}};

  // Should be -16
  std::cout << sqm2 << ", determinant (Laplace): " << laplace_det(sqm2) << '\n';
  std::cout << sqm2 << ", determinant (Gaussian): " << gaussian_det(sqm2)
            << '\n';

  // Should be:
  // [[-2, -4], [-1, 6]]
  std::cout << sqm2 << ", cofactor: " << cofactor(sqm2) << '\n';

  // [[-2, -1], [-4, 6]]
  std::cout << sqm2 << ", adjoint: " << adj(sqm2) << '\n';

  // https://www.emathhelp.net/calculators/linear-algebra/inverse-of-matrix-calculator/?i=%5B%5B6%2C1%5D%2C%5B4%2C-2%5D%5D&m=a
  std::cout << sqm2 << ", inverse: " << inverse(sqm2) << '\n';

  Matrix<3, 3> sqm3{{{{6, 1, 1}, {4, -2, 5}, {2, 8, 7}}}};

  // Should be -306
  std::cout << sqm3 << ", determinant (Laplace): " << laplace_det(sqm3) << '\n';
  std::cout << sqm3 << ", determinant (Gaussian): " << gaussian_det(sqm3)
            << '\n';

  // Should be:
  // [[-54, -18, 36], [1, 40, -46], [7, -26, -16]]
  std::cout << sqm3 << ", cofactor: " << cofactor(sqm3) << '\n';

  // [[-54, 1, 7], [-18, 40, -26], [36, -46, -16]]
  std::cout << sqm3 << ", adjoint: " << adj(sqm3) << '\n';

  // https://www.emathhelp.net/calculators/linear-algebra/inverse-of-matrix-calculator/?i=%5B%5B6%2C1%2C1%5D%2C%5B4%2C-2%2C5%5D%2C%5B2%2C8%2C7%5D%5D&m=a
  std::cout << sqm3 << ", inverse: " << inverse(sqm3) << '\n';

  return 0;
}
