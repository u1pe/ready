#ifndef ACCEL_H
#define ACCEL_H
#include <memory>
#include <vector>
#include "bvh.h"
#include "hit.h"
#include "ray.h"
#include "shape.h"

// bvh用
bool ray_vs_aabb(const Ray& ray, const AABB& aabb) {
  double t_max = std::numeric_limits<double>::max();
  double t_min = std::numeric_limits<double>::min();

  double org[] = {ray.origin.x, ray.origin.y, ray.origin.z};
  double dir[] = {ray.direction.x, ray.direction.y, ray.direction.z};
  for (int i = 0; i < 3; i++) {
    double t1 = (aabb.min[i] - org[i]) / dir[i];
    double t2 = (aabb.max[i] - org[i]) / dir[i];
    double t_near = std::min(t1, t2);
    double t_far = std::max(t1, t2);

    t_max = std::min(t_max, t_far);
    t_min = std::max(t_min, t_near);

    if (t_min > t_max) return false;
  }
  return true;
}
class Accel {
 public:
  std::vector<std::shared_ptr<Shape> > shapes;  //ポインタの配列
  // std::vector<std::shared_ptr<Light> > lights;
  Accel(){};  //デフォルトコンストラクタ

  void add(std::shared_ptr<Shape> p) {
    shapes.push_back(p);  //配列の後ろに入れていく
  };
  // void add2(std::shared_ptr<Light> l) {
  //   lights.push_back(l);  //配列の後ろに入れていく
  // };

  bool intersect(const Ray& ray, const Node* node, Hit& hit) const {
    bool is_hit_aabb = ray_vs_aabb(ray, node->aabb);
    // std::cout << is_hit_aabb << std::endl;
    if (!is_hit_aabb) {
      return false;
    } else {
      if (node->node_state == 0) {
        Hit hit_each;
        bool is_hit = false;
        for (auto& p : node->objects) {
          if (p->intersect(ray, hit_each)) {
            if (hit_each.t < hit.t) {
              hit = hit_each;
            }
            is_hit = true;
          }
        }
        return is_hit;
      } else {
        bool is_hit1 = intersect(ray, node->left, hit);
        bool is_hit2 = intersect(ray, node->right, hit);

        return is_hit2 || is_hit1;
      }
    }
  }
};
#endif