#include "hitable.hpp"

class Sphere : public Hitable {
public:

	Eigen::Vector3f center;
	float radius;

	bool intersect(float &hitPoint, Ray &ray);

};