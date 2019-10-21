#include "scene.hpp"
#include "triangle.hpp"
#include "hitable.hpp"

#include <iostream>

Eigen::Vector3f Scene::traceRay(Ray &ray, int level) {
  //This variable will hold the hitable which the ray intersects first.
  Hitable hitObject;

  //This variable will hold the value of t on intersection in the formula r(t) = o + t * d 
  float tOnIntersection = std::numeric_limits<float>::infinity();

  for (auto &h: this->objectsInScene) {
  	if ( h->intersect(tOnIntersection, ray) ) {
  		hitObject = *h;
  	}
  }

  //If tOnIntersection is still infinity, it means this ray did not hit anything, so we return the background color
  if ( tOnIntersection == std::numeric_limits<float>::infinity() ) return Eigen::Vector3f(1.0,1.0,1.0);

  //If we reach this point, it means the ray hitted a object. Now we should compute the color of this object, so we call the shade method.
  std::cout << "Found intersection between triangle and ray" << std::endl;
  //TODO
  //Eigen::Vector3f color;
  //shade(1, hitface, origin+tOnIntersection*rayDirection, color, origin, dest);
  //return color;
  return Eigen::Vector3f(0,0,0);
}

//TODO For now this only copies triangles
Scene::Scene(Tucano::Mesh &mesh, std::vector<Tucano::Material::Mtl> &materials)
{
	this->materials = &materials;
//	this->objectsInScene.resize(mesh.getNumberOfFaces());
	for (size_t i = 0; i < mesh.getNumberOfFaces(); ++i)
	{
		Tucano::Face f = mesh.getFace(i);
		int a = f.vertex_ids[0];
		int b = f.vertex_ids[1];
		int c = f.vertex_ids[2];
		this->objectsInScene.push_back(new Triangle(mesh.getVertex(a).head<3>(),
		                                       mesh.getVertex(b).head<3>(),
		                                       mesh.getVertex(c).head<3>(),
		                                       f.normal.head<3>()));
	}
}

Scene::Scene()
{}
