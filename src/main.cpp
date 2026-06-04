#include <chrono>
#include <cmath>
#include <iostream>

#include "random-mt.h"
#include "tgaimage.h"
#include "timer.h"

namespace colors {
// clang-format off
// attention: order         B    G    R    A
constexpr TGAColor white  {255, 255, 255, 255};
constexpr TGAColor green  {  0, 255,   0, 255};
constexpr TGAColor red    {  0,   0, 255, 255};
constexpr TGAColor blue   {255, 128,  64, 255};
constexpr TGAColor yellow {  0, 200, 255, 255};
// clang-format on
}  // namespace colors

struct Coord {
  int x{};
  int y{};
};

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

    // using this 'ugly' technique for critical performance
    y += by > ax ? 1 : -1 * (error > bx - ax);
    error -= 2 * (bx - ax) * (error > bx - ax);
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  Timer t{};

  constexpr int wdt{64};
  constexpr int hgt{64};

  TGAImage framebuffer(wdt, hgt, TGAImage::RGB);

  for (int i{0}; i < (1 << 24); ++i) {
    Coord a{random_mt::get(0, wdt), random_mt::get(0, hgt)};
    Coord b{random_mt::get(0, wdt), random_mt::get(0, hgt)};

    draw_line(a.x, a.y, b.x, b.y, framebuffer,
              {static_cast<std::uint8_t>(random_mt::get(0, 255)),
               static_cast<std::uint8_t>(random_mt::get(0, 255)),
               static_cast<std::uint8_t>(random_mt::get(0, 255)),
               static_cast<std::uint8_t>(random_mt::get(0, 255))});
  }

  // Coord a{7, 3};
  // Coord b{12, 37};
  // Coord c{62, 53};

  // draw_line(a, b, framebuffer, colors::blue);
  // draw_line(c, b, framebuffer, colors::green);
  // draw_line(c, a, framebuffer, colors::yellow);
  // draw_line(a, c, framebuffer, colors::red);

  // framebuffer.set(a.x, a.y, colors::white);
  // framebuffer.set(b.x, b.y, colors::white);
  // framebuffer.set(c.x, c.y, colors::white);

  framebuffer.write_tga_file("framebuffer.tga");

  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}
