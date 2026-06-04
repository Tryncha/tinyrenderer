#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "geometry.h"

class Model {
 public:
  Model(const std::string filename);

  int get_num_verts() const;  // number of vertices
  int get_num_faces() const;  // number of triangles

  // 0 <= i < nverts()
  Vec<3> get_vert(const int i) const;
  // 0 <= iface <= nfaces(), 0 <= nthvert < 3
  Vec<3> get_vert(const int iface, const int nthvert) const;

 private:
  std::vector<Vec<3>> verts{};   // array of vertices
  std::vector<int> facet_vrt{};  // per-triangle index in the above array
};

#endif
