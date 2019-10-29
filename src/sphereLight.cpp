#include <random>
#include "sphereLight.hpp"

/****************************************************************
 * Helper functions                                             *
 ****************************************************************/

float generateRandomFloat(float a, float b)
{
  return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

/****************************************************************
 * SphereLight Class Implementation                                   *
 ****************************************************************/

SphereLight::SphereLight(Eigen::Vector3f spec, Eigen::Vector3f pos) : Light(spec, pos) {
  createSphereLight();
}

void SphereLight::createSphereLight() {
  std::cout << "[INFO] Then God said, \"Let there be light,\" and there was light.\n";
  std::mt19937 randomGenerator(LIGHT_SEED);
  std::srand(LIGHT_SEED);
  //Generate a random radius for this sphere light.
  sphericalRadius = RADIUS_LIMIT;//generateRandomFloat(1.0, RADIUS_LIMIT);
  //Generate a random amount of point lights which will be in this sphere light
  sphericalCount  = COUNT_LIMIT;//std::rand() % COUNT_LIMIT;

  std::uniform_real_distribution<float> uniform01(0.0, 1.0);
  std::vector<Eigen::Vector3f> newPointLights;
  for (int i = 0; i < this->sphericalCount; i++) {
    float theta = 2 * M_PI * uniform01(randomGenerator);
    float phi = acos(1 - 2 * uniform01(randomGenerator));
    float x = sin(phi) * cos(theta) * sphericalRadius;
    x += this->position[0];
    float y = sin(phi) * sin(theta) * sphericalRadius;
    y += this->position[1];
    float z = cos(phi) * sphericalRadius;
    z += this->position[2];

    Eigen::Vector3f newPointLight = Eigen::Vector3f(x, y, z);
    newPointLights.push_back(newPointLight);
  }
  this->sphericalLightsPos=newPointLights; 
}

std::vector<Eigen::Vector3f> SphereLight::getAllPointLights() {
  return sphericalLightsPos;
}
/* End of File */
