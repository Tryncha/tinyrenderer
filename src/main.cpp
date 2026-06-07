#include "constants.h"
#include "tgaimage.h"
#include "timer.h"

struct Point2D {
  int x{};
  int y{};
};

void draw_line(Point2D a, Point2D b, TGAImage& framebuffer,
               const TGAColor& color) {
  bool is_steep{std::abs(a.x - b.x) < std::abs(a.y - b.y)};
  // if the line is steep, we transpose the image
  if (is_steep) {
    std::swap(a.x, a.y);
    std::swap(b.x, b.y);
  }

  // make it left−to−right
  if (a.x > b.x) {
    std::swap(a.x, b.x);
    std::swap(a.y, b.y);
  }

  int y{a.y};
  int error{0};

  for (int x{a.x}; x <= b.x; ++x) {
    // if transposed, de−transpose
    if (is_steep) {
      framebuffer.set(static_cast<int>(y), x, color);
    } else {
      framebuffer.set(x, static_cast<int>(y), color);
    }

    error += 2 * std::abs(b.y - a.y);

    if (error > b.x - a.x) {
      y += b.y > a.y ? 1 : -1;
      error -= 2 * (b.x - a.x);
    }
  }
}

void draw_triangle(Point2D a, Point2D b, Point2D c, TGAImage& framebuffer,
                   TGAColor color) {
  draw_line(a, b, framebuffer, color);
  draw_line(b, c, framebuffer, color);
  draw_line(c, a, framebuffer, color);
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

  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}
