#include <algorithm>
#include <cmath>

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
    // Do not discard the pixel
    return {false, color};
  }
};

struct PhongShader : IShader {
  const Model& m_model;
  // Light direction in eye coordinates
  Vector<3> m_light{};
  // Triangle in eye coordinates
  std::array<Vector<3>, 3> m_triangle{};

  PhongShader(const Model& model, Vector<3> light) : m_model(model) {
    // Transform the light vector to view coordinates
    m_light = normalize(Vector<3>{
        (matrix::ModelView * Vector<4>{light.x, light.y, light.z, 0.0}).x,
        (matrix::ModelView * Vector<4>{light.x, light.y, light.z, 0.0}).y,
        (matrix::ModelView * Vector<4>{light.x, light.y, light.z, 0.0}).z});
  }

  virtual Vector<4> vertex(int face, int vert) {
    // Current vertex in object coordinates
    Vector<3> v{m_model.get_vert(face, vert)};
    Vector<4> gl_Position = matrix::ModelView * Vector<4>{v.x, v.y, v.z, 1.0};

    // in eye coordinates
    m_triangle[static_cast<std::size_t>(vert)] = {gl_Position.x, gl_Position.y,
                                                  gl_Position.z};

    // in clip coordinates
    return matrix::Perspective * gl_Position;
  }

  virtual std::pair<bool, TGAColor> fragment() const {
    // Output color of the fragment
    TGAColor gl_FragColor{255, 255, 255, 255};
    // Triangle normal in eye coordinates
    Vector<3> n{normalize(
        cross(m_triangle[1] - m_triangle[0], m_triangle[2] - m_triangle[0]))};

    // Reflected light direction
    Vector<3> r{normalize(2 * n * (n * m_light) - m_light)};

    // Ambient light intensity
    double ambient = 0.3;

    // Diffuse light intensity
    double diff = std::max(0.0, n * m_light);

    // Specular intensity, note that the camera lies on the z-axis (in eye
    // coordinates), therefore simple r.z, since (0,0,1) * (r.x, r.y, r.z) = r.z
    double spec{std::pow(std::max(r.z, 0.0), 35)};

    const double intensity{std::min(1.0, ambient + 0.4 * diff + 0.9 * spec)};

    for (const auto channel : {0, 1, 2}) {
      gl_FragColor[channel] =
          static_cast<std::uint8_t>(gl_FragColor[channel] * intensity);
    }

    // Do not discard the pixel
    return {false, gl_FragColor};
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

  TGAImage framebuffer{constant::width, constant::height,
                       TGAImage::format::rgb};

  // Iterate through all input objects
  for (int m{1}; m < argc; ++m) {
    // Load the data
    Model model{argv[m]};
    PhongShader shader{model, {1, 1, 1}};

    for (int f{0}; f < model.get_num_faces(); ++f) {
      Triangle clip{shader.vertex(f, 0), shader.vertex(f, 1),
                    shader.vertex(f, 2)};
      rasterize(clip, shader, zbuffer, framebuffer);
    }
  }

  framebuffer.write_tga_file("framebuffer.tga");
  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}
