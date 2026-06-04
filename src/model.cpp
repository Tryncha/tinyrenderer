#include "model.h"

#include <fstream>
#include <sstream>

Model::Model(const std::string filename) {
  std::ifstream in;
  in.open(filename, std::ifstream::in);

  if (in.fail()) return;

  std::string line;

  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;

    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      Vec<3> v;

      for (int i : {0, 1, 2}) {
        iss >> v[i];
      }

      verts.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      int f, t, n, cnt = 0;
      iss >> trash;

      while (iss >> f >> trash >> t >> trash >> n) {
        facet_vrt.push_back(--f);
        cnt++;
      }

      if (3 != cnt) {
        std::cerr << "Error: the obj file is supposed to be triangulated"
                  << '\n';
        return;
      }
    }
  }
}

int Model::get_num_verts() const { return verts.size(); }
int Model::get_num_faces() const { return facet_vrt.size() / 3; }

Vec<3> Model::get_vert(const int i) const { return verts[i]; }

Vec<3> Model::get_vert(const int iface, const int nthvert) const {
  return verts[facet_vrt[iface * 3 + nthvert]];
}
