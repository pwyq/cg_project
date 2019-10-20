#include <Eigen/Dense>

#include "primitives.hpp"

Eigen::Vector3f Ray::pointAt(const float &t)
{
	return origin + t * dir;
}

Ray::Ray(const Eigen::Vector3f &o, const Eigen::Vector3f &d)
{
	this->origin = o;
	this->dir = d;
}

Sphere::Sphere(const Eigen::Vector4f &p, const float &mr)
{
	this->pos = p;
	this->r = mr;
}
