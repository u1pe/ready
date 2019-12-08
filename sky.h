#ifndef SKY_H
#define SKY_H
#include <string>
#include "ray.h"
#include "vec3.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

class Sky {
 public:
  virtual Vec3 getRadiance(const Ray& ray) const = 0;
};

class UniformSky : public Sky {
 public:
  Vec3 color;

  UniformSky(const Vec3& _color) : color(_color){};

  Vec3 getRadiance(const Ray& ray) const { return color; };
};

class Simplesky : public Sky {
 public:
  Simplesky(){};

  Vec3 getRadiance(const Ray& ray) const {
    double t = (ray.direction.y + 1) / 2;
    return (1 - t) * Vec3(1, 1, 1) + t * Vec3(0.7, 0.8, 1);
  };
};

class IBL : public Sky {
 public:
  int width;        //横幅
  int height;       //縦幅
  float* ibl_data;  // HDR画像データ

  //ファイル名から読み込む
  IBL(const std::string& filename) {
    int n;
    ibl_data = stbi_loadf(filename.c_str(), &width, &height, &n, 0);
  };
  ~IBL() { stbi_image_free(ibl_data); };

  //レイの方向からの放射輝度を返す
  Vec3 getRadiance(const Ray& ray) const {
    //球面座標系(phi, theta)を計算する
    double phi = std::atan2(ray.direction.z, ray.direction.x);
    if (phi < 0) phi += 2 * M_PI;
    float theta = std::acos(ray.direction.y);

    //(u, v)座標系に直す
    double u = phi / (2 * M_PI);
    double v = theta / M_PI;

    //画素のインデックスを計算
    int w = (int)(u * width);
    int h = (int)(v * height);
    int adr = 3 * w + 3 * width * h;
    return Vec3(ibl_data[adr], ibl_data[adr + 1], ibl_data[adr + 2]);
  };
};
#endif
#endif