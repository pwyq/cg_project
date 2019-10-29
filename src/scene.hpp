#pragma once

// Tucano framework
#include <tucano/mesh.hpp>
#include <tucano/utils/mtlIO.hpp>

// General C++ library
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <chrono>

// Local header files
#include "ray.hpp"
#include "light.hpp"
#include "box.hpp"
#include "triangleList.hpp"

class Scene {
  const int MAX_LEVEL = 4;
public:
  /* Constructor */
  Scene(Tucano::Mesh &, std::vector<Tucano::Material::Mtl> &);
  Scene();

  /* Member Variable */
  Eigen::Vector3f cameraPosition;
  Box* boxOverAllTriangles;
  Hitable* structure;

  std::vector<Triangle*> trianglesInScene;
  std::vector<Light*> lights;
  std::vector<Tucano::Material::Mtl>* materials;

  //This is a list of all boxes (leafs of boxes tree). 
  //We have this to conform with the project requirements. We do not use it.
  // std::vector<Box*> leafBoxes;

  bool useAcc = true;
  bool useThreads = true;

  /* Methods */
  //Will return a color
  void traceRay(Eigen::Vector3f *, Ray &, int, Hitable *);

  //Will return a color
  inline Eigen::Vector3f shade(Hitable *, Ray &, float t, int level);

  //Will return a color
  inline Eigen::Vector3f computeDirectLight(Hitable *, Eigen::Vector3f);
  inline Eigen::Vector3f computeReflectedLight(Hitable *, Ray &, float , int);
  inline Eigen::Vector3f computeRefractedLight(Hitable *, Ray &, float , int , float , float);
};
