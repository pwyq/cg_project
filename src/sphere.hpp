#pragma once

#include "hitable.hpp"

class Sphere : public Hitable {
public:

	Eigen::Vector3f center;
	float radius;

    // TODO: need to change return type to Hitable
	// virtual bool intersect(float &hitPoint, Ray &ray);

};
