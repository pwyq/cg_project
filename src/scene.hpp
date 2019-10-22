#pragma once

#include <vector>

#include <tucano/mesh.hpp>
#include <tucano/utils/mtlIO.hpp>

#include "ray.hpp"
#include "hitable.hpp"
#include "light.hpp"

class Scene {
public:
	std::vector<Hitable*> objectsInScene;
	std::vector<Light> lights;
	std::vector<Tucano::Material::Mtl>* materials;
	Eigen::Vector3f cameraPosition;

	//Will return a color
	void traceRay(Eigen::Vector3f *, Ray &, int);

	//Will return a color
	Eigen::Vector3f shade(Hitable &hitObject, const Ray &, float t);

	Eigen::Vector3f computeDirectLight(Hitable &hitObject, Eigen::Vector3f hitPoint);


	Scene(Tucano::Mesh &, std::vector<Tucano::Material::Mtl> &);
	Scene();
};
