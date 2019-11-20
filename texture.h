#ifndef TEXTURE_H
#define TEXTURE_H
#include <cmath>

#include "vec3.h"

class texture {
 public:
  virtual Vec3 getcolor1(double u, double v, const Vec3& p) const = 0;
};

class Uniformtexture : public texture {
  Vec3 color;
  Uniformtexture(const Vec3& color_) : color(color_){};
};

#endif