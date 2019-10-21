#include "ray.hpp"


class Hitable {

public:
	int material_id;
	Eigen::Vector3f normal;
	
	bool intersect(float &hitPoint, Ray &ray);

};