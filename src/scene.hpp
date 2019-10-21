#include <vector>
#include "ray.hpp"

class Scene {
public:
	std::vector<Hitable> objectsInScene;
	std::vector<Light> lights;

	//Will return a color
	Eigen::Vector3f traceRay(Ray ray, int level);

	//Will return a color
	Eigen::Vector3f shade(Hitable hitObject, Ray ray, float t);


}