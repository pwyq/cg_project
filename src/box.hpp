#pragma once

#include "hitable.hpp"

class Box : public Hitable {
public:
	Eigen::Vector3f bMin;
	Eigen::Vector3f bMax;

    // init Box
    Box(Eigen::Vector3f x, Eigen::Vector3f y) {
        this.bMin = x;
        this.bMax = y;
    }

	bool intersect(float &hitPoint, Ray &ray);
}
