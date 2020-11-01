#ifndef ACCEL2_H
#define ACCEL2_H
#include <memory>
#include <vector>
#include "hit.h"
#include "ray.h"
#include "shape.h"
#include "vec3.h"
class Accel2 {
 public:
  std::vector<std::shared_ptr<Shape> > shapes;
  std::vector<std::shared_ptr<Shape> > lights;  //ポインタの配列
  Accel2(){};  //デフォルトコンストラクタ

  void add(std::shared_ptr<Shape> p) {
    shapes.push_back(p);
    if (p->light->Le() != Vec3(0, 0, 0)) {
      lights.push_back(p);
    }
    //配列の後ろに入れていく
  };

  bool intersect(const Ray& ray, Hit& hit) const {
    bool isHit = false;

    hit.t = 10000000;  //距離を大きくとっておく
    Hit hit_each;
    for (auto p : shapes) {  // speareのクラスから　　繰り返す
      if (p->intersect(ray, hit_each)) {  //->ポインタ
        isHit = true;
        if (hit_each.t < hit.t) {
          hit = hit_each;  //一番短いものに置き換える
        }
      }
    }
    return isHit;
  };
};
#endif