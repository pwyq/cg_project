#pragma once

#include <vector>

#include "hitable.hpp"
#include "triangle.hpp"

class TriangleList : public Hitable
{
public:
  std::vector<Triangle*> triangleList;
  Hitable *intersect(float &, Ray &, Hitable *);
  TriangleList(const std::vector<Triangle*> &);
  ~TriangleList();
};
