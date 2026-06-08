#include "constants.h"
#include "tgaimage.h"
#include "timer.h"

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

      double lower_limit{0.1};
      double upper_limit{0.9};

      // Draw a "wireframe" omiting center points
      // clang-format off
      if ((lower_limit < alpha && alpha < upper_limit) &&
          (lower_limit < beta  && beta  < upper_limit) &&
          (lower_limit < gamma && gamma < upper_limit)) {
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
  Timer t{};

  TGAImage framebuffer(constants::width, constants::height,
                       TGAImage::format::rgb);

  Point2D a{17, 4};
  Point2D b{55, 39};
  Point2D c{23, 59};

  draw_triangle(a, 255, b, 255, c, 255, framebuffer);

  framebuffer.write_tga_file("framebuffer.tga");
  t.print_time_elapsed();

  return 0;
}
