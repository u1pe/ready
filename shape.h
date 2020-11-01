#ifndef SHAPE_H
#define SHAPE_H
#include <math.h>
#include <iostream>
#include <memory>
#include "hit.h"
#include "light.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"

class AABB {
 public:
  double min[3];
  double max[3];
};
class Shape {
 public:
  std::shared_ptr<Material> material;
  std::shared_ptr<Light> light;
  AABB aabb;
  Shape(const std::shared_ptr<Material>& _material,
        const std::shared_ptr<Light>& _light)
      : material(_material), light(_light){};

  virtual bool intersect(const Ray& ray, Hit& hit) const = 0;
  // virtual Vec3 samplepoint(Vec3& p, double pdf_area) const = 0;
};

class Sphere : public Shape {
 public:
  Vec3 center;
  double radius;

  AABB aabb;

  Sphere(const Vec3& center, double radius,
         const std::shared_ptr<Material>& _material,
         const std::shared_ptr<Light>& _light)
      : center(center),
        radius(radius),
        Shape(_material, _light){
            /*aabb.max[0] = center.x + radius;
            aabb.max[1] = center.y + radius;
            aabb.max[2] = center.z + radius;
            aabb.min[0] = center.x - radius;
            aabb.min[1] = center.y - radius;
            aabb.min[2] = center.z - radius;*/
        };
  bool intersect(const Ray& ray, Hit& hit) const {
    double a = ray.direction.length2();
    double b = 2 * dot(ray.direction, ray.origin - center);
    double c = (ray.origin - center).length2() - radius * radius;
    double d = b * b - 4 * a * c;  //球のベクトル方程式を使っている

    if (d < 0) return false;

    double t1 = (-b - std::sqrt(d)) / (2 * a);
    double t2 = (-b + std::sqrt(d)) / (2 * a);
    if (t1 > 10000 || t2 < ray.tmin) return false;
    double t = t1;
    if (t < ray.tmin) {
      t = t2;
      if (t > 10000) return false;
    }
    hit.t = t;
    hit.hitPos = ray.origin + t * ray.direction;
    hit.hitNormal = normalize(hit.hitPos - center);

    hit.hitSphere = this;
    hit.hitarea = 4 * M_PI * radius * radius;

    return true;
  };

  /* Vec3 sampledisk(Vec3& p, double hit_area){
       Vec3 ShadowRay=
   }*/
};

// class Cylinder : public Shape {
//  public:
//   Vec3 center;
//   double radius;
//   double zmin;
//   double zmax;

//   // AABB aabb;
//   // x^2+y^2=r^2 (x=rcosθ,y=rsinθ)
//   // z=zmin+v(zmax-zmin)  (0<=v<=1)
//   Cylinder(const Vec3& center, double radius, double zmax, double zmin,
//            const std::shared_ptr<Material>& _material,
//            const std::shared_ptr<Light>& _light)
//       : center(center),
//         radius(radius),
//         zmax(zmax),
//         zmin(zmin),
//         Shape(_material, _light){
//             /* aabb.max[0]=std::max();
//              aabb.max[1]=center.y+radius;
//              aabb.max[2]=center.z+radius;
//              aabb.min[0]=center.x-radius;
//              aabb.min[1]=center.y-radius;
//              aabb.min[2]=center.z-radius;*/
//         };
//   bool intersect(const Ray& ray, Hit& hit) const {
//     double a = (ray.direction.x * ray.direction.x) +
//                (ray.direction.z * ray.direction.z);
//     double b = 2 * ((ray.direction.x * (ray.origin).x) +
//                     (ray.direction.z * (ray.origin).z));
//     double c = ((ray.origin).x * (ray.origin).x) +
//                ((ray.origin).z * (ray.origin).z) - radius * radius;

//     double d = b * b - 4 * a * c;

//     if (d < 0) {
//       return false;
//     }

//     double t1 = (-b + std::sqrt(d)) / (2 * a);
//     double t2 = (-b - std::sqrt(d)) / (2 * a);
//     //
//     td::cout<<((ray.origin-center)+t1*ray.direction).y<<","<<(ray.origin+t2*ray.direction).y<<std::endl;
//     double t = t2;
//     double hitz;

//     if (t2 < 0) {
//       t = t1;

//       if (t < 0) {
//         return false;
//       }
//     }

