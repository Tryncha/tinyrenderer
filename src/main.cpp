#include "constants.h"
#include "model.h"
#include "random-mt.h"
#include "tgaimage.h"
#include "timer.h"

struct Point2D {
  int x{};
  int y{};
};

double signed_triangle_area(Point2D a, Point2D b, Point2D c) {
  // clang-format off
        return 0.5 * ((b.y - a.y) * (b.x + a.x)
                   +  (c.y - b.y) * (c.x + b.x)
                   +  (a.y - c.y) * (a.x + c.x));
  // clang-format on
}

// Bounding box for the triangle defined by
// its top left and bottom right corners
void draw_triangle(Point2D a, int intensity_a, Point2D b, int intensity_b,
                   Point2D c, int intensity_c, TGAImage& framebuffer,
                   TGAImage& zbuffer, TGAColor color) {
  Point2D bounding_box_min{std::min(std::min(a.x, b.x), c.x),
                           std::min(std::min(a.y, b.y), c.y)};
  Point2D bounding_box_max{std::max(std::max(a.x, b.x), c.x),
                           std::max(std::max(a.y, b.y), c.y)};

  double total_area{signed_triangle_area(a, b, c)};

  // Backface culling + discarding triangles that cover less than a pixel
  if (total_area < 1) return;

  for (int x{bounding_box_min.x}; x <= bounding_box_max.x; ++x) {
    for (int y{bounding_box_min.y}; y <= bounding_box_max.y; ++y) {
      // clang-format off
        double alpha{signed_triangle_area({x, y}, b, c) / total_area};
        double beta {signed_triangle_area({x, y}, c, a) / total_area};
        double gamma{signed_triangle_area({x, y}, a, b) / total_area};
      // clang-format on

      if (alpha < 0 || beta < 0 || gamma < 0) {
        continue;
      }

      const auto depth_gray = static_cast<std::uint8_t>(
          alpha * intensity_a + beta * intensity_b + gamma * intensity_c);

      if (depth_gray <= zbuffer.get(x, y)[0]) {
        continue;
      }

      framebuffer.set(x, y, color);
      zbuffer.set(x, y, {depth_gray});
    }
  }
}

// First of all, (x,y) is an orthogonal projection of the vector (x,y,z).
// Second, since the input models are scaled to have fit in the [-1,1]^3 world
// coordinates, we want to shift the vector (x,y) and then scale it to span the
// entire screen.
std::pair<Point2D, int> project(Vec<3> v) {
  // clang-format off
  return {{static_cast<int>((v[0] + 1.0f) * constants::width  / 2),
           static_cast<int>((v[1] + 1.0f) * constants::height / 2)},
                            (v[2] + 1.0f) * 255.0f / 2};
  // clang-format on
}

void build_model(const std::string filename, TGAImage& framebuffer,
                 TGAImage& zbuffer) {
  Model model{filename};

  // Iterate through all triangles
  for (int i{0}; i < model.get_num_faces(); ++i) {
    const auto [a, intensity_a] = project(model.get_vert(i, 0));
    const auto [b, intensity_b] = project(model.get_vert(i, 1));
    const auto [c, intensity_c] = project(model.get_vert(i, 2));

    TGAColor random_colors{};
    for (int j{0}; j < 3; ++j) {
      random_colors[j] = static_cast<std::uint8_t>(random_mt::get(0, 255));
    }

    draw_triangle({a.x, a.y}, intensity_a, {b.x, b.y}, intensity_b, {c.x, c.y},
                  intensity_c, framebuffer, zbuffer, random_colors);
  }
}

int main([[maybe_unused]] int argc, char** argv) {
  Timer t{};

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " obj/model.obj" << '\n';
    return 1;
  }

  // clang-format off
  TGAImage framebuffer{constants::width, constants::height, TGAImage::format::rgb};
  TGAImage zbuffer    {constants::width, constants::height, TGAImage::format::grayscale};
  // clang-format on

  build_model(argv[1], framebuffer, zbuffer);

  framebuffer.write_tga_file("framebuffer.tga");
  zbuffer.write_tga_file("zbuffer.tga");

  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}
