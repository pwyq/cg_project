#include "triangle.hpp"


bool Triangle::intersect(float &hitPoint, Ray &ray) {             
    //Find triangle's plane, plane equation: p * n - D = 0
    float D = this.normal.dot(vertex0);

    //Calculate intersection point of ray with triangle's plane
    float t = (D - ray.origin.dot(this.normal)) / ray.direction.dot(this.normal);
    
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
    float areaABC = faceNormal.dot( (vertex1 - vertex0).cross(vertex2 - vertex0) );
    float areaPBC = faceNormal.dot( (vertex1 - point).cross(vertex2 - point) );
    float areaPCA = faceNormal.dot( (vertex2 - point).cross(vertex0 - point) );

    float a = areaPBC / areaABC;
    float b = areaPCA / areaABC;	
    
    if ( a < 0 || a > 1 ) return false;
    if ( b < 0 || b > 1 ) return false;
    if ( a + b > 1 ) return false;
    return true;  
}

/*

      //Check if intersection is inside the triangle. We do this by calculating the barycentric coordinates
      float areaABC = faceNormal.dot( (vertex1 - vertex0).cross(vertex2 - vertex0) );
      float areaPBC = faceNormal.dot( (vertex1 - intersectionPoint).cross(vertex2 - intersectionPoint) );
      float areaPCA = faceNormal.dot( (vertex2 - intersectionPoint).cross(vertex0 - intersectionPoint) );

      float a = areaPBC / areaABC;
      float b = areaPCA / areaABC;
      
      //If the intersection point is outside the triangle we can skip this face 
      if ( a < 0 || a > 1 ) continue;
      if ( b < 0 || b > 1 ) continue;
      if ( a + b > 1 ) continue;
      
      //When we reach this point, it means that the current face intersects with the ray and the intersection
      //point is closer to the intersection point we already found. Therefore we update the values "face" and "max".
      face = currentFace;
      max = t;
   */