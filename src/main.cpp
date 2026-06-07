#include "constants.h"
#include "tgaimage.h"
#include "timer.h"

struct Point2D {
  int x{};
  int y{};
};

struct Point3D {
  int x{};
  int y{};
  int z{};
};

double signed_triangle_area(Point2D a, Point2D b, Point2D c) {
  return 0.5 * ((b.y - a.y) * (b.x + a.x) + (c.y - b.y) * (c.x + b.x) +
                (a.y - c.y) * (a.x + c.x));
}

void draw_triangle(Point3D a, Point3D b, Point3D c, TGAImage& framebuffer) {
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

      // Using z coordinates to create a grayscale,
      // interpolating the color as the weighted sum below
      TGAColor z{
          static_cast<unsigned char>(alpha * a.z + beta * b.z + gamma * c.z)};

      framebuffer.set(x, y, {z});
    }
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  Timer t{};

  TGAImage framebuffer(constants::width, constants::height,
                       TGAImage::format::grayscale);

  Point3D a{17, 4, 13};
  Point3D b{55, 39, 128};
  Point3D c{23, 59, 255};

  draw_triangle(a, b, c, framebuffer);

  framebuffer.write_tga_file("framebuffer.tga");
  t.print_time_elapsed();

  return 0;
}