//     hitz = (ray.origin).y + t * ray.direction.y;
//     if (hitz < zmin || hitz > zmax) {
//       t = t1;
//       hitz = (ray.origin).y + t * ray.direction.y;
//       if (hitz < zmin || hitz > zmax) {
//         return false;
//       }
//     }

//     hit.t = t;
//     hit.hitPos = ray.origin + t * ray.direction;
//     Vec3 normal =
//         normalize(hit.hitPos - Vec3(center.x, hit.hitPos.y, center.z));
//     hit.hitNormal =
//         dot(ray.direction, hit.hitNormal) < 0 ? normal : -1 * normal;
//     // std::cout << dot(ray.direction, hit.hitNormal) << std::endl;
//     if (dot(ray.direction, hit.hitNormal) > 0) {
//       hit.hitNormal = hit.hitNormal * -1;
//     }
//     hit.hitarea = 2 * M_PI * radius * (zmax - zmin);
//     hit.hitSphere = this;

//     return true;
//   };
// };

class Disk : public Shape {
 public:
  Vec3 center;

  double radius;
  double zmin;
  double zmax;
  Disk(const Vec3& _center, double _radius,
       const std::shared_ptr<Material>& _material,
       const std::shared_ptr<Light>& _light)
      : center(_center), radius(_radius), Shape(_material, _light){};
  bool intersect(const Ray& ray, Hit& hit) const {
    double t = (center.y - ray.origin.y) / ray.direction.y;

    if (t <= 0) {
      return false;
    }
    if (std::pow(ray.origin.x + t * ray.direction.x - center.x, 2.0) +
            std::pow(ray.origin.z + t * ray.direction.z - center.z, 2.0) >=
        radius * radius) {
      return false;
    }
    if (ray.direction.y == 0) {
      return false;
    }

    hit.t = t;
    hit.hitPos = ray.origin + t * ray.direction + center;
    Vec3 a = normalize(hit.hitPos - ray.origin);

    hit.hitNormal = Vec3(0, a.y, 0);
    // std::cout << dot(ray.direction, hit.hitNormal) << std::endl;
    if (dot(ray.direction, hit.hitNormal) > 0) {
      hit.hitNormal = hit.hitNormal * -1;
    }
    hit.hitSphere = this;
    hit.hitarea = M_PI * radius * radius;
    return true;
  };
};
void cal_tri(const Vec3& m1, const Vec3& m2, const Vec3& m3,
             const Vec3& direction, const Vec3& origin, double& t, double& u,
             double& v) {
  Vec3 a = m2 - m1;
  Vec3 b = m3 - m1;
  Vec3 c = origin - m1;
  double k;
  k = dot(cross(direction, b), a);
  t = dot(cross(c, a), b) / k;
  u = dot(cross(direction, b), c) / k;
  v = dot(cross(c, a), direction) / k;
}
class tri : public Shape {
 public:
  Vec3 m1;
  Vec3 m2;
  Vec3 m3;
  AABB aabb;
  tri(const Vec3& _m1, const Vec3& _m2, const Vec3& _m3,
      const std::shared_ptr<Material>& _material,
      const std::shared_ptr<Light>& _light)
      : m1(_m1), m2(_m2), m3(_m3), Shape(_material, _light) {
    aabb.max[0] = std::max(m1.x, std::max(m2.x, m3.x));
    aabb.min[0] = std::min(m1.x, std::min(m2.x, m3.x));
    aabb.max[1] = std::max(m1.y, std::max(m2.y, m3.y));
    aabb.min[1] = std::min(m1.y, std::min(m2.y, m3.y));
    aabb.max[2] = std::max(m1.z, std::max(m2.z, m3.z));
    aabb.min[2] = std::min(m1.z, std::min(m2.z, m3.z));
  };

  bool intersect(const Ray& ray, Hit& hit) const {
    double t, u, v;
    cal_tri(m1, m2, m3, ray.direction, ray.origin, t, u, v);
    if (u + v <= 1 && u >= 0 && u <= 1 && v >= 0 && v <= 1 && t >= 0) {
      hit.t = t;
      hit.hitPos = ray.origin + t * ray.direction;
      hit.hitNormal = dot(cross(m2 - m1, m3 - m1), ray.direction) < 0
                          ? normalize(cross(m2 - m1, m3 - m1))
                          : -1 * normalize(cross(m2 - m1, m3 - m1));

      hit.hitSphere = this;
      hit.u = u / (u + v);
      hit.v = u + v;

      // hit.hitarea = 4 * M_PI * radius * radius;

      return true;
    }
    return false;
  };
};
#endif