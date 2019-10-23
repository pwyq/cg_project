#pragma once

#include <Eigen/Dense>

#include "hitable.hpp"

class Triangle : public Hitable {
public:
	Eigen::Vector3f vertex0;
	Eigen::Vector3f vertex1;
	Eigen::Vector3f vertex2;

	bool isInTriangle(Eigen::Vector3f point); 
	virtual Hitable* intersect(float &, Ray &);

	Triangle(const Eigen::Vector3f &,
	         const Eigen::Vector3f &,
	         const Eigen::Vector3f &,
	         const Eigen::Vector3f &,
	         const int);

	Eigen::Vector3f getPosition();

	float getMaxCoordinate(int dimension);
	float getMinCoordinate(int dimension);
	float getMaxX();
	float getMaxY();
	float getMaxZ();
	float getMinX();
	float getMinY();
	float getMinZ();
};
