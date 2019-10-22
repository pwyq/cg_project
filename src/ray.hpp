#pragma once

#include <Eigen/Dense>

class Ray {

public:
	Eigen::Vector3f origin;
	Eigen::Vector3f direction;

	Ray(Eigen::Vector3f origin, Eigen::Vector3f direction) {
		this->origin = origin;
		this->direction = direction;
	}
	Ray()
	{}

	Eigen::Vector3f getPoint(float t);
};
