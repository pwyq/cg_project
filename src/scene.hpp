#pragma once

#include <vector>

#include <tucano/mesh.hpp>
#include <tucano/utils/mtlIO.hpp>

#include "ray.hpp"
#include "hitable.hpp"
#include "light.hpp"
#include "triangle.hpp"
#include "box.hpp"

class Scene {
public:
	std::vector<Triangle*> trianglesInScene;
	Box* boxOverAllTriangles;

	std::vector<Light> lights;
	std::vector<Tucano::Material::Mtl>* materials;
	Eigen::Vector3f cameraPosition;

	bool useAcc = true;

	//Will return a color
	void traceRay(Eigen::Vector3f *, Ray &, int);
	void traceRayWithAcc(Eigen::Vector3f *color, Ray &ray, int level);

	//Will return a color
	Eigen::Vector3f shade(Hitable &hitObject, const Ray &, float t);

	//Will return a color
	Eigen::Vector3f computeDirectLight(Hitable &hitObject, Eigen::Vector3f hitPoint);

	Scene(Tucano::Mesh &, std::vector<Tucano::Material::Mtl> &);
	Scene();
};
