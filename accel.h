#ifndef ACCEL_H
#define ACCEL_H
#include <memory>
#include <vector>
#include "hit.h"
#include "ray.h"
#include "shape.h"

// bvh用
/*bool ray_vs_aabb(const Ray& ray, const AABB& aabb) {
    double t_max = std::numeric_limits<double>::max();
    double t_min = std::numeric_limits<double>::min();

    double org[] = {ray.origin.x, ray.origin.y, ray.origin.z};
   //vecを配列で扱えるようにしたいね... double dir[] = {ray.direction.x,
   ray.direction.y, ray.direction.z};


    for(int i = 0; i < 3; i++) {
        double t1 = (aabb.min[i] - org[i])/dir[i];
        double t2 = (aabb.max[i] - org[i])/dir[i];

        double t_near = std::min(t1, t2);
        double t_far = std::max(t1, t2);

        t_max = std::min(t_max, t_far);
        t_min = std::max(t_min, t_near);

        if (t_min > t_max) return false;
    }
    return true;
       }*/
class Accel {
 public:
  std::vector<std::shared_ptr<Shape> > shapes;  //ポインタの配列
  std::vector<std::shared_ptr<Light> > lights;
  Accel(){};  //デフォルトコンストラクタ

  void add(std::shared_ptr<Shape> p) {
    shapes.push_back(p);  //配列の後ろに入れていく
  };
  void add2(std::shared_ptr<Light> l) {
    lights.push_back(l);  //配列の後ろに入れていく
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