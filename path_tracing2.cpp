#include <omp.h>  //並列化計算
#include <iostream>
#include "accel.h"
#include "bvh.h"
#include "camera2.h"
#include "hit.h"
#include "image.h"
#include "light.h"
#include "material.h"
#include "random.h"
#include "ray.h"
#include "shape.h"
#include "sky.h"  //ibl入り
#include "util.h"
#include "vec3.h"
//ガンマ補正
const int MAX_DEPTH = 500;
const double ROULETTE = 0.99;
Accel accel;  //グローバルに定義する必要がある
Node* pNode;
Vec3 radiance(const Ray& init_ray, const Accel& accel, const Sky& sky,
              int depth = 0) {
  Vec3 col;
  Vec3 throughput(1, 1, 1);
  Ray ray = init_ray;
  for (int depth = 0; depth < MAX_DEPTH; depth) {
    Hit hit;
    if (accel.intersect(ray, pNode, hit)) {  // Diffuse
      Vec3 n = hit.hitNormal;
      Vec3 s, t;
      orthonormalBasis(n, s, t);
      Vec3 wo_local = worldTolocal(-1 * ray.direction, s, t, n);

      auto hitMaterial = hit.hitSphere->material;
      auto hitLight = hit.hitSphere->light;
      // col = col + throughput * hitLight->Le();
      Vec3 brdf;
      Vec3 wi_local;
      double pdf;

      brdf = hitMaterial->sample(wo_local, wi_local, pdf);
      double cos = cosTheta(wi_local);
      Vec3 wi = localToworld(wi_local, s, t, n);
      throughput = throughput * brdf * absCosTheta(wi_local) / pdf;
      ray = Ray(hit.hitPos /*+ 0.01 * hit.hitNormal*/, wi);
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
  const int N = 200;
  Image img(2102, 1500);
  PinholeCamera2 cam(Vec3(0, 0, 5), Vec3(0, 0, -1), 1);
  auto mat1 = std::make_shared<Mirror>(Vec3(0.2, 0.8, 0.2));
  auto mat2 = std::make_shared<Diffuse>(Vec3(0.8, 0.8, 0.2));
  auto mat3 = std::make_shared<Diffuse>(Vec3(0.8, 0.2, 0.8));
  auto mat4 = std::make_shared<Mirror>(Vec3(0.2, 0.2, 0.2));
  auto mat5 = std::make_shared<Glass>(1.5);
  auto light1 = std::make_shared<Light>(Vec3(1, 1, 1));
  auto light2 = std::make_shared<Light>(Vec3(0, 0, 0));
  // auto light3= std::make_shared<Light>(Vec3(5,5,5));

  // accel.add(
  //     std::make_shared<Cylinder>(Vec3(0, 0, 0), 1, 0.2, -1, mat1, light2));
  accel.add(std::make_shared<tri>(Vec3(-2, -1, 2), Vec3(-2, 0, -2),
                                  Vec3(-2, -1, -2), mat3, light2));
  accel.add(std::make_shared<tri>(Vec3(-2, -1, 2), Vec3(-2, 0, 2),
                                  Vec3(-2, -1, -2), mat3, light2));
  accel.add(std::make_shared<tri>(Vec3(-2, 0, 2), Vec3(-1, 0, 2),
                                  Vec3(-2, 0, -2), mat3, light2));
  accel.add(std::make_shared<tri>(Vec3(-1, 0, -2), Vec3(-1, 0, 2),
                                  Vec3(-2, 0, -2), mat3, light2));
  // accel.add(std::make_shared<Sphere>(Vec3(0, 0, 0), 1, mat3, light2));
  // accel.add(std::make_shared<Disk>(Vec3(0, 0.2, 0), 3, mat3, light2));
  // accel.add(std::make_shared<Sphere>(Vec3(10003, 0, 0), 10000, mat1,
  // light2));
  // accel.add(std::make_shared<Sphere>(Vec3(-10003, 0, 0), 10000, mat2,
  // light2)); accel.add(std::make_shared<Sphere>(Vec3(0, 10003, 0), 10000,
  // mat3, light2)); accel.add(std::make_shared<Sphere>(Vec3(0, 0, -10003),
  // 10000, mat3, light2));
  accel.add(std::make_shared<Sphere>(Vec3(0, -10001, 0), 10000, mat2, light2));
  // std::cout<<accel.shapes.size() << std::endl;

  ConstructBVH(&pNode, accel.shapes);
  IBL sky("PaperMill_E_3k.hdr");
  double screen_height = 2.0;
  double pixel_size = screen_height / img.height;

  for (int i = 0; i < img.width; i++) {
#pragma omp parallel for schedule(dynamic, 1)
    for (int j = 0; j < img.height; j++) {
      for (int k = 0; k < N; k++) {
        double u = screen_height * (2 * i - img.width) / (2 * img.height) +
                   screen_height * rnd() / img.height;
        double v = screen_height * (2 * j - img.height) / (2 * img.height) +
                   screen_height * rnd() / img.height;
        Ray ray = cam.getRay(u, v);
        Vec3 color = radiance(ray, accel, sky);
        //  Vec3 color2 = radiancenormal(color);

        img.setPixel(i, j,
                     img.getPixel(i, j) +
                         1 / (double)N * color);  // kでまわして平均をとる
      }
    }
    std::cout << progressbar(i, img.width) << " " << percentage(i, img.width)
              << "\r" << std::flush;
  }

  img.gamma_correction();
  img.ppm_output();
}
