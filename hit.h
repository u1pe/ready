#ifndef HIT_H
#define HIT_H
#include "vec3.h"

class Shape;  //前方宣言
class Hit {
 public:
  double t;
  Vec3 hitPos;
  Vec3 hitNormal;
  double u;
  double v;
  const Shape* hitSphere;
  double hitarea;

  Hit() {
    t = 1000000;
    hitSphere = nullptr;
  };
  Hit(double t, const Vec3& hitPos, const Vec3& hitNormal)
      : t(t), hitPos(hitPos), hitNormal(hitNormal){};
  //メンバ変数の初期化
};

#endif