#ifndef LIGHT_H
#define LIGHT_H
#include "vec3.h"

class Light {
 public:
    Vec3 color;

  Light(const Vec3& _color) : color(_color){};

  Vec3 Le() const { return color; };
};

#endif