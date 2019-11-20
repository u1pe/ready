#include <omp.h>  //並列化計算
#include <iostream>
#include <vector>
#include "accel.h"
#include "camera2.h"
#include "hit.h"
#include "image.h"
#include "light.h"
#include "material.h"
#include "random.h"
#include "ray.h"
#include "sampler.h"
#include "shape.h"
#include "sky.h"
#include "util.h"
#include "vec3.h"
//ガンマ補正
const int MAX_DEPTH = 500;
const double ROULETTE = 0.99;
Accel accel;  //グローバルに定義する必要がある

Simplesky sky;

Vec3 radianceNEE(const Ray& init_ray, const Accel& accel, const Sky& sky,
                 /*const std::shared_ptr<Light>& lights,*/ int depth = 0) {
  Vec3 col;
  Vec3 throughput(1, 1, 1);
  Ray ray = init_ray;
  for (int depth = 0; depth < MAX_DEPTH; depth) {
    Hit hit;
    if (accel.intersect(ray, hit)) {  // Diffuse
      Vec3 n = hit.hitNormal;
      Vec3 s, t;
      orthonormalBasis(n, s, t);
      Vec3 wo_local = worldTolocal(-1 * ray.direction, s, t, n);

      auto hitMaterial = hit.hitSphere->material;
      auto hitLight = hit.hitSphere->light;
      col = col + throughput * hitLight->Le();
      Vec3 brdf;
      Vec3 wi_local;
      double pdf;

      brdf = hitMaterial->sample(wo_local, wi_local, pdf);
      double cos = cosTheta(wi_local);
      Vec3 wi = localToworld(wi_local, s, t, n);
      Vec3 a = sampleSphere() * 1 + Vec3(0, 3, 0);
      Vec3 nee = normalize(a - hit.hitPos);
      double posit = hit.hitPos.x * hit.hitPos.x +
                     (hit.hitPos.y - 3) * (hit.hitPos.y - 3) +
                     hit.hitPos.z * hit.hitPos.z - 1;
      if (posit > -1e-4) {
        Ray shadowray = Ray(hit.hitPos + 0.01 * hit.hitNormal, nee);

        Hit hit2;
        bool shadow_hit = accel.intersect(shadowray, hit2);

        Vec3 b = normalize(a - Vec3(0, 3, 0));
        double k = (a - hit.hitPos).length();
        double m = (hit2.hitPos - hit.hitPos).length();
        // std::cout << k - m << std::endl;

        if (shadow_hit && std::abs(k - m) < 1e-3) {
          double cos2 = std::abs(dot(nee, b));  ///(nee.length()*b.length());
          double pdf_area = 1 / hit2.hitarea;
          double cos3 = std::abs(dot(nee, hit.hitNormal));
          double pdf_solid = (cos3 * cos2) / (k * k);
          // std::cout << pdf_solid << std::endl;
          auto hitLight2 = hit2.hitSphere->light;
          col =
              col + throughput * brdf * pdf_solid * hitLight2->Le() / pdf_area;
        }
      }
      throughput = throughput * brdf * cos / pdf;
      ray = Ray(hit.hitPos + 0.01 * hit.hitNormal, wi);
      //+ 0.001 * hit.hitNormal,
      // std::cout<<ray.direction<<std::endl;
    } else {
      col = col + throughput * sky.getRadiance(ray);
      break;
    }

    if (rnd() < ROULETTE) {
      throughput = throughput / ROULETTE;
    } else {
      break;
    }
  }
  return col;
}

