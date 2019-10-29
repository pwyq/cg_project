#include "ray.hpp"

Eigen::Vector3f Ray::getPoint(float t) {
  return origin + t * direction;
}
