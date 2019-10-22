#pragma once
#include "hitable.hpp"
#include "triangle.hpp"

#include <vector>

class Box : public Hitable {
public:
	Eigen::Vector3f bMin;
	Eigen::Vector3f bMax;

	std::vector<Hitable*> subBoxes;
	std::vector<Triangle*> triangles;
	bool isLeaf;

    // init Box
    Box(){};

    //Constructor creates a box around the given list of triangles
    Box(vector<Triangle*> &inputTriangles); 
    Box(Eigen::Vector3f x, Eigen::Vector3f y, bool isLeaf);

	bool intersect(float &hitPoint, Ray &ray);
};
