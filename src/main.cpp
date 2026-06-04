#include <chrono>
#include <cmath>
#include <iostream>

#include "constants.h"
#include "geometry.h"
#include "model.h"
#include "random-mt.h"
#include "tgaimage.h"
#include "timer.h"

void draw_line(int ax, int ay, int bx, int by, TGAImage& framebuffer,
               const TGAColor& color) {
  bool is_steep{std::abs(ax - bx) < std::abs(ay - by)};
  // if the line is steep, we transpose the image
  if (is_steep) {
    std::swap(ax, ay);
    std::swap(bx, by);
  }

  // make it left−to−right
  if (ax > bx) {
    std::swap(ax, bx);
    std::swap(ay, by);
  }

  int y{ay};
  int error{0};

  for (int x{ax}; x <= bx; ++x) {
    // if transposed, de−transpose
    if (is_steep) {
      framebuffer.set(static_cast<int>(y), x, color);
    } else {
      framebuffer.set(x, static_cast<int>(y), color);
    }

    error += 2 * std::abs(by - ay);

    if (error > bx - ax) {
      y += by > ay ? 1 : -1;
      error -= 2 * (bx - ax);
    }
  }
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy,
              TGAImage& framebuffer, TGAColor color) {
  draw_line(ax, ay, bx, by, framebuffer, color);
  draw_line(bx, by, cx, cy, framebuffer, color);
  draw_line(cx, cy, ax, ay, framebuffer, color);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  Timer t{};

  TGAImage framebuffer(constants::width, constants::height, TGAImage::RGB);

  triangle(7, 45, 35, 100, 45, 60, framebuffer, constants::colors::red);
  triangle(120, 35, 90, 5, 45, 110, framebuffer, constants::colors::white);
  triangle(115, 83, 80, 90, 85, 120, framebuffer, constants::colors::green);

  framebuffer.write_tga_file("framebuffer.tga");

  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}
