#include "triangle.hpp"
#include <limits>
#include <iostream>

Triangle::Triangle(const Eigen::Vector3f &a,
                   const Eigen::Vector3f &b,
                   const Eigen::Vector3f &c,
                   const Eigen::Vector3f &norm,
                   const int id) : Hitable(norm, id)
{
	this->vertex0 = a;
	this->vertex1 = b;
	this->vertex2 = c;
    this->material_id = id;
}


float Triangle::getMaxCoordinate(int dimension) {
    float max = std::numeric_limits<float>::min();
    if ( vertex0(dimension) > max ) max = vertex0(dimension);
    if ( vertex1(dimension) > max ) max = vertex1(dimension);
    if ( vertex2(dimension) > max ) max = vertex2(dimension);
    return max;
}

float Triangle::getMinCoordinate(int dimension) {
    float min = std::numeric_limits<float>::max();
    if ( vertex0(dimension) < min ) min = vertex0(dimension);
    if ( vertex1(dimension) < min ) min = vertex1(dimension);
    if ( vertex2(dimension) < min ) min = vertex2(dimension);
    return min;
}

float Triangle::getMaxX() { return getMaxCoordinate(0); }
float Triangle::getMaxY() { return getMaxCoordinate(1); }
float Triangle::getMaxZ() { return getMaxCoordinate(2); }
float Triangle::getMinX() { return getMinCoordinate(0); }
float Triangle::getMinY() { return getMinCoordinate(1); }
float Triangle::getMinZ() { return getMinCoordinate(2); }

//Get the middlepoint of the triangle
Eigen::Vector3f Triangle::getPosition() {
    return (vertex0 + vertex1 + vertex2)/3.0;
}


Hitable* Triangle::intersect(float &hitPoint, Ray &ray) { 
    //Checks if the ray is parrallel to the triangle, then return NULL
    if ( this -> normal.dot(ray.direction) == 0 ) return NULL;

    //Find triangle's plane, plane equation: p * n - D = 0
    float D = this->normal.dot(vertex0);

    //Calculate intersection point of ray with triangle's plane
    float t = (D - ray.origin.dot(this->normal)) / ray.direction.dot(this->normal);

    //If t is negative this means the triange's plane is behind the ray
    if ( t < 0 ) return NULL;
    //If t is greater then max, it means that this face is behind some face we already intersected with, so skip this face.
    if ( t > hitPoint ) return NULL;  

    //Calculate the actual intersection point of the ray with the triangle's plane
    Eigen::Vector3f intersectionPoint = ray.getPoint(t);

    if ( isInTriangle(intersectionPoint) ) {
        hitPoint = t;
        return this;
    }
    return NULL;
}

bool Triangle::isInTriangle(Eigen::Vector3f point) {
    float areaABC = this->normal.dot( (vertex1 - vertex0).cross(vertex2 - vertex0) );
    float areaPBC = this->normal.dot( (vertex1 - point).cross(vertex2 - point) );
    float areaPCA = this->normal.dot( (vertex2 - point).cross(vertex0 - point) );

    float a = areaPBC / areaABC;
    float b = areaPCA / areaABC;    
    
    if ( a < 0 || b < 0 || a + b > 1 ) return false;
    return true;  
}
