#include <cmath>
#include <iostream>
#include <utility>

#include "constants.h"
#include "model.h"
#include "random-mt.h"
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

// First of all, (x,y) is an orthogonal projection of the vector (x,y,z).
// Second, since the input models are scaled to have fit in the [-1,1]^3 world
// coordinates, we want to shift the vector (x,y) and then scale it to span the
// entire screen.
std::pair<int, int> project(Vec<3> v) {
  return {(v[0] + 1.0f) * constants::width / 2,
          (v[1] + 1.0f) * constants::height / 2};
}

double signed_triangle_area(Point2D a, Point2D b, Point2D c) {
  return 0.5 * ((b.y - a.y) * (b.x + a.x) + (c.y - b.y) * (c.x + b.x) +
                (a.y - c.y) * (a.x + c.x));
}

// Using bounding boxes
void draw_triangle(Point2D a, Point2D b, Point2D c, TGAImage& framebuffer,
                   TGAColor color) {
  Point2D bounding_box_min{std::min(std::min(a.x, b.x), c.x),
                           std::min(std::min(a.y, b.y), c.y)};
  Point2D bounding_box_max{std::max(std::max(a.x, b.x), c.x),
                           std::max(std::max(a.y, b.y), c.y)};

  double total_area{signed_triangle_area(a, b, c)};
  if (total_area < 1) return;

  for (int x{bounding_box_min.x}; x <= bounding_box_max.x; ++x) {
    for (int y{bounding_box_min.y}; y <= bounding_box_max.y; ++y) {
      double alpha{signed_triangle_area({x, y}, b, c) / total_area};
      double beta{signed_triangle_area({x, y}, c, a) / total_area};
      double gamma{signed_triangle_area({x, y}, a, b) / total_area};

      if (alpha < 0 || beta < 0 || gamma < 0) {
        continue;
      }

      framebuffer.set(x, y, color);
    }
  }
}

void build_model(const std::string filename, TGAImage& framebuffer) {
  Model model{filename};

  // Iterate through all triangles
  for (int i{0}; i < model.get_num_faces(); ++i) {
    const auto [ax, ay] = project(model.get_vert(i, 0));
    const auto [bx, by] = project(model.get_vert(i, 1));
    const auto [cx, cy] = project(model.get_vert(i, 2));

    TGAColor random_colors{};
    for (int c{0}; c < 3; ++c) {
      random_colors[c] = static_cast<std::uint8_t>(random_mt::get(0, 255));
    }

    draw_triangle({ax, ay}, {bx, by}, {cx, cy}, framebuffer, random_colors);

    // draw_line({ax, ay}, {bx, by}, framebuffer, constants::colors::red);
    // draw_line({bx, by}, {cx, cy}, framebuffer, constants::colors::red);
    // draw_line({cx, cy}, {ax, ay}, framebuffer, constants::colors::red);
  }

  // iterate through all vertices
  // for (int i{0}; i < model.get_num_verts(); ++i) {
  //   const Vec<3> v{model.get_vert(i)};  // get i-th vertex
  //   const auto [x, y] = project(v);     // project it to the screen

  //   framebuffer.set(x, y, constants::colors::white);
  // }
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
