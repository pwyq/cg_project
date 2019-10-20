#pragma once

#include <Eigen/Dense>

class Ray
{
public:
	Eigen::Vector3f origin;
	Eigen::Vector3f dir;

	Eigen::Vector3f pointAt(const float &);

	Ray(const Eigen::Vector3f &, const Eigen::Vector3f &);
};

/**
 * @brief Template class for everything that can be hit with a Ray.
 */
class Hittable
{
public:
	//TODO define parameters for this.
	//virtual bool hit(const Ray &);
};

class Sphere : Hittable
{
public:
	Eigen::Vector4f pos;
	float r;

	//bool hit();

	Sphere(const Eigen::Vector4f &, const float &);
};
