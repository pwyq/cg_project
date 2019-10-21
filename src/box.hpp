#pragma once

#include "hitable.hpp"

class Box : public Hitable {
public:
	Eigen::Vector3f bMin;
	Eigen::Vector3f bMax;

	bool intersect(float &hitPoint, Ray &ray);

}
