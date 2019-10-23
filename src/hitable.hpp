#pragma once

#include <Eigen/Dense>

#include "ray.hpp"

class Hitable {

public:
	virtual Hitable* intersect(float &, Ray &);
	Eigen::Vector3f normal;
	int material_id;

	Hitable();
	Hitable(const Eigen::Vector3f &, const int id);
};
