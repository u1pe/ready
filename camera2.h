#ifndef CAMERA2_H
#define CAMERA2_H
#include "ray.h"
#include "sampler.h"
class Camera2 {
 public:
  Vec3 camPos;
  Vec3 camForward;
  Vec3 camRight;
  Vec3 camUp;

  Camera2(const Vec3& camPos, const Vec3& camForward)
      : camPos(camPos), camForward(camForward) {
    camRight = -1 * normalize(cross(camForward, Vec3(0, 1, 0)));
    camUp = normalize(cross(camForward, camRight));
  };

  virtual Ray getRay(double u, double v) const = 0;
};

class PinholeCamera2 : public Camera2 {
 public:
  double pinholeDist;
  PinholeCamera2(const Vec3& camPos, const Vec3& camForward,
                 double _pinholeDist)
      : Camera2(camPos, camForward), pinholeDist(_pinholeDist){};

  Ray getRay(double u, double v) const {
    Vec3 pinhole = camPos + pinholeDist * camForward;  //ピンホールの位置
    Vec3 sensorPos = camPos + u * camRight + v * camUp;
    Vec3 rayDir = normalize(pinhole - sensorPos);  //レイの方向
    return Ray(camPos, rayDir);
  };
};
class ThinLensCamera2 : public Camera2 {
 public:
   double lensDistance;
  Vec3 focusPoint;
  double objectDistance;
  double focalLength;
  double Fnumber;
  double lensRadius;
  Vec3 lensCenterPos;

  ThinLensCamera2(const Vec3& _camPos, const Vec3& _camForward,
                  double _lensDistance, const Vec3& _focusPoint,
                  double _Fnumber)
      : Camera2(_camPos, _camForward),
        lensDistance(_lensDistance),
        focusPoint(_focusPoint),
        Fnumber(_Fnumber) {
    objectDistance = (focusPoint - camPos).length() - lensDistance;
    focalLength = 1.0 / (1.0 / lensDistance + 1.0 / objectDistance);
    lensRadius = 0.5 * focalLength / Fnumber;
    lensCenterPos = camPos + lensDistance * camForward;
  };

  Ray getRay(double u, double v) const {
    //センサー面上の位置
    Vec3 sensorPos = camPos + u * camRight + v * camUp;
    //レンズ上の位置をサンプリング
    Vec3 l = sampleDisk();
    Vec3 lensPos = lensCenterPos + lensRadius * (l.x * camRight + l.y * camUp);

    //センサー面からレンズ中心までの方向ベクトル
    Vec3 sensorToLensCenter = normalize(lensCenterPos - sensorPos);
    //センサー面からオブジェクト面までの距離
    double sensorObjectDistance =
        (lensDistance + objectDistance) / dot(camForward, sensorToLensCenter);
    //オブジェクト面での位置
    Vec3 objectPos = sensorPos + sensorObjectDistance * sensorToLensCenter;

    //センサー面からレンズ面までの方向ベクトル
    Vec3 sensorToLensPos = normalize(lensPos - sensorPos);
    //センサー面からレンズ面までの距離の二乗
    double sensorLensDistance2 = (lensPos - sensorPos).length2();
    //寄与係数の計算
    // double w = lensDistance * lensDistance *
    //            std::pow(dot(camForward, sensorToLensPos), 2.0) /
    //            sensorLensDistance2;

    Vec3 rayDir = normalize(objectPos - lensPos);
    return Ray(lensPos, rayDir);
  };
};

#endif