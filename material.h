#ifndef MATERIAL_H
#define MATERIAL_H
#include <math.h>
#include "random.h"
#include "ray.h"
#include "sampler.h"
#include "vec3.h"

class Material {
 public:
  virtual Vec3 sample(const Vec3& wo, Vec3& wi, double& pdf) const = 0;
};

Vec3 worldTolocal(const Vec3& v, const Vec3& s, const Vec3& t, const Vec3& n) {
  return Vec3(dot(v, s), dot(v, n), dot(v, t));
}

Vec3 localToworld(const Vec3& v, const Vec3& s, const Vec3& t, const Vec3& n) {
  Vec3 a = Vec3(s.x, n.x, t.x);
  Vec3 b = Vec3(s.y, n.y, t.y);
  Vec3 c = Vec3(s.z, n.z, t.z);

  return Vec3(dot(v, a), dot(v, b), dot(v, c));
}

class Diffuse : public Material {
 public:
  Vec3 rho;  //反射率

  Diffuse(const Vec3& _rho) : rho(_rho){};

  Vec3 sample(const Vec3& wo, Vec3& wi, double& pdf) const {
    double u = rnd();
    double v = rnd();
    double theta = 0.5 * acos(1 - 2 * u);
    double phi = 2 * M_PI * v;
    double x = std::cos(phi) * std::sin(theta);
    double y = std::cos(theta);
    double z = std::sin(phi) * std::sin(theta);

    wi = Vec3(x, y, z);
    pdf = std::cos(theta) / M_PI;

    return rho / M_PI;
  }
};

double cosTheta(const Vec3& v) { return v.y; }
double absCosTheta(const Vec3& v) { return std::abs(v.y); }
double fresnel(const Vec3& v, const Vec3& n, double ior1, double ior2) {
  double f0 = std::pow((ior1 - ior2) / (ior1 + ior2), 2.0);
  double cos = absCosTheta(v);
  return f0 + (1 - f0) * std::pow(1 - cos, 5.0);
}

bool refract(const Vec3& wi, const Vec3& n, Vec3& wt, double ior1,
             double ior2) {
  double cos = absCosTheta(wi);
  double sin = std::sqrt(std::max(0.0, 1.0 - cos * cos));
  double alpha = ior1 / ior2 * sin;
  if (alpha * alpha > 1.0) {
    return false;
  }

  wt = ior1 / ior2 * ((-1 * wi) + +dot(wi, n) * n) -
       std::sqrt(1 - alpha * alpha) * n;
  return true;
}

class Mirror : public Material {
 public:
  Vec3 rho;
  Mirror(const Vec3& _rho) : rho(_rho){};

  Vec3 sample(const Vec3& wo, Vec3& wi, double& pdf) const {
    wi = reflect(wo, Vec3(0, 1, 0));
    pdf = 1.0;
    return 1 / cosTheta(wi) * rho;
  };
};

class Glass : public Material {
 public:
  double ior;

  Glass(double _ior) : ior(_ior){};

  Vec3 sample(const Vec3& wo, Vec3& wi, double& pdf) const {
    bool isEntering = cosTheta(wo) > 0;

    double n1;
    double n2;
    Vec3 normal;

    if (isEntering) {
      n1 = 1.0;
      n2 = ior;
      normal = Vec3(0, 1, 0);
    } else {
      n1 = ior;
      n2 = 1.0;
      normal = Vec3(0, -1, 0);
    }

    double fr = fresnel(wo, normal, n1, n2);

    if (rnd() < fr) {
      wi = reflect(wo, normal);

      pdf = fr;

      return fr / absCosTheta(wi) * Vec3(1, 1, 1);

    }

    else {
      if (refract(wo, normal, wi, n1, n2)) {
        pdf = 1 - fr;

        return std::pow(n1 / n2, 2.0) * (1 - fr) / absCosTheta(wi) *
               Vec3(1, 1, 1);
        ;
      } else {
        wi = reflect(wo, normal);

        pdf = 1 - fr;

        return (1 - fr) / absCosTheta(wi) * Vec3(1, 1, 1);
      }
    }
  };
};

#endif