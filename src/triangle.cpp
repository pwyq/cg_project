#include "triangle.hpp"

#include <iostream>


bool Triangle::intersect(float &hitPoint, Ray &ray) {             
    //Find triangle's plane, plane equation: p * n - D = 0
    float D = this->normal.dot(vertex0);

    //Calculate intersection point of ray with triangle's plane
    float t = (D - ray.origin.dot(this->normal)) / ray.direction.dot(this->normal);

    //If t is negative this means the triange's plane is behind the ray
    if ( t < 0 ) return false;
    //If t is greater then max, it means that this face is behind some face we already intersected with, so skip this face.
    if ( t > hitPoint ) return false;  

	//Calculate the actual intersection point of the ray with the triangle's plane
    Eigen::Vector3f intersectionPoint = ray.getPoint(t);

    if ( isInTriangle(intersectionPoint) ) {
    	hitPoint = t;
    	return true;
    }
    return false;
}

bool Triangle::isInTriangle(Eigen::Vector3f point) {
    float areaABC = normal.dot( (vertex1 - vertex0).cross(vertex2 - vertex0) );
    float areaPBC = normal.dot( (vertex1 - point).cross(vertex2 - point) );
    float areaPCA = normal.dot( (vertex2 - point).cross(vertex0 - point) );

    float a = areaPBC / areaABC;
    float b = areaPCA / areaABC;	
    
    if ( a < 0 || a > 1 ) return false;
    if ( b < 0 || b > 1 ) return false;
    if ( a + b > 1 ) return false;
    return true;  
}

Triangle::Triangle(const Eigen::Vector3f &a,
                   const Eigen::Vector3f &b,
                   const Eigen::Vector3f &c,
                   const Eigen::Vector3f &norm,
                   const int id)
	: Hitable(norm, id)
{
	this->vertex0 = a;
	this->vertex1 = b;
	this->vertex2 = c;
    this->material_id = id;
}
