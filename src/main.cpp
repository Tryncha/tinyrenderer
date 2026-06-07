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

void draw_triangle(Point2D a, Point2D b, Point2D c, TGAImage& framebuffer,
                   TGAColor color) {
  // bounding box for the triangle
  // defined by its top left and bottom right corners
  Point2D bounding_box_min{std::min(std::min(a.x, b.x), c.x),
                           std::min(std::min(a.y, b.y), c.y)};
  Point2D bounding_box_max{std::max(std::max(a.x, b.x), c.x),
                           std::max(std::max(a.y, b.y), c.y)};

  double total_area{signed_triangle_area(a, b, c)};

  // backface culling + discarding triangles that cover less than a pixel
  // if (total_area < 1) return;

  for (int x{bounding_box_min.x}; x <= bounding_box_max.x; ++x) {
    for (int y{bounding_box_min.y}; y <= bounding_box_max.y; ++y) {
      double alpha{signed_triangle_area({x, y}, b, c) / total_area};
      double beta{signed_triangle_area({x, y}, c, a) / total_area};
      double gamma{signed_triangle_area({x, y}, a, b) / total_area};

      // negative barycentric coordinate => the pixel is outside the triangle
      if (alpha < 0 || beta < 0 || gamma < 0) {
        continue;
      }

      framebuffer.set(x, y, color);
    }
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  Timer t{};

  TGAImage framebuffer(constants::width, constants::height, TGAImage::RGB);

  Point2D a1{7, 45};
  Point2D b1{35, 100};
  Point2D c1{45, 60};

  Point2D a2{120, 35};
  Point2D b2{90, 5};
  Point2D c2{45, 110};

  Point2D a3{115, 83};
  Point2D b3{80, 90};
  Point2D c3{85, 120};

  draw_triangle(a1, b1, c1, framebuffer, constants::colors::red);
  draw_triangle(a2, b2, c2, framebuffer, constants::colors::white);
  draw_triangle(a3, b3, c3, framebuffer, constants::colors::green);

  framebuffer.write_tga_file("framebuffer.tga");

  t.print_time_elapsed();

  return 0;
}
