#ifndef SAMPLER_H
#define SAMPLER_H
#include "random.h"
#include "vec3.h"

Vec3 sampleDisk() {
  double theta = 2 * M_PI * rnd();
  return Vec3(std::cos(theta), std::sin(theta), 0);
}

Vec3 randomHemisphere(const Vec3& n) {
  double u = rnd();
  double v = rnd();

  double y = u;
  double x = std::sqrt(1 - u * u) * std::cos(2 * M_PI * v);
  double z = std::sqrt(1 - u * u) * std::sin(2 * M_PI * v);

  Vec3 xv, zv;
  orthonormalBasis(n, xv, zv);

  return x * xv + y * n + z * zv;
}
Vec3 randomsphere(double radius, const Vec3& center) {
  double y = radius * (2 * rnd() - 1);
  double radius2 = std::sqrt(radius * radius - y * y);
  double theta = rnd() * 2 * M_PI;
  double x = radius2 * std::cos(theta);
  double z = radius2 * std::sin(theta);
  return Vec3(x, y, z) + center;
}

Vec3 randomCosineHemisphere(double& pdf, const Vec3& n) {
  double u = rnd();
  double v = rnd();

  double theta = 0.5 * std::acos(1 - 2 * u);
  double phi = 2 * M_PI * v;
  pdf = 1 / M_PI * std::cos(theta);

  double x = std::cos(phi) * std::sin(theta);
  double y = std::cos(theta);
  double z = std::sin(phi) * std::sin(theta);

  Vec3 xv, zv;
  orthonormalBasis(n, xv, zv);

  return x * xv + y * n + z * zv;
}

inline Vec3 sampleSphere() {
  const float z = 1 - 2 * rnd();
  const float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
  const float phi = 2 * M_PI * rnd();
  return Vec3(r * std::cos(phi), z, r * std::sin(phi));
}
#endif