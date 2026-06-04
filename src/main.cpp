#include <chrono>
#include <cmath>
#include <iostream>

#include "geometry.h"
#include "model.h"
#include "random-mt.h"
#include "tgaimage.h"
#include "timer.h"

namespace constants {
constexpr int width{1024};
constexpr int height{1024};
}  // namespace constants

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

// First of all, (x,y) is an orthogonal projection of the vector (x,y,z).
// Second, since the input models are scaled to have fit in the [-1,1]^3 world
// coordinates, we want to shift the vector (x,y) and then scale it to span the
// entire screen.
std::tuple<int, int> project(Vec<3> v) {
  return {(v[0] + 1.0f) * constants::width / 2,
          (v[1] + 1.0f) * constants::height / 2};
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  Timer t{};

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " obj/model.obj" << '\n';
    return 1;
  }

  Model model(argv[1]);
  TGAImage framebuffer(constants::width, constants::height, TGAImage::RGB);

  // iterate through all triangles
  for (int i{0}; i < model.get_num_faces(); ++i) {
    const auto [ax, ay] = project(model.vert(i, 0));
    const auto [bx, by] = project(model.vert(i, 1));
    const auto [cx, cy] = project(model.vert(i, 2));

    draw_line(ax, ay, bx, by, framebuffer, colors::red);
    draw_line(bx, by, cx, cy, framebuffer, colors::red);
    draw_line(cx, cy, ax, ay, framebuffer, colors::red);
  }

  // iterate through all vertices
  for (int i{0}; i < model.get_num_verts(); ++i) {
    const Vec<3> v{model.vert(i)};   // get i-th vertex
    const auto [x, y] = project(v);  // project it to the screen

    framebuffer.set(x, y, colors::white);
  }

  framebuffer.write_tga_file("framebuffer.tga");

  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}
