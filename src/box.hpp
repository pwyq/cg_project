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

    // default Box constructor
    Box();

    //Constructor creates a box around the given list of triangles
    Box(std::vector<Triangle*> &inputTriangles); 
    
	virtual Hitable* intersect(float &hitPoint, Ray &ray);
	virtual bool isIntersecting(float &hitPoint, Ray &ray);


	void splitBox(std::vector<Triangle*> &inputTriangles);

	std::vector<Box*> getLeafBoxes();

};
