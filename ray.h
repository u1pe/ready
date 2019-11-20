#ifndef RAY_H
#define RAY_H
#include "vec3.h"
class Ray {
 public:
  Vec3 origin;
  Vec3 direction;
  double tmin = 0.001;
  Ray(const Vec3& origin, const Vec3& direction)
      : origin(origin), direction(direction){};
};

#endif