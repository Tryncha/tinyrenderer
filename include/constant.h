#ifndef CONSTANT_H
#define CONSTANT_H

#include "matrix.h"
#include "tgaimage.h"

// clang-format off
namespace constant {
constexpr int width {1024};
constexpr int height{1024};
}  // namespace constant

namespace color {
// warning: order           B    G    R    A
constexpr TGAColor white {255, 255, 255, 255};
constexpr TGAColor green {  0, 255,   0, 255};
constexpr TGAColor red   {  0,   0, 255, 255};
constexpr TGAColor blue  {255, 128,  64, 255};
constexpr TGAColor yellow{  0, 200, 255, 255};
}  // namespace color

namespace matrix{
inline Matrix<4, 4> ModelView{};
inline Matrix<4, 4> Perspective{};
inline Matrix<4, 4> Viewport{};
}

namespace camera {
constexpr Vector<3> eye   {-1,  0,  2};
constexpr Vector<3> center{ 0,  0,  0};
constexpr Vector<3> up    { 0,  1,  0};
}  // namespace camera
// clang-format on

#endif
