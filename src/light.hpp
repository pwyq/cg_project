#pragma once

#include <iostream>
#include <vector>

#include <Eigen/Dense>
#include <tucano/camera.hpp>

class Light {
public:
  /* Constructor */
  // spectrum, position
  Light(Eigen::Vector3f, Eigen::Vector3f);

  /* Member Variables */
  Eigen::Vector3f spectrum;
  Eigen::Vector3f position;

  virtual std::vector<Eigen::Vector3f> getAllPointLights();
};

/* End of File */
