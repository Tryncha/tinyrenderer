#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "vector.h"

class Model {
 public:
  Model(const std::string filename);

  // number of vertices
  int get_num_verts() const;
  // number of triangles
  int get_num_faces() const;

  // 0 <= i < nverts()
  Vector<3> get_vert(const int i) const;
  // 0 <= iface <= nfaces(), 0 <= nthvert < 3
  Vector<3> get_vert(const int iface, const int nthvert) const;

 private:
  // array of vertices
  std::vector<Vector<3>> verts{};
  // per-triangle index in the above array
  std::vector<int> facet_vrt{};
};

#endif
