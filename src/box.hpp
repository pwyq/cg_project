#pragma once
#include "hitable.hpp"
#include "triangle.hpp"

#include <vector>

class Box : public Hitable {
public:
	Eigen::Vector3f bMin;
	Eigen::Vector3f bMax;
	std::vector<Hitable*> children; //either smaller boxes or triangles
	bool isLeaf; //if true, children will contain triangles

    // init Box
    Box(){};

    //Constructor creates a box around the given list of triangles
    Box(vector<Triangle*> &inputTriangles); 
    
	bool intersect(float &hitPoint, Ray &ray);

	void splitBox(vector<Triangle*> &inputTriangles);

	std::vector<Box*> getLeafBoxes();

};
