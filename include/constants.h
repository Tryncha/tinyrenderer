#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "tgaimage.h"

namespace constants {
constexpr int width{64};
constexpr int height{64};

constexpr double lower_limit{0.1};
constexpr double upper_limit{0.9};

namespace colors {
// clang-format off
// attention: order         B    G    R    A
constexpr TGAColor white  {255, 255, 255, 255};
constexpr TGAColor green  {  0, 255,   0, 255};
constexpr TGAColor red    {  0,   0, 255, 255};
constexpr TGAColor blue   {255, 128,  64, 255};
constexpr TGAColor yellow {  0, 200, 255, 255};
// clang-format on
}  // namespace colors
}  // namespace constants

#endif
