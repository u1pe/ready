#ifndef RANDOM_H
#define RANDOM_H
#include <iostream>
#include <random>

std::random_device rnd_dev;
std::mt19937 mt(rnd_dev());
std::uniform_real_distribution<> dist(0, 1);
inline double rnd() {
  return dist(mt);  //乱数生成
}

#endif