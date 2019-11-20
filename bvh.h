#ifndef BVH_H
#define BVH_H
//物体の数によってスピードに差が出る、xとlogxのような

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include "shape.h"

//  class Node{

/* public:
     int node_state;
     std::vector<std::shared_ptr<Shape>> objects;
     AABB aabb;
     Node* left;
     Node* right;
};
void MakeAABB(const std::vector<std::shared_ptr<Shape>>& shapes, AABB& aabb) {
 for(int i = 0; i < 3; i++) {
     aabb.max[i] = shapes[0]->aabb.max[i];//->はなんの役割を持つのか？
     aabb.min[i] = shapes[0]->aabb.min[i];
 }
 for(int i = 1; i < spheres.size(); i++) {
     for(int j = 0; j < 3; j++) {
         aabb.min[j] = std::min(aabb.min[j], spheres[i]->aabb.min[j]);
         aabb.max[j] = std::max(aabb.max[j], spheres[i]->aabb.max[j]);
     }
 }
}
void SplitObjects(const AABB& aabb,
               std::vector<std::shared_ptr<Shape>>& shapes,
               std::vector<std::shared_ptr<Shape>>& lshapes,
               std::vector<std::shared_ptr<Shape>>& rshapes) {

 int offset = std::numeric_limits<int>::max();
 std::vector<std::shared_ptr<Shape>> bestsplit_array;
 int bestsplit_index;

 for(int i = 0; i < 3; i++) {

     double half = (aabb.max[i] + aabb.min[i])/2.0;
     auto k = std::partition(shapes.begin(), shapes.end(),
             [i,half](std::shared_ptr<Shape>& shape) {
                 return (shape->aabb.min[i] + shape->aabb.max[i])/2.0 <= half;
             });

     int b = spheres.size()/2 - (k - spheres.begin());
     b = std::abs(b);

     if (b < offset) {

         offset = b;
         bestsplit_array = spheres;
         bestsplit_index = k - spheres.begin();
     }
 }

 lspheres = std::vector<std::shared_ptr<Sphere>>(bestsplit_array.begin(),
bestsplit_array.begin() + bestsplit_index); rspheres =
std::vector<std::shared_ptr<Sphere>>(bestsplit_array.begin() + bestsplit_index,
bestsplit_array.end());
}




void ConstructBVH(Node** tree,std::vector<std::shared_ptr<Sphere>>& spheres){
 assert(spheres.size()>0);

 Node* pNode=new Node;
 *tree =pNode;

 MakeAABB(spheres,pNode->aabb);
 const int n=4;
 if(spheres.size() <= n) {//②
     pNode->node_state = 0;
     pNode->objects = spheres;
 }
 else {
     pNode->node_state = 1;

     std::vector<std::shared_ptr<Sphere>> rspheres, lspheres;
     SplitObjects(pNode->aabb, spheres, lspheres, rspheres);

     if(lspheres.size() == 0 || rspheres.size() == 0) {
         pNode->node_state = 0;
         pNode->objects = spheres;
         return;
     }

     ConstructBVH(&(pNode->left), lspheres);
     ConstructBVH(&(pNode->right), rspheres);
 }
}







*/
#endif