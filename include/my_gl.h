#ifndef MY_GL_H
#define MY_GL_H

#include "tgaimage.h"
#include "vector.h"

void look_at(Vector<3> eye, Vector<3> center, Vector<3> up);
void init_perspective(double f);
void init_viewport(int x, int y, int width, int height);

struct IShader {
  virtual std::pair<bool, TGAColor> fragment() const = 0;
};

typedef std::array<Vector<4>, 3> Triangle;

void rasterize(const Triangle& clip, const IShader& shader,
               std::vector<double>& zbuffer, TGAImage& framebuffer);

#endif
