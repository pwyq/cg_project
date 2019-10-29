#include <random>
#include <cstdlib>

#include "light.hpp"

/****************************************************************
 * Light Class Implementation                                   *
 ****************************************************************/

// Constructor
Light::Light(Eigen::Vector3f spectrum, Eigen::Vector3f position) {
  this->spectrum = spectrum;
  this->position = position;
}

std::vector<Eigen::Vector3f> Light::getAllPointLights() {
  std::vector<Eigen::Vector3f> pointLights;
  pointLights.push_back(position);
  return pointLights;
}
/* End of File */
