#pragma once
#include <vector>
#include "hitable.hpp"

class Box : public Hitable {
public:
	Eigen::Vector3f bMin;
	Eigen::Vector3f bMax;

	std::vector<Hitable*> subBoxes;
	std::vector<Triangle*> triangles;
	bool isLeaf;

    // init Box
    Box(){
    };
    Box(Eigen::Vector3f x, Eigen::Vector3f y, bool isLeaf);

	bool intersect(float &hitPoint, Ray &ray);

};
