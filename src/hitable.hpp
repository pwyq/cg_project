#pragma once

#include <Eigen/Dense>

#include "ray.hpp"

class Hitable {

public:
	virtual Hitable* intersect(float &, Ray &);
	virtual bool isIntersecting(float &hitPoint, Ray &ray);

	Eigen::Vector3f normal;
	int material_id;

	Hitable();
	Hitable(const Eigen::Vector3f &, const int id);
};