Vec3 radiance(const Ray& init_ray, const Accel& accel, const Sky& sky,
              int depth = 0) {
  Vec3 col;
  Vec3 throughput(1, 1, 1);
  Ray ray = init_ray;
  for (int depth = 0; depth < MAX_DEPTH; depth) {
    Hit hit;
    if (accel.intersect(ray, hit)) {  // Diffuse
      Vec3 n = hit.hitNormal;
      Vec3 s, t;
      orthonormalBasis(n, s, t);
      Vec3 wo_local = worldTolocal(-1 * ray.direction, s, t, n);

      auto hitMaterial = hit.hitSphere->material;
      auto hitLight = hit.hitSphere->light;
      col = col + throughput * hitLight->Le();
      Vec3 brdf;
      Vec3 wi_local;
      double pdf;
      brdf = hitMaterial->sample(wo_local, wi_local, pdf);
      double cos = cosTheta(wi_local);
      Vec3 wi = localToworld(wi_local, s, t, n);
      throughput = throughput * brdf * cos / pdf;
      ray = Ray(hit.hitPos + 0.01 * hit.hitNormal, wi);
      //+ 0.001 * hit.hitNormal,
      // std::cout<<ray.direction<<std::endl;
    } else {
      col = col + throughput * sky.getRadiance(ray);
      break;
    }

    if (rnd() < ROULETTE) {
      throughput = throughput / ROULETTE;
    } else {
      break;
    }
  }
  return col;
}

Vec3 radiancenormal(Vec3& col) {
  Hit hit;
  Vec3 color = col + (hit.hitNormal + Vec3(1, 1, 1)) * 0.5;
  return color;
}

int main() {
  const int N = 100;
  Image img(600, 400);
  PinholeCamera2 cam(Vec3(0, 0, 3), Vec3(0, 0, -1), 1);
  auto mat1 = std::make_shared<Diffuse>(Vec3(0.8, 0.2, 0.8));
  auto mat2 = std::make_shared<Diffuse>(Vec3(0.8, 0.8, 0.2));
  auto mat3 = std::make_shared<Diffuse>(Vec3(0.8, 0.8, 0.8));
  auto mat4 = std::make_shared<Diffuse>(Vec3(0.2, 0.2, 0.8));
  auto mat5 = std::make_shared<Diffuse>(Vec3(0.2, 0.8, 0.2));
  // auto mat5 = std::make_shared<Glass>(1.5);
  auto light1 = std::make_shared<Light>(Vec3(5, 5, 5));
  auto light2 = std::make_shared<Light>(Vec3(0, 0, 0));
  // auto light3 = std::make_shared<Light>(Vec3(0, 0, 0));
  // auto light3= std::make_shared<Light>(Vec3(5,5,5));

  // accel.add(
  // std::make_shared<Cylinder>(Vec3(0, 0, 0), 3, 0.2, -1, mat2, light2));
  accel.add(std::make_shared<Sphere>(Vec3(0, 3, 0), 1, mat3, light1));
  accel.add(std::make_shared<Sphere>(Vec3(0, 0, 0), 1, mat2, light2));
  // accel.add(std::make_shared<Disk>(Vec3(0, 0.2, 0), 3, mat3, light2));
  accel.add(std::make_shared<Sphere>(Vec3(10003, 0, 0), 10000, mat1, light2));
  accel.add(std::make_shared<Sphere>(Vec3(-10003, 0, 0), 10000, mat4, light2));
  accel.add(std::make_shared<Sphere>(Vec3(0, 10003, 0), 10000, mat3, light2));
  accel.add(std::make_shared<Sphere>(Vec3(0, 0, -10003), 10000, mat3, light2));
  accel.add(std::make_shared<Sphere>(Vec3(0, -10001, 0), 10000, mat3,
                                     light2));  //地面を作るイメージ！
  // std::cout<<accel.shapes.size() << std::endl;
  double screen_height = 2.0;
  double pixel_size = screen_height / img.height;
#pragma omp　parallel for
  for (int k = 0; k < N; k++) {
    for (int i = 0; i < img.width; i++) {
      for (int j = 0; j < img.height; j++) {
        double u = screen_height * (2 * i - img.width) / (2 * img.height) +
                   screen_height * rnd() / img.height;
        double v = screen_height * (2 * j - img.height) / (2 * img.height) +
                   screen_height * rnd() / img.height;
        // std::cout << rnd() << std::endl;
        Ray ray = cam.getRay(u, v);
        Vec3 color = radianceNEE(ray, accel, sky);
        Vec3 color2 = radiancenormal(color);

        img.setPixel(
            i, j,
            img.getPixel(i, j) + 1.0 / N * color);  // kでまわして平均をとる
      }
    }
    std::cout << progressbar(k, N) << " " << percentage(k, N) << "\r"
              << std::flush;
  }

  img.gamma_correction();
  img.ppm_output();
}
