#include "constant.h"
#include "matrix.h"
#include "model.h"
#include "random_mt.h"
#include "tgaimage.h"
#include "timer.h"
#include "vector.h"

// clang-format off
void set_model_view(Matrix<4, 4>& ModelView,
                    Vector<3> eye, Vector<3> center, Vector<3> up) {
  Vector<3> n{normalize(eye - center)};
  Vector<3> l{normalize(cross(up, n))};
  Vector<3> m{normalize(cross(n, l))};

  ModelView = Matrix<4, 4>{{{{l[0], l[1], l[2],          0},
                             {m[0], m[1], m[2],          0},
                             {n[0], n[1], n[2],          0},
                             {   0,    0,    0,          1}}}}

            * Matrix<4, 4>{{{{   1,    0,    0, -center[0]},
                             {   0,    1,    0, -center[1]},
                             {   0,    0,    1, -center[2]},
                             {   0,    0,    0,          1}}}};
}

void set_perspective(Matrix<4, 4>& Perspective, double d) {
  Perspective = {{{{1, 0,      0, 0},
                   {0, 1,      0, 0},
                   {0, 0,      1, 0},
                   {0, 0, -1 / d, 1}}}};
}

void set_viewport(Matrix<4, 4>& Viewport, int x, int y, int width, int height) {
  Viewport = {{{{width / 2.0,            0, 0, x + (width  / 2.0)},
                {          0, height / 2.0, 0, y + (height / 2.0)},
                {          0,            0, 1,                  0},
                {          0,            0, 0,                  1}}}};
}

// clang-format on
void rasterize(const Matrix<4, 4>& Viewport,
               const std::array<Vector<4>, 3>& clip,
               std::vector<double>& zbuffer, TGAImage& framebuffer,
               TGAColor color) {
  // clang-format off
  // Normalized device coordinates
  std::array<Vector<4>, 3> ndc{clip[0] / clip[0][3],
                               clip[1] / clip[1][3],
                               clip[2] / clip[2][3]};
  // clang-format on

  // Screen coordinates
  std::array<Vector<2>, 3> screen{
      {{(Viewport * ndc[0])[0], (Viewport * ndc[0])[1]},
       {(Viewport * ndc[1])[0], (Viewport * ndc[1])[1]},
       {(Viewport * ndc[2])[0], (Viewport * ndc[2])[1]}}};

  Matrix<3, 3> abc = {{{{screen[0][0], screen[0][1], 1.0},
                        {screen[1][0], screen[1][1], 1.0},
                        {screen[2][0], screen[2][1], 1.0}}}};

  // backface culling + discarding triangles that cover less than a pixel
  if (laplace_det(abc) < 1) return;

  // bounding box for the triangle
  auto [bbminx, bbmaxx] =
      std::minmax({screen[0][0], screen[1][0], screen[2][0]});

  // defined by its top left and bottom right corners
  auto [bbminy, bbmaxy] =
      std::minmax({screen[0][1], screen[1][1], screen[2][1]});

  // clip the bounding box by the screen
  for (int x{std::max<int>(static_cast<int>(bbminx), 0)};
       x <= std::min<int>(static_cast<int>(bbmaxx), framebuffer.width() - 1);
       ++x) {
    for (int y{std::max<int>(static_cast<int>(bbminy), 0)};
         y <= std::min<int>(static_cast<int>(bbmaxy), framebuffer.height() - 1);
         ++y) {
      // barycentric coordinates of {x,y} w.r.t the triangle
      Vector<3> bc{transpose(inverse(abc)) * Vector<3>{static_cast<double>(x),
                                                       static_cast<double>(y),
                                                       1.0}};

      // negative barycentric coordinate => the pixel is outside the triangle
      if (bc[0] < 0 || bc[1] < 0 || bc[2] < 0) continue;

      double z{bc * Vector<3>{ndc[0][2], ndc[1][2], ndc[2][2]}};
      auto zbuffer_idx = static_cast<std::size_t>(x + y * framebuffer.width());

      if (z <= zbuffer[zbuffer_idx]) continue;

      zbuffer[zbuffer_idx] = z;
      framebuffer.set(x, y, color);
    }
  }
}

int main(int argc, char** argv) {
  Timer t{};

  if (argc != 2) {
    std::cerr << "Please use: " << argv[0] << " assets/obj/model.obj" << '\n';
    return 1;
  }

  Matrix<4, 4> ModelView{};
  Matrix<4, 4> Perspective{};
  Matrix<4, 4> Viewport{};

  // clang-format off
  set_model_view (ModelView,   camera::eye, camera::center, camera::up);

  set_perspective(Perspective, norm(camera::eye - camera::center));

  set_viewport   (Viewport,  constant::width / 16,      constant::height / 16,
                            (constant::width * 7) / 8, (constant::height * 7) / 8);

  TGAImage framebuffer{constant::width, constant::height, TGAImage::format::rgb};

  // Note: () not {} — brace-initialization would call the initializer_list
  // constructor, creating a 2-element vector instead of width * height elements
  std::vector<double> zbuffer(constant::width * constant::height, -std::numeric_limits<double>::max());
  // clang-format on

  for (int m{1}; m < argc; ++m) {
    Model model{argv[m]};

    for (int i{0}; i < model.get_num_faces(); ++i) {
      std::array<Vector<4>, 3> clip{};

      for (const auto d : {0, 1, 2}) {
        Vector<3> v{model.get_vert(i, d)};
        clip[static_cast<std::size_t>(d)] =
            Perspective * ModelView * Vector<4>{v[0], v[1], v[2], 1.0};
      }

      TGAColor random_colors{};
      for (int j{0}; j < 3; ++j) {
        random_colors[j] = static_cast<std::uint8_t>(random_mt::get(0, 255));
      }

      rasterize(Viewport, clip, zbuffer, framebuffer, random_colors);
    }
  }

  framebuffer.write_tga_file("framebuffer.tga");
  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}
