#include "hitable.hpp"

class Triangle : public Hitable {
public:
	Eigen::Vector3f vertex0;
	Eigen::Vector3f vertex1;
	Eigen::Vector3f vertex2;
	Eigen::Vector3f normal;

	void getBarycentricCoordinates(Eigen::Vector3f point, int &a, int &b); 

	bool intersect(float &hitPoint, Ray &ray);
}