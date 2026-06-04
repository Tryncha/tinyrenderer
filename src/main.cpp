#include <cmath>
#include <iostream>
#include <utility>

#include "constants.h"
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

// First of all, (x,y) is an orthogonal projection of the vector (x,y,z).
// Second, since the input models are scaled to have fit in the [-1,1]^3 world
// coordinates, we want to shift the vector (x,y) and then scale it to span the
// entire screen.
std::pair<int, int> project(Vec<3> v) {
  return {(v[0] + 1.0f) * constants::width / 2,
          (v[1] + 1.0f) * constants::height / 2};
}

void build_model(const std::string filename, TGAImage& framebuffer) {
  Model model{filename};

  // iterate through all triangles
  for (int i{0}; i < model.get_num_faces(); ++i) {
    const auto [ax, ay] = project(model.get_vert(i, 0));
    const auto [bx, by] = project(model.get_vert(i, 1));
    const auto [cx, cy] = project(model.get_vert(i, 2));

    draw_line(ax, ay, bx, by, framebuffer, constants::colors::red);
    draw_line(bx, by, cx, cy, framebuffer, constants::colors::red);
    draw_line(cx, cy, ax, ay, framebuffer, constants::colors::red);
  }

  // iterate through all vertices
  for (int i{0}; i < model.get_num_verts(); ++i) {
    const Vec<3> v{model.get_vert(i)};  // get i-th vertex
    const auto [x, y] = project(v);     // project it to the screen

    framebuffer.set(x, y, constants::colors::white);
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  Timer t{};

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " obj/model.obj" << '\n';
    return 1;
  }

  TGAImage framebuffer(constants::width, constants::height, TGAImage::RGB);

  build_model(argv[1], framebuffer);
  framebuffer.write_tga_file("framebuffer.tga");

  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}
