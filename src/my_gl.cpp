#include "my_gl.h"

#include <algorithm>

#include "constant.h"

// clang-format off
void look_at(Vector<3> eye, Vector<3> center, Vector<3> up) {
  Vector<3> n{normalize(eye - center)};
  Vector<3> l{normalize(cross(up, n))};
  Vector<3> m{normalize(cross(n, l))};

  matrix::ModelView = Matrix<4, 4>{{{{l.x, l.y, l.z,         0},
                                     {m.x, m.y, m.z,         0},
                                     {n.x, n.y, n.z,         0},
                                     {  0,   0,   0,         1}}}}
                    * Matrix<4, 4>{{{{  1,   0,   0, -center.x},
                                     {  0,   1,   0, -center.y},
                                     {  0,   0,   1, -center.z},
                                     {  0,   0,   0,         1}}}};
}

void init_perspective(double f) {
  matrix::Perspective = {{{{1, 0,      0, 0},
                           {0, 1,      0, 0},
                           {0, 0,      1, 0},
                           {0, 0, -1 / f, 1}}}};
}

void init_viewport(int x, int y, int width, int height) {
  matrix::Viewport = {{{{width / 2.0,            0, 0, x + (width  / 2.0)},
                        {          0, height / 2.0, 0, y + (height / 2.0)},
                        {          0,            0, 1,                  0},
                        {          0,            0, 0,                  1}}}};
}

// clang-format on
void rasterize(const Triangle& clip, const IShader& shader,
               std::vector<double>& zbuffer, TGAImage& framebuffer) {
  // clang-format off
  // Normalized device coordinates
  const std::array<Vector<4>, 3> ndc{clip[0] / clip[0].w,
                                     clip[1] / clip[1].w,
                                     clip[2] / clip[2].w};
  // clang-format on

  // Screen coordinates
  const std::array<Vector<2>, 3> screen{
      {{(matrix::Viewport * ndc[0]).x, (matrix::Viewport * ndc[0]).y},
       {(matrix::Viewport * ndc[1]).x, (matrix::Viewport * ndc[1]).y},
       {(matrix::Viewport * ndc[2]).x, (matrix::Viewport * ndc[2]).y}}};

  const Matrix<3, 3> abc{{{{screen[0].x, screen[0].y, 1.0},
                           {screen[1].x, screen[1].y, 1.0},
                           {screen[2].x, screen[2].y, 1.0}}}};

  // Backface culling + discarding triangles that cover less than a pixel
  if (laplace_det(abc) < 1) return;

  // Bounding box for the triangle
  // Defined by its top left and bottom right corners
  const auto [bbminx, bbmaxx] =
      std::minmax({screen[0].x, screen[1].x, screen[2].x});
  const auto [bbminy, bbmaxy] =
      std::minmax({screen[0].y, screen[1].y, screen[2].y});

  // Clip the bounding box by the screen
  for (int x{std::max<int>(static_cast<int>(bbminx), 0)};
       x <= std::min<int>(static_cast<int>(bbmaxx), framebuffer.width() - 1);
       ++x) {
    for (int y{std::max<int>(static_cast<int>(bbminy), 0)};
         y <= std::min<int>(static_cast<int>(bbmaxy), framebuffer.height() - 1);
         ++y) {
      // Barycentric coordinates of {x,y} w.r.t the triangle
      Vector<3> bc{transpose(inverse(abc)) * Vector<3>{static_cast<double>(x),
                                                       static_cast<double>(y),
                                                       1.0}};

      // Negative barycentric coordinate => the pixel is outside the triangle
      if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;

      // Linear interpolation of the depth
      double z{bc * Vector<3>{ndc[0].z, ndc[1].z, ndc[2].z}};

      // Discard fragments that are too deep w.r.t the z-buffer
      const auto zbuffer_idx{
          static_cast<std::size_t>(x + y * framebuffer.width())};
      if (z <= zbuffer[zbuffer_idx]) continue;

      // Fragment shader can discard current fragment
      const auto [discard, color] = shader.fragment();
      if (discard) continue;

      // Update the z-buffer
      zbuffer[zbuffer_idx] = z;
      // Update the framebuffer
      framebuffer.set(x, y, color);
    }
  }
}
