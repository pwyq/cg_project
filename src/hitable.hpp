#pragma once

#include <Eigen/Dense>

#include "ray.hpp"

class Hitable {

public:
	virtual bool intersect(float &, Ray &);
	Eigen::Vector3f normal;

	Hitable();
	Hitable(const Eigen::Vector3f &);
};
