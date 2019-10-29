#pragma once

#include "light.hpp"

class SphereLight : public Light {
  const int LIGHT_SEED = 42; // hardcoded random seed
  const int COUNT_LIMIT = 3;
  const int RADIUS_LIMIT = 0.2;
public:
  float sphericalRadius; //radius of sphere
  int sphericalCount;    //number of point lights in sphere
  std::vector<Eigen::Vector3f> sphericalLightsPos;

  /* Methods */
  SphereLight(Eigen::Vector3f spec, Eigen::Vector3f pos);
  void createSphereLight();
  virtual std::vector<Eigen::Vector3f> getAllPointLights();
};
