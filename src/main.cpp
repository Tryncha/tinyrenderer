#include "constant.h"
#include "model.h"
#include "my_gl.h"
#include "random_mt.h"
#include "tgaimage.h"
#include "timer.h"
#include "vector.h"

struct RandomShader : IShader {
  const Model& model;
  TGAColor color{};

  // Triangle in eye coordinates
  std::array<Vector<3>, 3> triangle{};

  RandomShader(const Model& m) : model(m) {}

  virtual Vector<4> vertex(int face, int vert) {
    // Current vertex in object coordinates
    Vector<3> v{model.get_vert(face, vert)};
    Vector<4> gl_Position = matrix::ModelView * Vector<4>{v.x, v.y, v.z, 1.0};

    // in eye coordinates
    triangle[static_cast<std::size_t>(vert)] = {gl_Position.x, gl_Position.y,
                                                gl_Position.z};

    // in clip coordinates
    return matrix::Perspective * gl_Position;
  }

  virtual std::pair<bool, TGAColor> fragment() const {
    return {false, color};  // Do not discard the pixel
  }
};

int main(int argc, char** argv) {
  Timer t{};

  if (argc != 2) {
    std::cerr << "Please use: " << argv[0] << " assets/obj/model.obj" << '\n';
    return 1;
  }

  look_at(camera::eye, camera::center, camera::up);
  init_perspective(norm(camera::eye - camera::center));
  init_viewport(constant::width / 16, constant::height / 16,
                (constant::width * 7) / 8, (constant::height * 7) / 8);

  // Note: () not {} — brace-initialization would call the initializer_list
  // constructor, creating a 2-element vector instead of width * height elements
  // clang-format off
  std::vector<double> zbuffer(constant::width * constant::height, -std::numeric_limits<double>::max());
  // clang-format on

  TGAImage framebuffer{constant::width,
                       constant::height,
                       TGAImage::format::rgb,
                       {177, 195, 209, 255}};

  // Iterate through all input objects
  for (int m{1}; m < argc; ++m) {
    // Load the data
    Model model{argv[m]};
    RandomShader shader{model};

    for (int f{0}; f < model.get_num_faces(); ++f) {
      shader.color = {static_cast<uint8_t>(random_mt::get(0, 255)),
                      static_cast<uint8_t>(random_mt::get(0, 255)),
                      static_cast<uint8_t>(random_mt::get(0, 255)), 255};

      Triangle clip{shader.vertex(f, 0), shader.vertex(f, 1),
                    shader.vertex(f, 2)};

      rasterize(clip, shader, zbuffer, framebuffer);
    }
  }

  framebuffer.write_tga_file("framebuffer.tga");
  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}
