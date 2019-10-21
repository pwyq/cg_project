#include <vector>
#include "ray.hpp"
#include "light.hpp"

class Scene {
public:
	std::vector<Hitable> objectsInScene;
	std::vector<Light> lights;
	std::vector<Tucano::Material::Mtl> materials;
	Eigen::Vector3f* cameraPosition;

	//Will return a color
	Eigen::Vector3f traceRay(Ray ray, int level);

	//Will return a color
	Eigen::Vector3f shade(Hitable hitObject, Ray ray, float t);

	Scene(const Tucano::Mesh &, const Tucano::Mareial::Mtl &);

	Eigen::Vector3f computeDirectLight(Hitable &hitObject, Eigen::Vector3f hitPoint);
}
