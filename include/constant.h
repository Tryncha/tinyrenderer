#ifndef CONSTANT_H
#define CONSTANT_H

#include "matrix.h"
#include "tgaimage.h"

namespace constant {
constexpr int width{8192};
constexpr int height{8192};
}  // namespace constant

namespace color {
// clang-format off
// warning: order           B    G    R    A
constexpr TGAColor white  {255, 255, 255, 255};
constexpr TGAColor green  {  0, 255,   0, 255};
constexpr TGAColor red    {  0,   0, 255, 255};
constexpr TGAColor blue   {255, 128,  64, 255};
constexpr TGAColor yellow {  0, 200, 255, 255};
// clang-format on
}  // namespace color

#endif
