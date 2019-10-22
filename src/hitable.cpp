#include "hitable.hpp"

#include <iostream>

bool Hitable::intersect(float &hitPoint, Ray &ray)
{
	return false;
}

Hitable::Hitable(const Eigen::Vector3f &norm, const int id)
{
	normal = norm;
	material_id = id;
}

Hitable::Hitable()
{}
