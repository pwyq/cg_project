#include "hitable.hpp"

#include <iostream>

Hitable* Hitable::intersect(float &hitPoint, Ray &ray)
{
	return NULL;
}

bool Hitable::isIntersecting(float &hitPoint, Ray &ray)
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
