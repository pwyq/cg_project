#include "scene.hpp"

Eigen::Vector3f Scene::traceRay(Ray ray, int level = 0) {
  //This variable will hold the hitable which the ray intersects first.
  Hitable hitObject;

  //This variable will hold the value of t on intersection in the formula r(t) = o + t * d 
  float tOnIntersection = std::numeric_limits<float>::infinity();

  for (auto &h: this.objectsInScene) {
  	if ( h.intersect(tOnIntersection, ray) ) {
  		hitObject = h;
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
Scene::Scene(const Tucano::Mesh &mesh, const Tucano::Material::Mtl &materials)
{
	this->materials = materials;
	this->objectsInScene.resize(mesh.getNumberOfFaces());
	for (size_t i = 0; i < mesh.getNumberOfFaces(); ++i)
		this->objectsInScene[i] = new Triangle(mesh.getFace(i));
}
